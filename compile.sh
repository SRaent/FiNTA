#!/bin/sh
#g++ -o atafut -std=c++11 -I /usr/include/opencv4 -L /usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -O0 -g -pthread main.cpp #-Wall -Werror
#-O0 -g
#g++ -o atafut -std=c++11 -Wl,-Bstatic -I /home/moritz/opencv/opencv-4.1.0-built/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -O0 -g -pthread main.cpp #-Wall -Werror
g++ -o atafut -std=c++11 -I /home/moritz/opencv/opencv-4.1.0-built/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -O0 -g -pthread main.cpp #-Wall -Werror


#g++ -o atafut -std=c++11 -I /home/moritz/opencv/built-dynamic/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -O0 -g -pthread main.cpp #-Wall -Werror