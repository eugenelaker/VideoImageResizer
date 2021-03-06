#include "videocapture_opencv.h"
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

//---------------------------------------------------------------------
//  VideoCapture_opencv::Init
//	input:  video file path 
//---------------------------------------------------------------------
void VideoCapture_opencv::Init(std::string filepath)
{
	m_filepath = filepath;
}

//---------------------------------------------------------------------
//  VideoCapture_opencv::Proceed
//	used to perform capture frame, resize and write to disk 
//---------------------------------------------------------------------
int64_t VideoCapture_opencv::Proceed()
{
	std::stringstream msg;
	msg << "VideoImagesHandler::Proceed  thread " << std::this_thread::get_id();
	LOG_DBG(msg.str());
	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();

	fs::path path(m_filepath);
	cv::VideoCapture cap = cv::VideoCapture(path.string());

	msg.str("");
	msg << "Opening video capture for thread: " << std::this_thread::get_id();
	LOG_DBG(msg.str());
	if (!cap.isOpened()) {
		msg.str("");
		msg << "Video Capture not opened for file: " << m_filepath << " in thread: " << std::this_thread::get_id();
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
			//read next frame
			if (cap.read(frame)) {
				cnt++;
				//resize frame
				cv::resize(frame, resized_frame, cv::Size(RESIZE_X, RESIZE_Y));
				std::string new_file(dir + "//frame_" + std::to_string(cnt).append(IMAGE_FORMAT));
				//write to disk
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

	return ms.count(); //return execution time
}
