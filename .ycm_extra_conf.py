def Settings( **kwargs ):
  return {
    'flags': [ '-x', 'c++', '-std=c++11 ', '-ltiff', '-lfftw3', '-I', '/usr/include/opencv4', '-L', '/usr/lib', '-lopencv_core', '-lopencv_imgproc', '-lopencv_highgui ', '-lopencv_imgcodecs', '-O0', '-g', '-Wwrite-strings', '-pthread'],
  }
