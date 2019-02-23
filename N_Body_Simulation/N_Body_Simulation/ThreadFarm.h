#pragma once

// standard library includes
#include <queue>
#include <thread>
#include <vector>
#include <functional>

// my class includes
#include "Task.h"
#include "Semaphore.h"


class ThreadFarm
{
public:
	ThreadFarm();
	~ThreadFarm();

	// Method to start and Stop the farm
	void Run();
	void End();


	// Method causes thread to wait until all the current tasks in the farm are finished
	void WaitUntilTasksFinished();


	// Adds a new Task to the farm
	void AddTask(Task* newTask);


	// Setters
	inline void SetThreadCount(unsigned int newCount) { threadCount_ = newCount; }
	

	// Getters
	inline int NumWorkers() { return threads_.size(); }

private:

	// worker method
	// loop that each worker thread follows to compute tasks the farm has been given
	void Worker();


	// vector containing all the threads 
	std::vector<std::thread*> threads_;

	// Queue of tasks the farm needs to perform
	std::queue<Task*> tasks_;

	// semaphore and task mutex control access to tasks queue
	Semaphore task_semaphore_;
	std::mutex task_mutex_;

	// mutex and condition variable to control a thread waiting for all tasks to be finished
	std::mutex allTasksFinishedMutex_;
	std::condition_variable tasksFinishedLock_;


	// number of threads the farm will use
	unsigned int threadCount_;

	// number of tasks currently running
	int tasksRunning_;


	// flag saying if farm is still running
	bool running_;
};

