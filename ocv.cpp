#include "header.h"


void depth_estimate_opencv (Mat *img1, Mat *img2, Mat *res)
{
	StereoVar sv;
	
	/* The number of pyramid layers, including the initial image. levels=1 means that no extra layers are created and only the original images are used. This parameter is ignored if flag USE_AUTO_PARAMS is set. */
	int levels = 3;

	/* Specifies the image scale (<1) to build the pyramids for each image. pyrScale=0.5 means the classical pyramid, where each next layer is twice smaller than the previous.*/
	double pyrScale = 0.5;

	/* The number of iterations the algorithm does at each pyramid level. (If the flag USE_SMART_ID is set, the number of iterations will be redistributed in such a way, that more iterations will be done on more coarser levels.) */
	int nIt = 5;

	/* Minimum possible disparity value. Could be negative in case the left and right input images change places. */
	int minDisp = 0;

	/* Maximum possible disparity value. */
	int maxDisp = 64;

	/* Size of the pixel neighbourhood used to find polynomial expansion in each pixel. The larger values mean that the image will be approximated with smoother surfaces, yielding more robust algorithm and more blurred motion field. Typically, poly_n = 3, 5 or 7 */
	int poly_n = 5;

	/* Standard deviation of the Gaussian that is used to smooth derivatives that are used as a basis for the polynomial expansion. For poly_n=5 you can set poly_sigma=1.1 , for poly_n=7 a good value would be poly_sigma=1.5 */
	double poly_sigma = 1.0;

	/* The smoothness parameter, ot the weight coefficient for the smoothness term. */
	float fi = 5.0;

	/* The threshold parameter for edge-preserving smoothness. (This parameter is ignored if PENALIZATION_CHARBONNIER or PENALIZATION_PERONA_MALIK is used.) */
	float lambda = 0.3; // higher makes it look smoother

	/* Possible values: PENALIZATION_TICHONOV - linear smoothness; PENALIZATION_CHARBONNIER - non-linear edge preserving smoothness; PENALIZATION_PERONA_MALIK - non-linear edge-enhancing smoothness. (This parameter is ignored if flag USE_AUTO_PARAMS is set). */
	int penalization = 0;

	/* Type of the multigrid cycle. Possible values: CYCLE_O and CYCLE_V for null- and v-cycles respectively. (This parameter is ignored if flag USE_AUTO_PARAMS is set). */
	int cycle = 1;

	/* USE_INITIAL_DISPARITY: Use the input flow as the initial flow approximation.
	USE_EQUALIZE_HIST: Use the histogram equalization in the pre-processing phase.
	USE_SMART_ID: Use the smart iteration distribution (SID).
	USE_AUTO_PARAMS: Allow the method to initialize the main parameters.
	USE_MEDIAN_FILTERING: Use the median filer of the solution in the post processing phase. */
	// {USE_INITIAL_DISPARITY = 1, USE_EQUALIZE_HIST = 2, USE_SMART_ID = 4, USE_AUTO_PARAMS = 8, USE_MEDIAN_FILTERING = 16};
	int flags = (4 | 8);

	sv = StereoVar(levels, pyrScale, nIt, minDisp, maxDisp, poly_n, poly_sigma, fi, lambda, penalization, cycle, flags);
	sv(*img1, *img2, *res);
}

