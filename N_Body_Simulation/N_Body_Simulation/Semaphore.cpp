
// class header include
#include "Semaphore.h"



Semaphore::Semaphore(int count)
{
	counter_ = count;
}


Semaphore::~Semaphore()
{
}


void Semaphore::Wait() {


	std::unique_lock<decltype(mutex_)> lock(mutex_);
	while (!counter_) {
		condition_.wait(lock);
	}

	counter_--;

}


void Semaphore::Signal() {


	std::lock_guard<decltype(mutex_)> lock(mutex_);
	counter_++;
	condition_.notify_one();

}


void Semaphore::SignalAll() {

	unique_lock<mutex> lck(mutex_);
	counter_ += 20;
	condition_.notify_all();
}
