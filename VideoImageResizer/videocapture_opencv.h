#ifndef VIDEO_CAPTURE_INTERFACE
#define VIDEO_CAPTURE_INTERFACE
#include "videocaptureinterface.h"


class VideoCapture_opencv : 	public IVideoCapture
{
public:
	VideoCapture_opencv();
	virtual ~VideoCapture_opencv();

	//IVideoCapture
	virtual int64_t Proceed();
	virtual void Init(std::string filepath);
};

#endif // VIDEO_CAPTURE_INTERFACE


