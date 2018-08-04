#include "videocaptureinterface.h"
#include "videocapture_opencv.h"


//---------------------------------------------------------------------
//  factory for video capture. Choose which capturing engine to use
//					create_video_capture
//---------------------------------------------------------------------
IVideoCapture::ptr create_video_capture(video_capture_types type)
{
	switch (type)
	{
	case video_capture_types::opencv:
		return std::make_shared<VideoCapture_opencv>();
	case video_capture_types::ffmpeg:
		break;
	default:
		return nullptr;
		break;
	}

	return nullptr;
}