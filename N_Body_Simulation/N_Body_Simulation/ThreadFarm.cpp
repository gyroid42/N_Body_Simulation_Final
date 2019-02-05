
// class header include
#include "ThreadFarm.h"



ThreadFarm::ThreadFarm()
{
	threadCount_ = 0;
	running_ = false;
	tasksRunning_ = 0;
}


ThreadFarm::~ThreadFarm()
{
}


void ThreadFarm::AddTask(Task* task) {

	// lock access to tasks queue
	unique_lock<mutex> lck(task_mutex_);

	// add a new task to the queue and signal the task semaphore
	tasks_.push(task);
	task_semaphore_.Signal();
}

void ThreadFarm::Run() {

	// only start if farm is not already running
	if (!running_) {

		running_ = true;

		// add a new thread until thread count is reached
		for (unsigned int i = 0; i < threadCount_; i++) {

			threads_.push_back(new std::thread(std::mem_fun(&ThreadFarm::Worker), this));
		}
	}
}


void ThreadFarm::Worker() {

	// Main worker loop
	while (true) {

		Task* task = nullptr;

		// wait until a task is available
		task_semaphore_.Wait();

		// if farm no longer running exit the loop
		if (!running_) {

			break;
		}


		{
			// access the tasks queue get the next task
			unique_lock<mutex> lock(task_mutex_);

			task = tasks_.front();
			tasks_.pop();
			tasksRunning_++;
		}

		// run the task then delete it
		task->Run();
		delete task;

		{
			// if last task of farm finished then notify the tasks finished mutex
			unique_lock<mutex> lock(task_mutex_);
			tasksRunning_--;
			if (tasksRunning_ == 0 && tasks_.size() == 0) {

				tasksFinishedLock_.notify_all();
			}
		}
	}
}


void ThreadFarm::End() {

	// set farm to not running
	running_ = false;


	// signal all the workers to leave their loops
	task_semaphore_.SignalAll();


	// join all the threads
	for (auto thread : threads_) {

		thread->join();
	}

	// delete threads used
	for (auto thread : threads_) {

		delete thread;
		thread = nullptr;
	}

	// delete all remaining tasks
	while (!tasks_.empty()) {

		delete tasks_.front();
		tasks_.front() = nullptr;
		tasks_.pop();
	}

}


void ThreadFarm::WaitUntilTasksFinished() {

	// if tasks size > 0 then still tasks to complete
	if (tasks_.size() > 0) {

		unique_lock<mutex> lck(allTasksFinishedMutex_);
		tasksFinishedLock_.wait(lck);
	}
}
