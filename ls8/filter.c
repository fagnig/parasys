/*
  Author: 02346 Staff
  Date:   26-03-2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string.h>
#include <omp.h>

#include "readBMP.h"
#include "writeBMP.h"

/*Can be changed, must be odd so that the window is symmetric
  Makes the median filter use a window of WINDOW_SIZE x WINDOW_SIZE*/
#define WINDOW_SIZE 5

void processImage(char*);
void averagingFilterNaive(PIXEL_ARRAY*, PIXEL_ARRAY*, int);
void averagingFilterOpenMP(PIXEL_ARRAY*, PIXEL_ARRAY*, int);
void benchmark(char*);

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: ./filter <image-filename>\n.");
    exit(1);
  }
  processImage(argv[1]);
}

void processImage(char* filename) {
  /* Declarations of image structures,
     you only need to focus on PIXEL_ARRAY */
  Image charImg;
  Image finalImg;
  Image finalOmp;
  PIXEL_ARRAY img;
  PIXEL_ARRAY ompImage;
  PIXEL_ARRAY original;


  if (!ImageLoad(filename, &charImg)) {
    printf("ERROR loading image.");
    return;
  }
  /*  we create 2 arrays to start, since we modify data in place
      and need a reference to the original pixel data */
  createPixelArrayFromImage(charImg, &img);
  createPixelArrayFromImage(charImg, &original);
  createPixelArrayFromImage(charImg, &ompImage);

  if ((ompImage.sizeX != img.sizeX) ||(ompImage.sizeY != img.sizeY))
    printf("ERROR loading image\n");


  printf("Image: %s loaded!\n", filename);

  /* Time and call openMP version */
  /* Warmup */
  averagingFilterOpenMP(&ompImage, &original, WINDOW_SIZE);

  /* Timing */
  double time1 = omp_get_wtime();
  averagingFilterOpenMP(&ompImage, &original, WINDOW_SIZE);
  double timeomp = omp_get_wtime() - time1;

  /* Time and call serial version */

  /* Warmup */
  averagingFilterNaive(&img, &original, WINDOW_SIZE);

  /* Timing */
  time1 = omp_get_wtime();
  averagingFilterNaive(&img, &original, WINDOW_SIZE);
  double timeserial = omp_get_wtime() - time1;

  printf("Elapsed time, serial filtering (s) = %f\n", timeserial);


  /*Checks that the two pictures are identical*/
  if (0 == memcmp(ompImage.data, img.data, img.sizeX*img.sizeY*sizeof(int32_t))){
    printf("Elapsed time, using omp filtering (s) = %f\n", timeomp);
  } else {
    printf("ERROR creating omp image, image not identical to serial filtered version\n");
  }


  /* Write your modified image as output to a BMP file */
  char writename[40];
  char writeomp[40];
  char no_ext_filename[40];

  sscanf(filename, "%39[^.]", no_ext_filename);
  sprintf(writename, "%s-processed.bmp", no_ext_filename);
  createImageFromPixelArray(img, &finalImg);
  writeImage(writename, finalImg);
  printf("Wrote new picture to: %s\n", writename);

  sprintf(writeomp, "%s-processed-omp.bmp", no_ext_filename);
  createImageFromPixelArray(ompImage, &finalOmp);
  writeImage(writeomp, finalOmp);
  printf("Wrote new omp processed picture to: %s\n", writeomp);
}


/*
  This functions calculates the average filter
  with a parameterizable window specifying NxN for the window
*/
void averagingFilterNaive(PIXEL_ARRAY* img, PIXEL_ARRAY* orig_img, int N) {

  int i, j, n, m;
  int red_avg, blue_avg, green_avg;
  int radius, out_of_bounds, idx, curr_idx;
  int32_t pixel;

  if (N % 2 == 0) {
    printf("ERROR: Please use an odd sized window\n");
    exit(1);
  }

  radius = N / 2;

  for (i = 0; i < img->sizeY; i++) {
    for (j = 0; j < img->sizeX; j++) {
      /* For pixels whose window would extend out of bounds, we need to count
	 the amount of pixels that we miss, since the window size will be smaller */
      out_of_bounds = 0;

      /* We are going to average the rgb values over the window */
      red_avg = 0;
      blue_avg = 0;
      green_avg = 0;

      /* This for loop sums up the rgb values for each pixel in the window */
      for (n = i - radius; n <= i + radius; n++) {
	for (m = j - radius; m <= j + radius; m++) {
	  /*  If we have an edge pixel, some of the window pixels will
	      be out of bounds. Thus we skip these and note that the
	      amount of pixels in the window are less than the window size */
	  if (n < 0 || m < 0 || n >= img->sizeY || m >= img->sizeX) {
	    out_of_bounds++;
	    continue;
	  }
	  idx = m + n * img->sizeX;
	  /* Shift, mask and add */
	  red_avg += ((orig_img->data[idx] >> 16) & 0xFF);
	  green_avg += ((orig_img->data[idx] >> 8) & 0xFF);
	  blue_avg += (orig_img->data[idx] & 0xFF);
	}
      }
      /* Divide the total sum by the amount of pixels in the window */
      red_avg /= (N * N - out_of_bounds);
      green_avg /= (N * N - out_of_bounds);
      blue_avg /= (N * N - out_of_bounds);

      /* Set the average to the current pixel */
      curr_idx = j + i * img->sizeX;
      pixel = (red_avg << 16) + (green_avg << 8) + blue_avg;
      img->data[curr_idx] = pixel;
    }
  }
}

void averagingFilterOpenMP(PIXEL_ARRAY* img, PIXEL_ARRAY* orig_img, int N) {
  /* Fill Me In! */
  int i, j, n, m;
  int red_avg, blue_avg, green_avg;
  int radius, out_of_bounds, idx, curr_idx;
  int32_t pixel;

  if (N % 2 == 0) {
    printf("ERROR: Please use an odd sized window\n");
    exit(1);
  }

  radius = N / 2;

#pragma omp parallel for
  for (i = 0; i < img->sizeY; i++) {
    for (j = 0; j < img->sizeX; j++) {
      /* For pixels whose window would extend out of bounds, we need to count
      the amount of pixels that we miss, since the window size will be smaller */
      out_of_bounds = 0;

      /* We are going to average the rgb values over the window */
      red_avg = 0;
      blue_avg = 0;
      green_avg = 0;

      /* This for loop sums up the rgb values for each pixel in the window */
      for (n = i - radius; n <= i + radius; n++) {
        for (m = j - radius; m <= j + radius; m++) {
        /*  If we have an edge pixel, some of the window pixels will
        be out of bounds. Thus we skip these and note that the
        amount of pixels in the window are less than the window size */
          if (n < 0 || m < 0 || n >= img->sizeY || m >= img->sizeX) {
            out_of_bounds++;
            continue;
          }
        idx = m + n * img->sizeX;
        /* Shift, mask and add */
        red_avg += ((orig_img->data[idx] >> 16) & 0xFF);
        green_avg += ((orig_img->data[idx] >> 8) & 0xFF);
        blue_avg += (orig_img->data[idx] & 0xFF);
        }
      }
      /* Divide the total sum by the amount of pixels in the window */
      red_avg /= (N * N - out_of_bounds);
      green_avg /= (N * N - out_of_bounds);
      blue_avg /= (N * N - out_of_bounds);

      /* Set the average to the current pixel */
      curr_idx = j + i * img->sizeX;
      pixel = (red_avg << 16) + (green_avg << 8) + blue_avg;
      img->data[curr_idx] = pixel;
    }
  }
}
