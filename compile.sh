#!/bin/sh
g++ main.cpp -std=c++11 -ltiff -lfftw3 -I /usr/include/opencv4 -L /usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -O3 -g -Wwrite-strings -pthread #-Wall -Werror
#-O0 -g
