#pragma once
#include "videocaptureinterface.h"


class VideoCapture_opencv : 	public IVideoCapture
{
public:
	VideoCapture_opencv();
	virtual ~VideoCapture_opencv();

	//IVideoCapture
	virtual int64_t Proceed();
	virtual void Init(std::string filename);
};

