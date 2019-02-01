#include "Semaphore.h"



Semaphore::Semaphore(int count)
{
	counter_ = count;
}


Semaphore::~Semaphore()
{
}


void Semaphore::Wait() {

	while (true) {

		if (counter_ <= 0) {

			unique_lock<mutex> lck(*this);
			lock_.wait(lck);
		}

		unique_lock<mutex> lck(counterMutex_);

		if (counter_ <= 0) {
			continue;
		}

		counter_--;
		break;
	}
}


void Semaphore::Signal(int n) {

	{
		unique_lock<mutex> lck(counterMutex_);
		counter_ += n;
	}

	for (int i = 0; i < n; i++) {

		lock_.notify_one();
	}
}


void Semaphore::SignalAll() {

	unique_lock<mutex> lck(counterMutex_);
	counter_ += 20;
	lock_.notify_all();
}
