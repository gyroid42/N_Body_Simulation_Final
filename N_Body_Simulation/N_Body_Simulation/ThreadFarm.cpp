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

	unique_lock<mutex> lck(task_mutex_);

	tasks_.push(task);
	task_semaphore_.Signal();
}

void ThreadFarm::Run() {

	if (!running_) {

		running_ = true;

		for (unsigned int i = 0; i < threadCount_; i++) {

			threads_.push_back(new std::thread(std::mem_fun(&ThreadFarm::Worker), this));
		}
	}
}


void ThreadFarm::Worker() {

	while (true) {

		Task* task = nullptr;

		task_semaphore_.Wait();

		if (!running_) {

			break;
		}


		{

			unique_lock<mutex> lock(task_mutex_);

			task = tasks_.front();
			tasks_.pop();
			tasksRunning_++;
		}

		task->Run();
		delete task;

		{
			unique_lock<mutex> lock(task_mutex_);
			tasksRunning_--;
			if (tasksRunning_ == 0 && tasks_.size() == 0) {

				tasksFinishedLock_.notify_all();
			}
		}
	}
}


void ThreadFarm::End() {

	running_ = false;

	task_semaphore_.SignalAll();


	for (auto thread : threads_) {

		thread->join();
	}

}


void ThreadFarm::WaitUntilTasksFinished() {

	if (tasks_.size() > 0) {

		unique_lock<mutex> lck(allTasksFinishedMutex_);
		tasksFinishedLock_.wait(lck);
	}
}
