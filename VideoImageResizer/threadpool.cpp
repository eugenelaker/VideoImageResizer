#include "threadpool.h"

std::atomic<ThreadPool*> ThreadPool::m_instance;
std::mutex ThreadPool::m_mutex;

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

ThreadPool::ThreadPool(const std::size_t threads_cnt)
	: m_stop(false)
{
	if (threads_cnt == 0)
		m_threads.reserve(2);
	else
		m_threads.reserve(threads_cnt);

	for (std::size_t i = 0; i < threads_cnt; ++i)
		m_threads.emplace_back([this](){ Worker(); });

}


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

void ThreadPool::Stop()
{
	{
		std::unique_lock<std::recursive_mutex> lock(m_queue_mutex);

		m_stop = true;
	}

	m_notifier.notify_all();
}

void ThreadPool::Worker()
{
	while (true) {

		std::function <void()> task;

		{
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
























