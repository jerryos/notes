#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/private.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/imgproc/filterengine.hpp"
//#include "ippi.h"
//#include "ipps.h"
#include "ipp.h"
#include "errno.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

using namespace cv;

bool my_ipp_filter2D( Mat src, Mat &dst, int ddepth,
         Mat kernel, Point anchor0,
        double delta, int borderType )
{
//	imwrite("./ipp_input.jpg", src);
    if( ddepth < 0 )
        ddepth = src.depth();
   
	Point anchor = normalizeAnchor(anchor0, kernel.size());

    typedef IppStatus (CV_STDCALL * ippiFilterBorder)(const void * pSrc, int srcStep, void * pDst, int dstStep, IppiSize dstRoiSize,
        IppiBorderType border, const void * borderValue,
        const IppiFilterBorderSpec* pSpec, Ipp8u* pBuffer);

    int stype = src.type(), sdepth = CV_MAT_DEPTH(stype), cn = CV_MAT_CN(stype), ktype = kernel.type(), kdepth = CV_MAT_DEPTH(ktype);

    Point ippAnchor((kernel.cols-1)/2, (kernel.rows-1)/2);
	int borderTypeNI = borderType & ~BORDER_ISOLATED;
    IppiBorderType ippBorderType = ippiGetBorderType(borderTypeNI);	 /* convert to IPP BorderType  */

	/* find Filter */
	ippiFilterBorder ippFunc =
    	stype == CV_8UC1 ? (ippiFilterBorder)ippiFilterBorder_8u_C1R :
		stype == CV_8UC3 ? (ippiFilterBorder)ippiFilterBorder_8u_C3R :
		stype == CV_8UC4 ? (ippiFilterBorder)ippiFilterBorder_8u_C4R :
		stype == CV_16UC1 ? (ippiFilterBorder)ippiFilterBorder_16u_C1R :
		stype == CV_16UC3 ? (ippiFilterBorder)ippiFilterBorder_16u_C3R :
		stype == CV_16UC4 ? (ippiFilterBorder)ippiFilterBorder_16u_C4R :
		stype == CV_16SC1 ? (ippiFilterBorder)ippiFilterBorder_16s_C1R :
		stype == CV_16SC3 ? (ippiFilterBorder)ippiFilterBorder_16s_C3R :
		stype == CV_16SC4 ? (ippiFilterBorder)ippiFilterBorder_16s_C4R :
		stype == CV_32FC1 ? (ippiFilterBorder)ippiFilterBorder_32f_C1R :
		stype == CV_32FC3 ? (ippiFilterBorder)ippiFilterBorder_32f_C3R :
		stype == CV_32FC4 ? (ippiFilterBorder)ippiFilterBorder_32f_C4R : 0;

    IppiSize kernelSize = { kernel.cols, kernel.rows }, dstRoiSize = { src.cols, src.rows };
    IppDataType dataType = ippiGetDataType(ddepth), kernelType = ippiGetDataType(kdepth);
    Ipp32s specSize = 0, bufsize = 0;
    IppStatus status = (IppStatus)-1;

    ippiFilterBorderGetSize(kernelSize, dstRoiSize, dataType, kernelType, cn, &specSize, &bufsize);
    
    IppAutoBuffer<IppiFilterBorderSpec> spec(specSize);
    IppAutoBuffer<Ipp8u> buffer(bufsize);
    Ipp32f borderValue[4] = { 0, 0, 0, 0 };

    if(kdepth == CV_32F)
    {
        Ipp32f *pKerBuffer = (Ipp32f*)kernel.data;
        IppAutoBuffer<Ipp32f> kerTmp;
        int kerStep = sizeof(Ipp32f)*kernelSize.width;

        if((int)kernel.step != kerStep)
        {
            kerTmp.Alloc(kerStep*kernelSize.height);
            if(ippiCopy_32f_C1R((Ipp32f*)kernel.data, (int)kernel.step, kerTmp, kerStep, kernelSize) < 0)
                return false;
            pKerBuffer = kerTmp;
        }

        ippiFilterBorderInit_32f(pKerBuffer, kernelSize, dataType, cn, ippRndFinancial, spec);
        
        status = ippFunc(src.data, (int)src.step, dst.data, (int)dst.step, dstRoiSize, ippBorderType, borderValue, spec, buffer);		/* here */
//		imwrite("./ipp_output.jpg", dst);
        
    }

	else if(kdepth == CV_16S)
	{
    	Ipp16s *pKerBuffer = (Ipp16s*)kernel.data;
    	IppAutoBuffer<Ipp16s> kerTmp;
    	int kerStep = sizeof(Ipp16s)*kernelSize.width;
    	if((int)kernel.step != kerStep)
    	{
        	kerTmp.Alloc(kerStep*kernelSize.height);
        	if(ippiCopy_16s_C1R((Ipp16s*)kernel.data, (int)kernel.step, kerTmp, kerStep, kernelSize) < 0)
            	return false;
        	pKerBuffer = kerTmp;
    	}
		if((status = ippiFilterBorderInit_16s(pKerBuffer, kernelSize, 0, dataType, cn, ippRndFinancial, spec)) >= 0)
		{
    		status = ippFunc(src.data, (int)src.step, dst.data, (int)dst.step, dstRoiSize, ippBorderType, borderValue, spec, buffer);
		}
    }

    if (status >= 0)
    {
//        CV_IMPL_ADD(CV_IMPL_IPP);
        return true;
    }
    return false;
}

   
int main ( int, char** argv )
{
  Mat src;
  Mat kernel;
  Point anchor;
  double delta;
  int ddepth;
  int kernel_size;

  // Load an image
  src = imread( argv[1] );

  // assign space to dst image
  Mat dst=Mat::zeros(src.rows, src.cols, src.type());

  if( src.empty() )
    { return -1; }

/*
  // Create window
  const char* window_name = "IPP filter2D Demo";
  namedWindow( window_name, WINDOW_AUTOSIZE );
*/
  // Initialize arguments for the filter
  anchor = Point( -1, -1 );
  delta = 0;
  ddepth = -1;

  int ind = 2;

  // kernel size for a normalized box filter
  kernel_size = 3 + 2*( ind%5 );	// 3, 5, 7, 9, 11
  kernel = Mat::ones( kernel_size, kernel_size, CV_32F )/ (float)(kernel_size*kernel_size);

  struct timeval start, finish;
  gettimeofday(&start, NULL);
  // Apply filter
  my_ipp_filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_REPLICATE );
  
  gettimeofday(&finish, NULL);
  double duration_1=(finish.tv_sec-start.tv_sec)*1000+(finish.tv_usec-start.tv_usec)/1000;
  printf("IPP cost time: %f ms\n", duration_1);



  imwrite("./ipp_output.jpg", dst);
  
/*
  // show dst picture
  imshow( window_name, dst );
  waitKey(0);		
*/

  return 0;
}


// g++ ipp_filter2D.cpp -o ipp_filter2D -I /opt/intel/compilers_and_libraries_2016.1.150/linux/ipp/include -lippcore -lipps -lippvm -lippi  `pkg-config opencv --cflags --libs`  -g
/*
1.注释48行
/usr/local/include/opencv2/core/private.hpp +48

2.添加路径52行
/usr/local/include/opencv2/core/private.hpp +52 
改为opencv2/cvconfig.h
*/
