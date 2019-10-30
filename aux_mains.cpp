
int main_grids(){
	int line_thick = 5;
	for (int i = 1;i <= 30; ++i){
		Mat grid = gen_grid(line_thick,i,1000);
		double black_pixel_fraction = ((double)(i*i))/((double)(line_thick * line_thick + 2 * line_thick *i + i*i));
		PRINT(black_pixel_fraction)
		imwrite(to_string(line_thick) + "lines_" + to_string(i) + "spaces_" + to_string(black_pixel_fraction) + "black_px_fraction" + ".png",grid);
	}
	
	
	/*
	int line_thick = 5;
	int between_line = 2;
	int line_num = 30;
	Mat img1((line_thick + between_line)*(line_num + 1) + 1,(line_thick + between_line)*(line_num + 1) + 1, CV_8UC3, Scalar::all(0));
	gen_streight_lines(img1, line_num, 0, (double)line_thick - 0.01, Scalar::all(255));
	//gen_streight_lines(img1, line_num, 0, 1, Scalar(0,0,255));
	line(img1, Point(0,119+0.001), Point(217,119+0.001), Scalar(0,0,255),1);
	imwrite("test.png",img1);
	*/
	
	/*
	
	int linedist = 2;
	
	Mat img(3*7 + 5 + linedist - 1, 3*7 + 5 + linedist, CV_8UC3, Scalar::all(0));
	gen_streight_lines(img, (3*7/(5+linedist)) ,0,4.99, Scalar::all(255)); // vertical
	gen_streight_lines(img, (3*7/(5+linedist)) ,-90.0 * PI / 180.0,4.99, Scalar::all(255));
	
	imwrite("line_dist_" + to_string(linedist + 5) + ".png" ,img);
	*/
	
	return 0;
}


int main_gen_grid(){
	for (int linedist = 1; linedist <= 5; ++linedist){
	Mat img(2*2*2*3*3*7*5 + 5 + linedist, 2*2*2*3*3*7*5 + 5 + linedist, CV_8UC3, Scalar::all(0));
	gen_streight_lines(img, (2*2*2*3*3*7*5/(5+linedist)) ,0,4.99, Scalar::all(255));
	gen_streight_lines(img, (2*2*2*3*3*7*5/(5+linedist)) ,-90.0 * PI / 180.0,4.99, Scalar::all(255));
	
	imwrite("line_dist_" + to_string(linedist + 5) + ".png" ,img);
	}
	
	return 0;
}



int main_generate_noisy_grid() {
	int line_thick = 5;
	int i = 30;
	
	
	double signal_to_noise = pow(2,-3);
	
	Mat grid = gen_grid(line_thick,i,1000);
	//double black_pixel_fraction = ((double)(i*i))/((double)(line_thick * line_thick + 2 * line_thick *i + i*i));
	//PRINT(black_pixel_fraction)
	grid = noisify_gauss(grid, signal_to_noise);
	imwrite(to_string(line_thick) + "lines_" + to_string(i) + "spaces_" + to_string(signal_to_noise) + "signal_to_noise" + ".png",grid);
	
	
	signal_to_noise = pow(2,-2);
	
	grid = gen_grid(line_thick,i,1000);
	//double black_pixel_fraction = ((double)(i*i))/((double)(line_thick * line_thick + 2 * line_thick *i + i*i));
	//PRINT(black_pixel_fraction)
	grid = noisify_gauss(grid, signal_to_noise);
	imwrite(to_string(line_thick) + "lines_" + to_string(i) + "spaces_" + to_string(signal_to_noise) + "signal_to_noise" + ".png",grid);
	
	
	signal_to_noise = pow(2,-1);
	
	grid = gen_grid(line_thick,i,1000);
	//double black_pixel_fraction = ((double)(i*i))/((double)(line_thick * line_thick + 2 * line_thick *i + i*i));
	//PRINT(black_pixel_fraction)
	grid = noisify_gauss(grid, signal_to_noise);
	imwrite(to_string(line_thick) + "lines_" + to_string(i) + "spaces_" + to_string(signal_to_noise) + "signal_to_noise" + ".png",grid);
	
	
	signal_to_noise = pow(2,0);
	
	grid = gen_grid(line_thick,i,1000);
	//double black_pixel_fraction = ((double)(i*i))/((double)(line_thick * line_thick + 2 * line_thick *i + i*i));
	//PRINT(black_pixel_fraction)
	grid = noisify_gauss(grid, signal_to_noise);
	imwrite(to_string(line_thick) + "lines_" + to_string(i) + "spaces_" + to_string(signal_to_noise) + "signal_to_noise" + ".png",grid);
	
	
	signal_to_noise = pow(2,1);
	
	grid = gen_grid(line_thick,i,1000);
	//double black_pixel_fraction = ((double)(i*i))/((double)(line_thick * line_thick + 2 * line_thick *i + i*i));
	//PRINT(black_pixel_fraction)
	grid = noisify_gauss(grid, signal_to_noise);
	imwrite(to_string(line_thick) + "lines_" + to_string(i) + "spaces_" + to_string(signal_to_noise) + "signal_to_noise" + ".png",grid);
	
	
	signal_to_noise = pow(2,2);
	
	grid = gen_grid(line_thick,i,1000);
	//double black_pixel_fraction = ((double)(i*i))/((double)(line_thick * line_thick + 2 * line_thick *i + i*i));
	//PRINT(black_pixel_fraction)
	grid = noisify_gauss(grid, signal_to_noise);
	imwrite(to_string(line_thick) + "lines_" + to_string(i) + "spaces_" + to_string(signal_to_noise) + "signal_to_noise" + ".png",grid);
	
	
	return 0;
}

