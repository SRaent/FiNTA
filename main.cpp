
#define PRINT(x) cout << #x << " => " << x << endl;

#define SQRT2 (double)1.4142135623730950488016

#define THREADS 16

#define PX 396 //412
#define PY 238 //495
#define RV 8 // 8 vision radius (~2 times fibre thickness)
#define RS 3 //step radius
#define RT RV// vision for threshold
#define RN RF+RS //neighbor radius
#define RF RS/SQRT2  //forbidden radius
#define RM 0 //minimum vision radius
#define STEPS 360
#define DEV 0.55 // 0.55 deviation of gaussian smooth of circlefun
#define TH 0 //0.2 threshold for findpks
#define ML 7 //minimum loop length


//nolonger used
#define LT 2 // line thiccness for connectable test
#define LD 0.2 //deviation of smoothing if line function for connectable test
#define LS RS*LT // steps for averaging the line function
#define CT 3 // Connectabel threshhold. if the smoothed function goes below this, no new node will be spawned.
#define FA 0.125*PI //forbidden angle for already connected nodes to spawn new ones


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <complex.h>
#include <fftw3.h>
#include <thread>

#include "tiffio.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

#include "datafun.h"
#include "datafun.cpp"
#include "mathfun.cpp"
#include "node.h"
#include "node.cpp"
#include "analyse.cpp"
#include "visualise.cpp"
#include "generate.cpp"
/*
int main2(){
	save_hessian_colors(200,30.0);
	return 0;
}
*/

int main(){
	
	Mat img(1000, 1000, CV_8UC3, Scalar::all(0));
	gen_streight_lines(img,49,0, 3, Scalar::all(255));
	gen_streight_lines(img,49,-90.0 * PI / 180.0, 3, Scalar::all(255));
	
	//namedWindow( "Display window", WINDOW_AUTOSIZE );	// Create a window for display.
	//imshow( "Display window", img);	// Show our image inside it.
	/*
	waitKey(0);
	waitKey(0);
	waitKey(0);
	waitKey(0);
	waitKey(0);
	waitKey(0);
	return 0;
	*/
	/*
	Mat I2 = imread("/home/moritz/Documents/Moritz_pics_lap/Franzi_CPD_012.tif");
	if(!I2.data){
		cout << "bild existiert NICHT" << endl;
		return -1;
	}
	
	//Mat I2(img);
	Rect myROI(0,0,1024,884);
	Mat I3 = I2(myROI);
	cv::cvtColor(I3, I3, cv::COLOR_BGR2GRAY);
	Size s = I2.size();
	Mat I5;
	I3.convertTo(I5, CV_64F);
	*/
	Mat I3;
	cv::cvtColor(img, I3, cv::COLOR_BGR2GRAY);
	Mat I5;
	I3.convertTo(I5, CV_64F);
	
	normalize(I5,I5,255,0,32);
	
	
	PRINT(I5.channels());
	
	
	Mat hessian = convolve_hessian(I5,50,2.3); //1.8 (a bit less than half fibre thickness)
	Mat tubeness = tubeness_hessian(hessian);
	
	
	
	vector<node*> list;
	vector<node**> closures;
	gen_startpoints(list, closures, hessian, tubeness);
	
	normalize(tubeness,tubeness,255,0,32);
	
	
	//node* origin = new node(PX,PY,&list,&closures,&hessian);
	//node* origin2 = new node(500,500,&list,&closures,&hessian);
	//node* origin3 = new node(354,254,&list,&closures,&hessian);
	//node* origin4 = new node(356,860,&list,&closures,&hessian);
	//node* origin5 = new node(98,371,&list,&closures,&hessian);
	
	unsigned long long i = 0;
	
	for (bool buisy = 1; buisy && i < 500 ;){
		buisy = 0;
		unsigned long long end = list.size();
		for (unsigned long long it = 0; it < end; ++it){
			if (!(list[it]->procreated)){
				list[it]->procreate_hessian();
				buisy = 1;
			}
		}
		
		//for (unsigned long long i = 0; i < list.size(); ++i){
		//	PRINT(list[i])
		//}
		cout << i++ << " " << list.size() << endl;
		
	}
	
	vector<node*> junctions = find_junctions(list);
	vector<double> angles = all_con_angles_long_curvature(junctions,5);
	double_vector_to_file("angles.dat",angles);
	vector<double> angles_naive = con_angles_long(junctions,5);
	double_vector_to_file("angles_naive.dat",angles_naive);

	vector<vector<node*>> lines = find_lines(list,0.3*PI);
	
	
	I3.convertTo(I3, CV_8U);
	cv::cvtColor(I3, I3, cv::COLOR_GRAY2BGR);
	
	I3 = draw_list(I3,list);
	PRINT(list.size())
	
	only_loops(list, closures);
	
	PRINT(list.size())
	I3 = draw_list(I3,list,Scalar(255,0,0));
	
//	I3 = draw_list(I3,junctions,Scalar(0,0,0));
	
	
	vector<vector<node*>> loops = find_loops(closures);
	
	vector<double> areas = find_loop_areas_wo_max(loops);
	double_vector_to_file("areas.dat",areas);
	//PRINT(list.size())
	//PRINT(loops.size())
	//PRINT(closures.size())

	
	
//	I3 = draw_closures(I3,closures,Scalar(128,0,128));
	
	//draw_loops("./lines/", lines, I3, true);
	cout << "total area: " << total_loop_area(loops) << " max area: " << max_loop_area(loops) << " difference: " << max_loop_area(loops) - total_loop_area(loops)  << endl;
	
	imwrite("doubt.tif",I3);
	imwrite("doubt2.tif",tubeness);
	return 0;
}
