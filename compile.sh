#!/bin/sh
#g++ -o atafut -std=c++11 -I /usr/include/opencv4 -L /usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -O0 -g -pthread main.cpp #-Wall -Werror
#-O0 -g




#g++ -o atafut -std=c++11 -L /home/moritz/opencv/opencv-4.1.0-built/lib64 -I /home/moritz/opencv/opencv-4.1.0-built/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -O0 -g -pthread main.cpp #-Wall -Werror


#g++ -o atafut main.cpp -std=c++11 --static -L /home/moritz/opencv/opencv-4.1.0-built/lib64 -I /home/moritz/opencv/opencv-4.1.0-built/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -O0 -g -pthread #-Wall -Werror

#g++ -o atafut main.cpp -std=c++11  -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -O0 -g -pthread -ldl --static -L /home/moritz/opencv/opencv-4.1.0-built/lib64 -I /home/moritz/opencv/opencv-4.1.0-built/include/opencv4

#gcc main.cpp -o atafut -std=c++11 -O3 $(pkg-config --cflags opencv; for x in $(ls /usr/local/lib64/libopencv*.a; ls /usr/local/lib64/libopencv*.a); do echo -n "$x "; done) -ldl -lm -lpthread -lrt -lstdc++ -ljasper -ljpeg -lpng -ltiff -lz -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpng12 -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lgstbase-0.10 -lgstreamer-0.10 -lxml2 -lrt -lavcodec -lavformat -lavutil -lswscale -ldc1394 -lv4l1

#gcc main.cpp -o atafut -std=c++11 -O3 -I /usr/include/opencv4 -L /usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -ldl -lm -lpthread -lrt -lstdc++ -ljasper -ljpeg -lpng -ltiff -lz -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpng12 -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lgstbase-0.10 -lgstreamer-0.10 -lxml2 -lrt -lavcodec -lavformat -lavutil -lswscale -ldc1394 -lv4l1







#gcc main.cpp -o atafut -std=c++11 -O3 -L /home/moritz/opencv/opencv-4.1.0-built/lib64 -I /home/moritz/opencv/opencv-4.1.0-built/include/opencv4 -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -ldl -lm -lpthread -lrt -lstdc++ -ljasper -ljpeg -lpng -ltiff -lz -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpng12 -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lgstbase-0.10 -lgstreamer-0.10 -lxml2 -lrt -lavcodec -lavformat -lavutil -lswscale -ldc1394 -lv4l1 -lwebp




#gcc main.cpp -o atafut -std=c++11 -O3 -L /usr/local/lib64 -I /usr/local/include/opencv4 $(pkg-config --cflags opencv; for x in $(ls /usr/local/lib64/libopencv*.a; ls /usr/local/lib64/libopencv*.a); do echo -n "$x "; done) /usr/local/lib64/opencv4/3rdparty/libippicv.a -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -ldl -lm -lpthread -lrt -lstdc++ -ljasper -ljpeg -lpng -ltiff -lz -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpng12 -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lgstbase-0.10 -lgstreamer-0.10 -lxml2 -lrt -lavcodec -lavformat -lavutil -lswscale -ldc1394 -lv4l1 -lwebp #least errors so far!

gcc main.cpp -o atafut -std=c++11 -O3 -L /usr/local/lib64 -I /usr/local/include/opencv4 $(pkg-config --cflags opencv; for x in $(ls /usr/local/lib64/libopencv*.a; ls /usr/local/lib64/opencv4/3rdparty/*.a; ls /usr/local/lib64/libopencv*.a; ls /usr/local/lib64/opencv4/3rdparty/*.a); do echo -n "$x "; done) /usr/local/lib64/opencv4/3rdparty/libippicv.a -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -ldl -lm -lpthread -lrt -lstdc++ -ljasper -ljpeg -lpng -ltiff -lz -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpng12 -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lgstbase-0.10 -lgstreamer-0.10 -lxml2 -lrt -lavcodec -lavformat -lavutil -lswscale -ldc1394 -lv4l1 -lwebp -lIlmImf -lgstreamer-1.0 -lgstapp-1.0 -lgstriff-1.0 -lgstpbutils-1.0 #least errors so far!





#g++ -std=c++0x main.cpp -I /usr/local/include/opencv4 -L /usr/local/lib -L /usr/local/lib64 -L /usr/local/lib64/opencv4/3rdparty -O3 -lopencv_aruco -lopencv_imgcodecs -lopencv_calib3d -lopencv_ccalib -lopencv_xfeatures2d -lopencv_features2d -lopencv_xobjdetect -lopencv_dnn -lopencv_flann -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_reg -lopencv_xphoto -lopencv_stitching -lopencv_superres -lopencv_bgsegm -lopencv_face -lopencv_saliency -lopencv_stitching -lopencv_superres -lopencv_tracking -lopencv_video -lopencv_ximgproc -lopencv_shape -lopencv_text -lopencv_optflow -lopencv_bioinspired -lopencv_imgproc -lopencv_core -littnotify -ljpeg -llibwebp -lrt -ldl -lz -lpthread -o atafut

#g++ -std=c++0x main.cpp -I /usr/local/include/opencv4 -L /usr/local/lib -L /usr/local/lib64 -L /usr/local/lib64/opencv4/3rdparty -O3 -lopencv_videoio -lopencv_imgcodecs -lopencv_calib3d -lopencv_features2d -lopencv_dnn -lopencv_flann -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_stitching -lopencv_video -lopencv_imgproc -lopencv_core -littnotify -ljpeg -lwebp -lrt -ldl -lz -lpthread -o atafut




#export LD_LIBRARY_PATH=/home/moritz/opencv/opencv-4.1.0-built/lib64:$LD_LIBRARY_PATH

#g++ -o atafut -std=c++11 -I /home/moritz/opencv/opencv-4.1.0-built/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -O0 -g -pthread main.cpp #-Wall -Werror


#g++ -o atafut -std=c++11 -I /home/moritz/opencv/built-dynamic/include/opencv4 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -O0 -g -pthread main.cpp #-Wall -Werror