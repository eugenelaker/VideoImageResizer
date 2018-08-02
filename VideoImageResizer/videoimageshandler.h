#ifndef VIDEOIMAGESHANDLER_H
#define VIDEOIMAGESHANDLER_H

#include <string>
#include <memory>
#include <vector>
#include "defines.h"

class FileLogger;
class ThreadPool;

class VideoImagesHandler
{
public:
	VideoImagesHandler();
	~VideoImagesHandler();

	void Init(std::string folder, int workers_cnt);
	state_codes Start();
	void SetLogger(std::shared_ptr<FileLogger> logger);

private:
	std::vector<std::string> GetVideoFiles(const std::vector<std::string>& ext);

private:
	std::string m_folder;
	int m_workers_cnt;
	std::shared_ptr<FileLogger> m_logger;
	std::unique_ptr<ThreadPool> _tread_pool_ptr;

};

#endif // VIDEOIMAGESHANDLER_H
