/**
 * @file filter2D_demo.cpp
 * @brief Sample code that shows how to implement your own linear filters by using filter2D function
 * @author OpenCV team
 */

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

using namespace cv;

/**
 * @function main
 */
int main ( int, char** argv )
{
  /// Declare variables
  Mat src, dst;

  Mat kernel;
  Point anchor;
  double delta;
  int ddepth;
  int kernel_size;
  const char* window_name = "openCV filter2D Demo";

  /// Load an image
  src = imread( argv[1] );

  if( src.empty() )
    { return -1; }

  /// Create window
//  namedWindow( window_name, WINDOW_AUTOSIZE );

  /// Initialize arguments for the filter
  anchor = Point( -1, -1 );
  delta = 0;
  ddepth = -1;

  int ind = 2;

  /// Update kernel size for a normalized box filter
  kernel_size = 3 + 2*( ind%5 );	// 3, 5, 7, 9, 11
  kernel = Mat::ones( kernel_size, kernel_size, CV_32F )/ (float)(kernel_size*kernel_size);


  struct timeval start, finish;
  gettimeofday(&start, NULL);

  /// Apply filter
  filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_REPLICATE );

  gettimeofday(&finish, NULL);
  double duration_1=(finish.tv_sec-start.tv_sec)*1000+(finish.tv_usec-start.tv_usec)/1000;
  printf("opencv cost time: %f ms\n", duration_1);

  imwrite("./opencv_output.jpg", dst);

//  imshow( window_name, dst );
//  waitKey(0);		

  return 0;
}




//         filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_CONSTANT );
//         filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
//		   filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_REPLICATE );
