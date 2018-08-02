#ifndef _FILE_LOGGER_H_
#define _FILE_LOGGER_H_

#include <atomic>
#include <fstream>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include "defines.h"

class FileLogger
{
public:
	FileLogger(std::string logname);
	~FileLogger();

	static FileLogger* get_logger(std::string message = "");
	void OnLog(LogLevel level, std::string message);

protected:
	
	void open_log_file();

	void add_log_msg_to_queue(const std::string& message);

	void write_log_msg_to_file(const std::string& message);

	void log_fatal(const std::string& message);
	void log_error(const std::string& message);
	void log_warning(const std::string& message);
	void log_info(const std::string& message);
	void log_trace(const std::string& message);
	void log_debug(const std::string& message);

	void thread_func();

private:
	static std::atomic<FileLogger*>		m_instance;
	static std::mutex					m_logger_mutex;
	bool					m_is_stopped;
	std::thread				m_msg_loop_thread;
	std::mutex				m_msg_loop_mtx;
	std::condition_variable m_msg_loop_cv;
	std::list<std::string>	m_log_queue;
	std::fstream			m_output_log_file;
	std::string				m_log_file_name;
};

#define LOG_FTL(msg) {auto l = FileLogger::get_logger(); l->OnLog(LogLevel::fatal, msg);}
#define LOG_ERR(msg) {auto l = FileLogger::get_logger(); l->OnLog(LogLevel::error, msg);}
#define LOG_WRN(msg) {auto l = FileLogger::get_logger(); l->OnLog(LogLevel::warning, msg);}
#define LOG_INF(msg) {auto l = FileLogger::get_logger(); l->OnLog(LogLevel::info, msg);}
#define LOG_DBG(msg) {auto l = FileLogger::get_logger(); l->OnLog(LogLevel::debug, msg);}
#define LOG_TRACE(msg) {auto l = FileLogger::get_logger(); l->OnLog(LogLevel::trace, msg);}

#endif // _FILE_LOGGER_H_