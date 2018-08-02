#include "filelogger.h"
#include <ctime>
#include <time.h>
#include <sstream>

std::atomic<FileLogger*> FileLogger::m_instance;
std::mutex FileLogger::m_logger_mutex;

//---------------------------------------------------------------------
//  get_logger: return only one instance of thread pool
//	input:       log name 
//---------------------------------------------------------------------
FileLogger* FileLogger::get_logger(std::string logname)
{
	FileLogger* log = m_instance.load(std::memory_order_acquire);
	if (!log) {
		std::lock_guard<std::mutex> lock(m_logger_mutex);
		log = m_instance.load(std::memory_order_relaxed);
		if (!log) {
			log = new FileLogger(logname);
			m_instance.store(log, std::memory_order_release);
		}
	}

	return log;
}


FileLogger::FileLogger(std::string logname)
{
	m_is_stopped = false;
	m_log_file_name = logname;
	m_msg_loop_thread = std::thread(&FileLogger::thread_func, this);
}


FileLogger::~FileLogger()
{
	if (m_output_log_file.is_open()) {
		m_output_log_file.clear();
		m_output_log_file.close();
	}

	m_is_stopped = true;
	m_msg_loop_cv.notify_one();

	if (m_msg_loop_thread.joinable()) {
		m_msg_loop_thread.join();
	}
}

void FileLogger::OnLog(LogLevel level, std::string message)
{
	switch (level)
	{
	case LogLevel::fatal:
		log_fatal(message);
		break;

	case LogLevel::error:
		log_error(message);
		break;

	case LogLevel::warning:
		log_warning(message);
		break;

	case LogLevel::info:
		log_info(message);
		break;

	case LogLevel::trace:
		log_trace(message);
		break;

	case LogLevel::debug:
		log_debug(message);
		break;

	default:
		break;
	}
}

void FileLogger::thread_func()
{
	while (!m_is_stopped) {
		std::unique_lock<std::mutex> lock(m_msg_loop_mtx);
// 		m_msg_loop_cv.wait(lock, [this]()
// 		{
// 			return (m_is_stopped || !m_log_queue.empty());
// 		});

		m_msg_loop_cv.wait_for(lock, std::chrono::duration<int, std::milli>(5), [this]()
		{
			return (m_is_stopped || !m_log_queue.empty()); 
		});


		while (!m_is_stopped && !m_log_queue.empty()) {
			std::string log_msg = m_log_queue.front();
			m_log_queue.pop_front();
			write_log_msg_to_file(log_msg);
		}
	}
}

void FileLogger::open_log_file()
{
	
	std::string logFilePath = "";
	logFilePath += m_log_file_name + "_";
	time_t now = time(0);
	tm* tstruct;
	tstruct = localtime(&now);
	logFilePath += std::to_string(tstruct->tm_year + 1900) + "-"
		+ std::to_string(tstruct->tm_mon + 1) + "-"
		+ std::to_string(tstruct->tm_mday) + "_"
		+ std::to_string(tstruct->tm_hour) + "-"
		+ std::to_string(tstruct->tm_min) + "-"
		+ std::to_string(tstruct->tm_sec);
	logFilePath += ".log";

	m_output_log_file.open(logFilePath, std::ios::out | std::ios::ate);

	if (!m_output_log_file.is_open()) {
		m_output_log_file.clear();
		m_output_log_file.open(logFilePath, std::ios::out); //Create file.
		m_output_log_file.close();
		m_output_log_file.open(logFilePath);
	}
}

void FileLogger::write_log_msg_to_file(const std::string& message)
{
	if (!m_output_log_file.is_open()) {
		open_log_file();
	}
	else {
		m_output_log_file << message;
		m_output_log_file.flush();
	}
}

void FileLogger::add_log_msg_to_queue(const std::string& message)
{
	std::stringstream ss;
	ss << message << '\n';
	std::unique_lock<std::mutex> lock(m_msg_loop_mtx);
	m_log_queue.push_back(ss.str());
	lock.unlock();
	m_msg_loop_cv.notify_one();
}

void FileLogger::log_fatal(const std::string& message)
{
	add_log_msg_to_queue("[FATAL  ] " + message);
}

void FileLogger::log_error(const std::string& message)
{
	add_log_msg_to_queue("[ERROR  ] " + message);
}

void FileLogger::log_warning(const std::string& message)
{
	add_log_msg_to_queue("[WARNING] " + message);
}

void FileLogger::log_info(const std::string& message)
{
	add_log_msg_to_queue("[INFO   ] " + message);
}

void FileLogger::log_trace(const std::string& message)
{
	add_log_msg_to_queue("[TRACE  ] " + message);
}

void FileLogger::log_debug(const std::string& message)
{
	add_log_msg_to_queue("[DEBUG  ] " + message);
}
