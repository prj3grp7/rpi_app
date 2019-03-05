#include "Thread.h"

Thread::Thread(ThreadFunctor * tf) : tf_(tf)
{

}

Thread::~Thread()
{
	detach();
}

void Thread::start()
{
	if (pthread_create(&threadId_, nullptr, ThreadFunctor::threadMapper, tf_) != 0)
	{
		//throw ThreadError(); 
	}
	attached_ = true;
}

void Thread::join()
{
	if (!attached_)
	{
		//throw ThreadError(); 
	}
	pthread_join(threadId_, nullptr);
}

void Thread::detach()
{
	if (!attached_)
	{
		//throw ThreadError();
	}
	attached_ = false;
	tf_ = nullptr;
	pthread_detach(threadId_);
}
