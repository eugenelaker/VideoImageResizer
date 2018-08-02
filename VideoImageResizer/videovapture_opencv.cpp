#include "videovapture_opencv.h"
#include "filelogger.h"
#include <experimental/filesystem>
#include "opencv2/opencv.hpp"



namespace fs = std::experimental::filesystem;

VideoCapture_opencv::VideoCapture_opencv()
{
}


VideoCapture_opencv::~VideoCapture_opencv()
{
}

void VideoCapture_opencv::Init(std::string filename)
{
	m_filename = filename;
}

int64_t VideoCapture_opencv::Procced()
{
	LOG_DBG("VideoImagesHandler::Proceed");
	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
	fs::path path(m_filename);
	cv::VideoCapture cap = cv::VideoCapture(path.string());
	LOG_DBG("Openning video capture");
	if (!cap.isOpened()) {
		std::stringstream msg;
		msg << "Video Capture not opened for file: " << m_filename << "\n";
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
