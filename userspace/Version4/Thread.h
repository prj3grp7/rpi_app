#pragma once
#include "ThreadFunctor.h"
#include "pthread.h"
#include <string>

class Thread
{
public:
	Thread(ThreadFunctor* tf);
	virtual ~Thread();
	void start();
	void join();
	void detach();

private:
	ThreadFunctor * tf_;
	pthread_t threadId_;
	bool attached_;
};