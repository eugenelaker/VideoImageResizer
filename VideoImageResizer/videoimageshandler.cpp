#include "videoimageshandler.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include "threadpool.h"
#include "filelogger.h"
#include <experimental/filesystem>
#include "videocaptureinterface.h"


namespace fs = std::experimental::filesystem;


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
	LOG_DBG("VideoImagesHandler::Init");
	m_folder = folder;
	
	_tread_pool_ptr = std::make_unique<ThreadPool>(workers_cnt);
	
}

void VideoImagesHandler::SetLogger(std::shared_ptr<FileLogger> logger)
{
	m_logger = logger;
}


std::vector<std::string> VideoImagesHandler::GetVideoFiles(const std::vector<std::string>& ext)
{
	LOG_DBG("VideoImagesHandler::GetVideoFiles");
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

state_codes VideoImagesHandler::Start()
{
	LOG_DBG("VideoImagesHandler::Start");
	if (m_folder.empty())
		return state_codes::STATE_FOLDER_IS_EMPTY;

	int64_t total_time = 0;
	std::vector<std::string> files = GetVideoFiles(extensions);

	if (!files.empty() && _tread_pool_ptr) {
		int64_t result;
		std::vector<std::future<int64_t>> tasks_vec;
		std::vector<IVideoCapture::ptr> cature_vector;
		for (std::size_t i = 0; i < files.size(); ++i)
			cature_vector.push_back(create_video_capture(video_capture_types::opencv));
		int i = 0;
		for (const std::string& f : files) {
			
			if (cature_vector[i]) {
				cature_vector[i]->Init(f);
				tasks_vec.push_back(_tread_pool_ptr->AddTask([i,cature_vector](std::string f) -> int64_t {
					return cature_vector[i]->Proceed();
				}, f));
			}
			i++;
		}

		for (auto& tsk : tasks_vec)	{
			result = tsk.get();
			if (result != -1) {
				total_time += result;
				std::stringstream msg;
				msg << "Worker time to execute: " << result << " ms.";
				LOG_DBG(msg.str());
			}
		}

		for (IVideoCapture::ptr cap : cature_vector) {
			cap = nullptr;
		}
		cature_vector.clear();

		if (total_time) {
			std::stringstream msg;
			msg << "All workers total time to execute: " << total_time << " ms.";
			LOG_DBG(msg.str());
		}
	}
	else
		return state_codes::STATE_NO_VIDEO_FILES;

	return state_codes::STATE_SUCCEDED;
}


