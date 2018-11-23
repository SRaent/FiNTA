#!/bin/sh
g++ main.cpp -std=c++11 -ltiff -lfftw3 -I /usr/include/opencv -L /usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -O0 -g -Wwrite-strings
#-O0 -g
