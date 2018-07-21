#!/bin/sh
g++ main.cpp -std=c++11 -ltiff -lfftw3 -I /usr/include/opencv -L /usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -O4 -g
#-O0 -g
