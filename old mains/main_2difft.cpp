
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

double** I2;

uint32 w,h;




int main(){

	if(importsemtif("../Franzi_CPD_012.tif",I1,&w,&h)){
		cout << "image could not be imported" << endl;
	}
	
	I2 = new double*[w];
	
	fftw_complex *in, *out;
	fftw_plan p;
	
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * w * h);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * w * h);
	
	
	p = fftw_plan_dft_2d(w,h,in,out,FFTW_FORWARD, FFTW_ESTIMATE);
	
	for(unsigned long long i = 0; i < w; ++i){
		I2[i] = new double[h];
		for (unsigned long long j = 0; j < h; ++j){
			in[(i * h) + j][0] =  (double)I1[i][j]; // the in[i][j][0] referes to the real part of the fftw_complex data type
		}
	}
	
	fftw_execute(p);
	
	p = fftw_plan_dft_2d(w,h,out,in,FFTW_BACKWARD, FFTW_ESTIMATE);
	
	fftw_execute(p);
	
	for (unsigned long long i = 0; i < w; ++i){ //this loop prints x and y in the wrong order therefore they are flipped, but this is due to performance reasons
		for (unsigned long long j = 0; j < h; ++j){
			I2[i][j] = (double)I1[i][j] - in[(i*h)+j][0]/(w*h);			
//			I2[i][j] = log(abs(sqrt(pow(out[(i * h) + j][0],2) +  pow(out[(i * h) + j][1],2))) + 1);
			cout << I2[i][j] << " ";
		}
		cout << endl;
	}
	
	
	
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);
	
	
	
}

