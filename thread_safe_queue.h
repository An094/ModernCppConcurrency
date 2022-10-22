#pragma once
#include <iostream>
#include <mutex>
#include <queue>
#include <memory>
#include <condition_variable>
#include <thread>

template<typename T>
class thread_safe_queue
{
	std::mutex m;
	std::condition_variable cv;
	std::queue<std::shared_ptr<T>> queue;
public:
	thread_safe_queue() 
	{
	}

	thread_safe_queue(thread_safe_queue const& other_queue)
	{
		std::lock_guard<std::mutex> lg(other.m);
		queue = other_queue.queue;
	}

	void push(T& value)
	{
		std::lock_guard<std::mutex> lg(m);
		if (queue.empty())
		{
			return std::make_shared<T>();
		}
		else
		{
			std::shared_ptr<T> ref(queue.front());
			queue.pop();
			return ref;
		}
	}

	bool empty()
	{
		std::lock_guard<std::mutex> lg(m);
		return queue.empty();
	}

	std::shared_ptr<T> wait_pop()
	{
		std::unique_lock<std::mutex> lg(m);//using unique_lock for conditional variable, since wait will unlock mutex. We can't use lock_guard.
		cv.wait(lg, [this]
			{
				return !queue.empty();
			});

		//wait(std::unique<std::mutex>& lock, Predicate stop_waiting) 
		//Equivalent to
		//while(!stop.waiting())
		//{
		//	wait(lock);
		//}

		std::shared_ptr<T> ref = queue.front();
		queue.pop();
		return ref;
	}

	size_t size()
	{
		std::lock_guard<std::mutex> lg(m);
		return queue.size();
	}

	bool wait_pop(T& ref)
	{
		std::unique_lock<std::mutex> lg(m);
		cv.wait(lg, [this]
			{
				return !queue.empty();
			});
		ref = *(queue.front().get());
		queue.pop();
		return true;
	}

	bool pop(T& ref)
	{
		std::lock_guard<std::mutex> lg(m);
		if (queue.empty())
		{
			return false;
		}
		else
		{
			ref = queue.front();
			queue.pop();
			return true;
		}
	}
};