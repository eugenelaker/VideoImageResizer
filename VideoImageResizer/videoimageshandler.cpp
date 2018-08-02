#include "videoimageshandler.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <experimental/filesystem>
#include "threadpool.h"
#include "opencv2/opencv.hpp"

#include "filelogger.h"


namespace fs = std::experimental::filesystem;
const std::vector<std::string> extensions = { ".avi", ".mp4", ".mkv", ".flv" };

VideoImagesHandler::VideoImagesHandler()
	: m_folder(""),
	m_workers_cnt(0)
{

}

VideoImagesHandler::~VideoImagesHandler()
{

}

void VideoImagesHandler::Init(std::string folder, int workers_cnt)
{
	m_folder = folder;
	ThreadPool::getInstance(workers_cnt);
}

void VideoImagesHandler::SetLogger(std::shared_ptr<FileLogger> logger)
{
	m_logger = logger;
}


std::vector<std::string> VideoImagesHandler::GetVideoFiles(const std::vector<std::string>& ext)
{
	std::vector<std::string> files;
	for (auto& dirEntry : fs::directory_iterator(m_folder))
	{
		if (fs::is_regular_file(dirEntry))
		{
			if (std::find(extensions.begin(), extensions.end(), dirEntry.path().extension().string()) != extensions.end())
				files.push_back(dirEntry.path().string());
		}
		
	}

	return files;
}

std::int64_t VideoImagesHandler::Proceed(const std::string& file_path)
{
	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
	fs::path path(file_path);
	cv::VideoCapture cap = cv::VideoCapture(path.string());
	LOG_DBG("Openning video capture;");
	if (!cap.isOpened()) {
		std::stringstream msg;
		msg << "Video Capture not opened for file: " << file_path << "\n";
		LOG_ERR(msg.str());
		return -1;
	}
		

	cv::Mat frame;
	cv::Mat resized_frame;
	std::uint64_t cnt = 0;

	std::string filename_without_ext = path.stem().string();
	std::string dir = path.remove_filename().append(filename_without_ext);
	fs::create_directory(dir);

	
	while (true) {
		try {
			if (cap.read(frame)) {
				cnt++;
				cv::resize(frame, resized_frame, cv::Size(160, 160));
				std::string new_file(dir + "//frame_" + std::to_string(cnt).append(".png"));
							
				cv::imwrite(new_file, resized_frame);
			}
			else 
				break;
		}
		catch (std::runtime_error& ex) {
			cap.release();
			return -1;
		}
	}

	cap.release();

	std::chrono::duration<double> dur = std::chrono::steady_clock::now() - start;
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);

	return ms.count();
}

state_codes VideoImagesHandler::Start()
{
	if (m_folder.empty())
		return STATE_FOLDER_IS_EMPTY;

	int64_t total_time = 0;;
	ThreadPool* tpool = ThreadPool::getInstance();
	std::vector<std::string> files = GetVideoFiles(extensions);

	if (!files.empty() && tpool) {
		int64_t result;
		std::vector<std::future<int64_t>> tasks_vec;
		for (const std::string& f : files) {		
			tasks_vec.push_back(tpool->AddTask([&](std::string f) -> int64_t {
				return Proceed(f);
			}, f));
		}

		for (auto& tsk : tasks_vec)	{
			result = tsk.get();
			if (result != -1) {
				total_time += result;
				std::stringstream msg;
				msg << "Worker time to execute: " << result << " ms." << std::endl;
				LOG_DBG(msg.str());
			}
		}

		if (total_time) {
			std::stringstream msg;
			msg << "All workers total time to execute: " << total_time << " ms." << std::endl;
			LOG_DBG(msg.str());
		}
	}
	else
		return STATE_NO_VIDEO_FILES;

	return STATE_SUCCEDED;
}


