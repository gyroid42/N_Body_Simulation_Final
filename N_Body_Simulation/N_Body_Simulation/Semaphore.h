#pragma once

// include parent class
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

	void Wait();
	void Signal(int n = 1);

	void SignalAll();


	inline void SetCounter(int newCount) { counter_ = newCount; }
	inline void ReduceCounter(int amount = 1) { counter_ -= amount; }

private:

	int counter_;

	mutex counterMutex_;
	condition_variable lock_;
};

