#include "threadpool.h"
#include <algorithm>

std::atomic<ThreadPool*> ThreadPool::m_instance;
std::mutex ThreadPool::m_mutex;

//---------------------------------------------------------------------
//  getInstance: return only one instance of thread pool
//	input:       count of desired workers 
//---------------------------------------------------------------------
ThreadPool* ThreadPool::getInstance(int thread_cnt)
{
	ThreadPool* sin = m_instance.load(std::memory_order_acquire);
	if (!sin) {
		std::lock_guard<std::mutex> myLock(m_mutex);
		sin = m_instance.load(std::memory_order_relaxed);
		if (!sin) {
			sin = new ThreadPool(thread_cnt);
			m_instance.store(sin, std::memory_order_release);
		}
	}

	return sin;
}

ThreadPool::ThreadPool()
	: ThreadPool(std::thread::hardware_concurrency())
{
}

//---------------------------------------------------------------------
//  ThreadPool: constructor
//	 
//---------------------------------------------------------------------
ThreadPool::ThreadPool(const std::size_t threads_cnt)
	: m_stop(false)
{
	std::size_t hw_threads = std::thread::hardware_concurrency();
	unsigned long const num_threads = std::min(hw_threads != 0 ? hw_threads : 2, threads_cnt);
	m_threads.reserve(num_threads);

	for (std::size_t i = 0; i < threads_cnt; ++i)
		m_threads.emplace_back([this](){ Worker(); });

}

//---------------------------------------------------------------------
//  ThreadPool: destructor
//	 
//---------------------------------------------------------------------
ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::recursive_mutex> lock(m_queue_mutex);
		m_stop = true;
	}

	m_notifier.notify_all();
	for (auto& thread : m_threads) {
		if (thread.joinable())
			thread.join();
	}
}
//---------------------------------------------------------------------
//  ThreadPool: Stop function to stop thread pool execution
//	 
//---------------------------------------------------------------------
void ThreadPool::Stop()
{
	{
		std::unique_lock<std::recursive_mutex> lock(m_queue_mutex);

		m_stop = true;
	}

	m_notifier.notify_all();
}
//---------------------------------------------------------------------
//  ThreadPool: Worker of each thread
//	 
//---------------------------------------------------------------------
void ThreadPool::Worker()
{
	while (true) {

		std::function <void()> task;

		{
			// waiting to new task in queue to start execute
			std::unique_lock<std::recursive_mutex> lock(m_queue_mutex);
			m_notifier.wait(lock, [this] {return !m_task_queue.empty() || m_stop; });

			if (m_stop && m_task_queue.empty())
				return;

			task = std::move(m_task_queue.front());
			m_task_queue.pop();
		}

		task();
	}
}
























