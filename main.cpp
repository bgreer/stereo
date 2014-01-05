#include "header.h"


int main( int argc, char** argv )
{
	clock_t starttime, endtime;
	tag t[3];
	char fname_right[256], fname_left[256];
	int method, returnval;

	Mat img1_c, img2_c;
	Mat img1_g, img2_g;
	Mat result;

	// default values:
	method = 0;
	strcpy(fname_right, "images/RIGHTEYE_sm.JPG");
	strcpy(fname_left, "images/LEFTEYE_sm.JPG");

	/* parse command-line arguments */
	t[0].name = "-r";
	t[0].type = TAGTYPE_STRING;
	t[0].data = fname_right;
	t[1].name = "-l";
	t[1].type = TAGTYPE_STRING;
	t[1].data = fname_left;
	t[2].name = "-m"; // method
	t[2].type = TAGTYPE_INT;
	t[2].data = &method;
	returnval = parse_params(argc, argv, 3, t);
	
	// read in color images
	printf("Opening %s\n", fname_right);
	img1_c = imread(fname_right, 1 );
	printf("Right image dimensions: %d by %d\n", img1_c.rows, img1_c.cols);

	printf("Opening %s\n", fname_left);
	img2_c = imread(fname_left, 1 );
	printf("Left image dimensions: %d by %d\n", img2_c.rows, img2_c.cols);

	// make sure images are the same size
	if (img1_c.rows != img2_c.rows || img1_c.cols != img2_c.cols)
	{
		printf("ERROR: images must be the same size!\n");
		return -1;
	}

/*
	Mat test = imread("test.bmp", 1);
	printf("%d %d\n", test.rows, test.cols);

	uint8_t *ptr = (uint8_t*) test.data;
	int cn = test.channels();
	printf("%d %d\n", cn, img1_c.channels());

	printf("%d %d %d\n", ptr[(0)*3+0], ptr[(0)*3+1], ptr[(0)*3+2]);
	printf("%d %d %d\n", ptr[(1)*3+0], ptr[(1)*3+1], ptr[(1)*3+2]);
	printf("%d %d %d\n", ptr[(2)*3+0], ptr[(2)*3+1], ptr[(2)*3+2]);
	printf("%d %d %d\n", ptr[(3)*3+0], ptr[(3)*3+1], ptr[(3)*3+2]);
*/

	// convert to greyscale
	cvtColor(img1_c, img1_g, CV_BGR2GRAY);
	cvtColor(img2_c, img2_g, CV_BGR2GRAY);


	// make space for the resulting image, set to 0 just in case
	result = Mat(img1_c.rows, img1_c.cols, CV_8UC1);
	result.setTo(0);

	// compute depth
	printf("Computing depth using method %d.. ", method);
	fflush(stdout);
	starttime = clock();
	switch (method)
	{
		case 0: // opencv
			depth_estimate_opencv(&img1_g, &img2_g, &result);
			break;
		case 1: // ?
			break;
	}
	endtime = clock();
	printf("Done.\n");
	printf("Compute time: %fs\n", ((float)(endtime - starttime))/CLOCKS_PER_SEC);
	printf("  FPS Estimate: %f\n", ((float)CLOCKS_PER_SEC)/(endtime-starttime));

	// display result on screen
	namedWindow( "RESULT", CV_WINDOW_AUTOSIZE );
	imshow("RESULT", result);

	waitKey(0);

// imwrite( "Gray_Image.jpg", gray_image );
	return 0;
}

