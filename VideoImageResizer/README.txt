					***********************************************
					*											  *
					*            Video Image Resizer              *
					*											  *
					***********************************************

NAME: 
		videoimageresizer = Video Image Resizer command-line reference					

SYNOPSIS:
		videoimageresizer [<opthion>]
		
DESCRIPTION:
		The "videoimageresizer" executable is the videoimageresizer command-line interface.
		It may be used to parce video file to images and resize it. All video file executes in parallel
		thread.
		
OPTION:
		-f <folder>
			Define video folder where video files stored

		-w <threads-cnt>
			Define number of worker threads for parallel video parcing
			
BUILD:
		1.	Explanation how to download, build and install is here: 
			https://docs.opencv.org/trunk/d7/d9f/tutorial_linux_install.html
		
		2.	Run from project derectory:
							cmake .
							make
		
		3.	Run builded program in next format:
					videoimageresizer -f [video folder you choose] -w [number of threads]
					
		4.	Find your parsed images in video folder in sub derectories named as source video files
		
		5.	Log file can be found in directory where your executable is.