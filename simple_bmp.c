#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/* This code can only deal with bmp files
 it was my first attempt at this problem and does some simple stuff
 future codes use opencv for image i/o and such
 */

/* aww yiss, 15-123 lab6 */
typedef struct { 
  uint8_t** rows; 
  uint8_t header[54];
  int height, width, size, padding;
} BITMAP;

/* The following function takes a FILE*
   and reads all the image attributes into bmp 
 */
void readImage(FILE* fp, BITMAP *bmp){
	int i;
	fread(bmp->header, 54, 1, fp);
	bmp->size = *((int*)(bmp->header+2));
	bmp->width = *((int*)(bmp->header+18));
	bmp->height = *((int*)(bmp->header+22));
	bmp->padding = (bmp->size - 54 - 3*(bmp->width)*(bmp->height))/(bmp->height);
	bmp->rows = malloc((bmp->height)*sizeof(uint8_t*));
	for (i=0; i<bmp->height; i++) {
		(bmp->rows)[i] = malloc((3*bmp->width + bmp->padding) * sizeof(uint8_t));
		fread((bmp->rows)[i], 3*bmp->width + bmp->padding, 1, fp);
	}
}
/* 
	The following function writes the image into a file.
*/
void writeImage(uint8_t* outfile, BITMAP *bmp){
	FILE* fpw;
	int i;
	if ((fpw = fopen(outfile, "w"))==NULL) return;
	fwrite(bmp->header, 54, 1, fpw);
	for (i=0; i<bmp->height; i++) {
		fwrite((bmp->rows)[i], 3*bmp->width + bmp->padding, 1, fpw);
	}
}

uint8_t scale(float val)
{
	if (val > 255.0) return 0xFF;
	if (val < 0.0) return 0x00;
	return (uint8_t)val;
}

float correlate_block(float **arr1, float **arr2, int x1, int y1, int x2, int y2, int blocksize)
{
	float ret = 0.0;
	float norm1 = 0.0, norm2 = 0.0, count = 0.0;
	float st1 = 0.0, st2 = 0.0;
	int ix, iy;

	// find average
	for (ix=-blocksize; ix<=blocksize; ix++)
	{
		for (iy=-blocksize; iy<=blocksize; iy++)
		{
			norm1 += arr1[y1+iy][x1+ix];
			norm2 += arr2[y2+iy][x2+ix];
			count += 1.0;
		}
	}
	norm1 /= count;
	norm2 /= count;

	// find stddev
	for (ix=-blocksize; ix<=blocksize; ix++)
	{
		for (iy=-blocksize; iy<=blocksize; iy++)
		{
			st1 += pow(arr1[y1+iy][x1+ix] - norm1,2.0);
			st2 += pow(arr2[y2+iy][x2+ix] - norm2,2.0);
		}
	}
	st1 = sqrt(st1/count);
	st2 = sqrt(st2/count);

	// compute cross-correlation
	for (ix=-blocksize; ix<=blocksize; ix++)
	{
		for (iy=-blocksize; iy<=blocksize; iy++)
		{
			ret += (arr1[y1+iy][x1+ix]-norm1) * (arr2[y2+iy][x2+ix]-norm2);
		}
	}

	ret /= (st1*st2);

	return ret;
}

float diff_block(float **arr1, float **arr2, int x1, int y1, int x2, int y2, int blocksize)
{
	float ret = 0.0;
	float norm1 = 0.0, norm2 = 0.0, count = 0.0;
	float st1 = 0.0, st2 = 0.0;
	int ix, iy;

	// find average
	for (ix=-blocksize; ix<=blocksize; ix++)
	{
		for (iy=-blocksize; iy<=blocksize; iy++)
		{
			norm1 += arr1[y1+iy][x1+ix];
			norm2 += arr2[y1+iy][x1+ix];
			count += 1.0;
		}
	}
	norm1 /= count;
	norm2 /= count;

	// find stddev
	for (ix=-blocksize; ix<=blocksize; ix++)
	{
		for (iy=-blocksize; iy<=blocksize; iy++)
		{
			st1 += pow(arr1[y1+iy][x1+ix] - norm1,2.0);
			st2 += pow(arr2[y1+iy][x1+ix] - norm2,2.0);
		}
	}
	st1 = sqrt(st1/count);
	st2 = sqrt(st2/count);

	// find diff
	for (ix=-blocksize; ix<=blocksize; ix++)
	{
		for (iy=-blocksize; iy<=blocksize; iy++)
		{
			ret += pow((arr1[y1+iy][x1+ix]-arr2[y1+iy][x1+ix]), 2.0) / sqrt(st1*st1 + st2*st2);
			count += 1.0;
		}
	}
	ret /= count;

	return ret;
}

int main (int argc, char *argv[])
{
	FILE *fp1;
	BITMAP *bmp1, *bmp2, *res;
	float **img1, **img2, **img3;
	int ii, ij, w, h, ix, iy;
	float maxval, corr;
	int maxpos, scandist, scanstep, block;

	// create space for input bitmaps
	bmp1 = malloc(sizeof(BITMAP));
	bmp2 = malloc(sizeof(BITMAP));
	fp1 = fopen("test02.bmp", "r");
	readImage(fp1, bmp1);
	close(fp1);
	fp1 = fopen("test03.bmp", "r");
	readImage(fp1, bmp2);
	close(fp1);

	if (bmp1->width != bmp2->width || bmp1->height != bmp2->height)
	{
		printf("ERROR: image sizes must be equal");
		return -1;
	}

	w = bmp1->width;
	h = bmp1->height;

//	printf("width=%d, height=%d\n", w, h);

	// allocate space for brightness
	img1 = (float**) malloc(h * sizeof(float*));
	img2 = (float**) malloc(h * sizeof(float*));
	img3 = (float**) malloc(h * sizeof(float*));
	for (ii=0; ii<h; ii++)
	{
		img1[ii] = (float*) malloc(w * sizeof(float));
		img2[ii] = (float*) malloc(w * sizeof(float));
		img3[ii] = (float*) malloc(w * sizeof(float));
	}

	// compute brightness
	for (ii=0; ii<h; ii++)
	{
		for (ij=0; ij<w; ij++)
		{
			img1[ii][ij] = (bmp1->rows[ii][3*ij] + bmp1->rows[ii][3*ij+1] + bmp1->rows[ii][3*ij+2])/3.0;
			img2[ii][ij] = (bmp2->rows[ii][3*ij] + bmp2->rows[ii][3*ij+1] + bmp2->rows[ii][3*ij+2])/3.0;
			img1[ii][ij] /= 255.0;
			img1[ii][ij] -= 0.5;
			img2[ii][ij] /= 255.0;
			img2[ii][ij] -= 0.5;
		}
	}

	// do a fake shift of part of img2
	/*
	for (ii=200; ii<400; ii++)
	{
		for (ij=200; ij<400; ij++)
		{
			img2[ii][ij] = img2[ii][ij+100];
		}
	}
	
	for (ii=600; ii<800; ii++)
	{
		for (ij=600; ij<800; ij++)
		{
			img2[ii][ij] = img2[ii][ij+50];
		}
	}
*/
	/* flow:
		consider a single pixel in img1
		correlate the neighboring block of pixels with the block around the same pixel in img2
		do this correlation of blocks against pixels x-adjacent to the same pixel in img2
		this gives you a linear array of correlation coefs. each value correcponds to a pixel offset
		the pixel with the highest correlation is your likely offset
	*/

	scandist = 150;
	scanstep = 1;
	block = 30;

	for (iy=block+1; iy<h-block-1; iy++)
	{
		printf("%d ", iy);
		fflush(stdout);
		for (ix=block+scandist+1; ix<w-block-scandist-1; ix++)
		{
			maxval = -1e6;
			maxpos = 0;
			for (ii=-scandist; ii<scandist; ii+=scanstep) // scanning
			{
				corr = correlate_block(img1, img2, ix+ii,iy,  ix,iy,  block);
				if (corr > maxval)
				{
					maxval = corr;
					maxpos = ii;
				}
			}
			img3[iy][ix] = maxpos;
		}
	}


	// create final image
	for (ii=0; ii<h; ii++)
	{
		for (ij=0; ij<w; ij++)
		{
			bmp1->rows[ii][3*ij+0] = scale(img3[ii][ij]*1.0);
			bmp1->rows[ii][3*ij+1] = scale(img3[ii][ij]*1.0);
			bmp1->rows[ii][3*ij+2] = scale(img3[ii][ij]*1.0);
		}
	}
	// output final image
	writeImage("output.bmp", bmp1);
	return 0;
}
