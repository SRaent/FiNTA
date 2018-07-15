
#define PRINT(x) std::cout << #x << " => " << x << std::endl;

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <complex.h>
#include <fftw3.h>

#include "tiffio.h"


#include "datafun.h"
#include "datafun.cpp"

using namespace std;


uint8** I1;
uint32 w,h;




int main(){

	if(importsemtif("../Franzi_CPD_012.tif",I1,&w,&h)){
		cout << "image could not be imported" << endl;
	}
	
	
	fftw_complex *in, *out;
	fftw_plan p;
	
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * h);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * h);
	
	p = fftw_plan_dft_1d(h,in,out,FFTW_FORWARD, FFTW_ESTIMATE);
	
	for (unsigned long long j = 0; j < h; ++j){
		in[j][0] =  I1[0][j]; // the in[j][0] referes to the real part if the fftw_complex data type
	}
	
	fftw_execute(p);
	
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);
	
	
	
	for (unsigned long long k = 0; k < w; ++k){
		for (unsigned long long j = 0; j < h; ++j){
			
		}
//		cout << endl;
	}
}

