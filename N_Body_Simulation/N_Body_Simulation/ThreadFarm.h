#pragma once

#include "Task.h"
#include "Semaphore.h"
#include <queue>
#include <thread>
#include <vector>
#include <functional>

class ThreadFarm
{
public:
	ThreadFarm();
	~ThreadFarm();



	void AddTask(Task* newTask);


	void Run();
	void End();


	inline void SetThreadCount(unsigned int newCount) { threadCount_ = newCount; }
	
	void WaitUntilTasksFinished();

private:

	void Worker();

	unsigned int threadCount_;


	std::vector<std::thread*> threads_;
	std::queue<Task*> tasks_;

	Semaphore task_semaphore_;
	std::mutex task_mutex_;

	std::mutex allTasksFinishedMutex_;
	std::condition_variable tasksFinishedLock_;
	int tasksRunning_;

	bool running_;
};

