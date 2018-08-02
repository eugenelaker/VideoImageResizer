#ifndef STATE_CODES_H
#define STATE_CODES_H

enum class state_codes {
	STATE_SUCCEDED = 1,
	STATE_FAILER,
	STATE_FOLDER_IS_EMPTY,
	STATE_NO_VIDEO_FILES,
	STATE_ERROR_CONVERTING,
};

enum class video_capture_types
{
	opencv,
	ffmpeg,
};

enum class LogLevel
{
	none = 0,
	fatal = 1,
	error = 2,
	warning = 3,
	info = 4,
	debug = 5,
	trace = 6
};

#endif // STATE_CODES_H

