def Settings( **kwargs ):
  return {
          'flags': ['-std=c++14', '-isystem/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/10.2.0/../../../../include/c++/10.2.0/', '-isystem/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/10.2.0/../../../../include/c++/10.2.0/x86_64-pc-linux-gnu', '-isystem/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/10.2.0/../../../../include/c++/10.2.0/backward', '-isystem/usr/local/include', '-isystem/usr/lib/clang/11.0.0/include', '-isystem/usr/include', '-x', 'c++', '-L', '/usr/lib/', 'lstdc++', '-stdlib=libstdc++', 
              '-I', '/usr/include/opencv4', '-L', '/usr/lib', '-lopencv_core', '-lopencv_imgproc', '-lopencv_highgui', '-lopencv_imgcodecs', '-lopencv_videoio',
              ],
  }


#def Settings( **kwargs ):
#  return {
#          'flags': ['-std=c++14', '-isystem/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/10.1.0/../../../../include/c++/10.1.0/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/10.1.0/../../../../include/c++/10.1.0/x86_64-pc-linux-gnu', '-isystem/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/10.1.0/../../../../include/c++/10.1.0/backward', '-isystem/usr/local/include', '-isystem/usr/lib/clang/10.0.0/include', '-isystem/usr/include', '-x', 'c++', '-L', '/usr/lib/', '-stdlib=libstdc++', '-lstdc++',
#               '-I', '/usr/include/opencv4', '-L', '/usr/lib', '-lopencv_core', '-lopencv_imgproc', '-lopencv_highgui', '-lopencv_imgcodecs', '-lopencv_videoio',
#              ],
#  }
