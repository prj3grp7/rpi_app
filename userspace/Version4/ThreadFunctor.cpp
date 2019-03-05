#include "ThreadFunctor.h"

void * ThreadFunctor::threadMapper(void * thread)
{
	ThreadFunctor * tf = static_cast<ThreadFunctor*>(thread);
	tf->run();
	return nullptr;
}