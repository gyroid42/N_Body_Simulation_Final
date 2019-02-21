#pragma once


#include "Semaphore.h"
#include <list>


template<class T>
class Channel
{
public:


	Channel();
	~Channel();


	void write(T data);
	T read();

private:

	std::list<T> buffer_;

	std::mutex bufferMutex_;

	Semaphore semaphore_;
};





template <class T>
Channel<T>::Channel()
{
}

template <class T>
Channel<T>::~Channel()
{
}


template <class T>
void Channel<T>::write(T data) {

	std::unique_lock<std::mutex> lock(bufferMutex_);

	buffer_.push_back(data);

	semaphore_.Signal();
}


template <class T>
T Channel<T>::read() {

	semaphore_.Wait();

	std::unique_lock<std::mutex> lock(bufferMutex_);

	T item = buffer_.front();

	buffer_.pop_front();

	return item;
}


