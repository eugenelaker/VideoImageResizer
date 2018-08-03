#ifndef VIDEO_CAPTURE_INTERFACE_H
#define VIDEO_CAPTURE_INTERFACE_H

#include <iostream>
#include <memory>
#include "defines.h"

class IVideoCapture
{
public:
	typedef std::shared_ptr <IVideoCapture> ptr;

	IVideoCapture();
	virtual ~IVideoCapture() {}
	
	virtual void Init(std::string filename) = 0;
	virtual int64_t Proceed() = 0;

protected:
	std::string m_filename;
	
};

IVideoCapture::ptr create_video_capture(video_capture_types type);

#endif // VIDEO_CAPTURE_INTERFACE_H

