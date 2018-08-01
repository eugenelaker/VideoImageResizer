#ifndef VIDEOIMAGESHANDLER_H
#define VIDEOIMAGESHANDLER_H

#include <string>
#include <memory>
#include <vector>
#include "defines.h"


class VideoImagesHandler
{
public:
	VideoImagesHandler();
	~VideoImagesHandler();

	void Init(std::string folder, int workers_cnt);
	state_codes run();

private:
	std::vector<std::string> GetVideoFiles(const std::vector<std::string>& ext);
	std::int64_t Proceed(const std::string& file_path);

private:
	std::string m_folder;
	int m_workers_cnt;

};

#endif // VIDEOIMAGESHANDLER_H
