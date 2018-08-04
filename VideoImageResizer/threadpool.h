#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <mutex>
#include <thread>
#include <queue>
#include <functional>
#include <condition_variable>
#include <future>

class ThreadPool
{
public:
	ThreadPool(const std::size_t threads_cnt);
	~ThreadPool();

	//---------------------------------------------------------------------
	//  ThreadPool: AddTask to queue
	//	 input:  functor , function params
	//---------------------------------------------------------------------
	template < typename Function_t, typename... Args >
	auto AddTask(Function_t&& func, Args&&... args)
		->std::future< typename std::result_of< Function_t(Args...) >::type >
	{
		using return_t = typename std::result_of<Function_t(Args...)>::type;

		auto task = std::make_shared<std::packaged_task<return_t()>>(std::bind(std::forward<Function_t>(func), std::forward<Args>(args)...));

		std::future<return_t> result = task->get_future();

		{
			std::unique_lock<std::recursive_mutex> lock(m_queue_mutex);
			m_task_queue.emplace([task]() { (*task)(); });
		}

		m_notifier.notify_one();
		return result;
	}
	void Stop();
	void Worker();

	std::size_t Thread_Count() const { return m_threads.size(); }

private:
	ThreadPool() = delete;

	ThreadPool(const ThreadPool & source) = delete;
	ThreadPool & operator=(const ThreadPool & source) = delete;
private:
	std::vector<std::thread >           m_threads;
	std::queue<std::function<void()>>   m_task_queue;
	std::recursive_mutex                m_queue_mutex;
	std::condition_variable_any         m_notifier;
	bool                                m_stop;
};

#endif // THREADPOOL_H
