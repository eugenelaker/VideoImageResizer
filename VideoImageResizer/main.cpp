#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "videoimageshandler.h"
#include "filelogger.h"

#if defined(__cplusplus) && (__cplusplus == 201103L) //c++11
	template<typename T, typename... Args>
	std::unique_ptr<T> make_unique(Args&&... args) {
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
#endif

int main(int argc, char* argv[])
{
	std::string folder_name;
	int worker_cnt = 0;

	int next_option;
	const char* const short_options = "hf:w:";

	do {
		next_option = getopt(argc, argv, short_options);

		switch (next_option)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [-f \"folder\"] [-w \"workers\"] \n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'f':
			folder_name = optarg;
			break;
		case 'w':
			worker_cnt = atoi(optarg);
			break;
		case -1:
			break;
		default: /* Something else: unexpected. */
			fprintf(stderr, "Usage: %s [-f folder] [-w workers] \n", argv[0]);
			exit(EXIT_FAILURE);
		}
	} while (next_option != -1);

	if (argc < 5) {
		fprintf(stderr, "Usage: %s [-f \"folder\"] [-w \"workers\"] \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	std::shared_ptr<FileLogger> logger;
	logger.reset( FileLogger::get_logger("VideoImageResizer"));
	std::unique_ptr<VideoImagesHandler> handler = std::make_unique<VideoImagesHandler>();
	if (handler) {
		handler->Init(folder_name, worker_cnt);
		switch (handler->Start()) {
		case state_codes::STATE_SUCCEDED:
			logger = nullptr;
			exit(EXIT_SUCCESS);
		case state_codes::STATE_FAILER:
			logger = nullptr;
			exit(EXIT_FAILURE);
		case state_codes::STATE_FOLDER_IS_EMPTY:
			std::cout << "folder name is empty" << std::endl;
			logger = nullptr;
			exit(EXIT_FAILURE);
		case  state_codes::STATE_NO_VIDEO_FILES:
			std::cout << "folder has not contains video files" << std::endl;
			logger = nullptr;
			exit(EXIT_FAILURE);
		case state_codes::STATE_ERROR_CONVERTING:
			std::cout << "Exception converting image to PNG format" << std::endl;
		default:
			std::cout << "something goes wrong" << std::endl;
			logger = nullptr;
			exit(EXIT_FAILURE);
		}
	}

	logger = nullptr;
	
	return 0;
}