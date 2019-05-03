#!/bin/sh
g++ main.cpp -o atafut -std=c++11 -ltiff -lfftw3 -I /usr/include/opencv4 -L /usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -O0 -g -Wwrite-strings -pthread #-Wall -Werror
#-O0 -g
