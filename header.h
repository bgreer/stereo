#include <time.h>
#include <cv.h>
#include <highgui.h>
#include "opencv2/contrib/contrib.hpp"
// use parse_params code I wrote forever ago
#include "parse_params.h"

using namespace cv;

// function prototypes

// ocv.cpp
void depth_estimate_opencv (Mat *img1, Mat *img2, Mat *res);
