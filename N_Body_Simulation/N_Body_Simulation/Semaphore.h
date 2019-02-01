#pragma once


#include <mutex>

using std::condition_variable;
using std::mutex;
using std::unique_lock;

class Semaphore :
	public mutex
{
public:
	Semaphore(int count = 0);
	~Semaphore();

	inline void SetCounter(int newCount) { counter_ = newCount; }

	void Wait();
	void Signal(int n = 1);

	void SignalAll();

private:

	int counter_;

	mutex counterMutex_;
	condition_variable lock_;
};

