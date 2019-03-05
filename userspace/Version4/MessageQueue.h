#pragma once
#ifndef _MSGQUEUE_H
#define _MSGQUEUE_H
#include <stdlib.h>
#include <queue>
#include <algorithm>
#include <pthread.h>
#include "Message.h"

struct Item : public Message
{
	Message* _msg;
	unsigned long _id;
};


class MsgQueue {
private:
	std::queue<Message*> _mq; //The message queue container
	unsigned long _size; //The size of the queue
	pthread_mutex_t _msgQueueMtx; //Mutex for locking _mq
	pthread_cond_t _msgQueueCnd_send; //Conditional for sending a message
	pthread_cond_t _msgQueueCnd_receive; //Conditional for recieving a message

public:
	//The Default constructor
	//size: The size of the queue, default 1
	MsgQueue(unsigned long size = 100)
	{
		//Init _size
		_size = size;
		//Init thread handling
		pthread_mutex_init(&_msgQueueMtx, nullptr);
		pthread_cond_init(&_msgQueueCnd_send, nullptr);
		pthread_cond_init(&_msgQueueCnd_receive, nullptr);
	}

	//The default destructor
	~MsgQueue() {  }

	//Method for sending a message
	//id: The id of the message
	//msg: The message to be sent.
	void send(unsigned long id, Message* msg = nullptr)
	{
		pthread_mutex_lock(&_msgQueueMtx); //Lock
		while (_mq.size() == _size) {
			pthread_cond_wait(&_msgQueueCnd_receive, &_msgQueueMtx); //Wait for space to send
		}
		Item* mq_item = new Item; //Create new item
		mq_item->_id = id;
		mq_item->_msg = msg;
		_mq.push(mq_item); //Push item onto queue
		pthread_mutex_unlock(&_msgQueueMtx); //Unlock
		pthread_cond_signal(&_msgQueueCnd_send); //Signal a message is ready
	}

	//Method for receiving a message
	//id: The id of the message
	//return: A pointer to the message. Nullptr if id not the same
	Message* receive(unsigned long& id)
	{
		pthread_mutex_lock(&_msgQueueMtx); //Lock
		while (_mq.empty()) {
			pthread_cond_wait(&_msgQueueCnd_send, &_msgQueueMtx); //Wait for messages to handle
		}
		Item* msg = static_cast<Item*>(_mq.front()); //Get item
		id = msg->_id; //set ID
		_mq.pop(); //Else remove and return the message
		pthread_mutex_unlock(&_msgQueueMtx);
		pthread_cond_signal(&_msgQueueCnd_receive); //Signal a message has been received
		return msg->_msg;
	}
};

#endif
