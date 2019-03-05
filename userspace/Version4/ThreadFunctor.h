#pragma once
class Thread;
class ThreadFunctor
{
public:

protected:
	virtual void run() = 0;
	ThreadFunctor() {}

private:
	friend class Thread;
	static void* threadMapper(void *thread);
};