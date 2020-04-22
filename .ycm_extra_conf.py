def Settings( **kwargs ):
  return {
          'flags': ['-std=c++14', '-isystem/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.3.0/../../../../include/c++/9.3.0', '-isystem/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.3.0/../../../../include/c++/9.3.0/x86_64-pc-linux-gnu', '-isystem/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.3.0/../../../../include/c++/9.3.0/backward', '-isystem/usr/local/include', '-isystem/usr/lib/clang/9.0.1/include', '-isystem/usr/include', '-x', 'c++', '-L', '/usr/lib/', '-stdlib=libstdc++', '-lstdc++',
               '-I', '/usr/include/opencv4', '-L', '/usr/lib', '-lopencv_core', '-lopencv_imgproc', '-lopencv_highgui', '-lopencv_imgcodecs', '-lopencv_videoio',
              ],
  }
