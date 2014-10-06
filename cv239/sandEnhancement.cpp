/* 
* Copyright(c) 2014,SWUST.
* All rights reserved. 
* 
* 文件名称：sandEnhancement.cpp
* 摘    要：沙尘水下图像增强
* 2014-9-23
*/ 
#include "stdafx.h"
#include <cv.h>
#include <math.h>
void FastFilter(IplImage *img, double sigma)
{
	int filter_size;

	// Reject unreasonable demands
	if ( sigma > 200 ) sigma = 200;

	// get needed filter size (enforce oddness)
	filter_size = (int)floor(sigma*6) / 2;
	filter_size = filter_size * 2 + 1;

	// If 3 sigma is less than a pixel, why bother (ie sigma < 2/3)
	if(filter_size < 3) return;

	// Filter, or downsample and recurse
	if (filter_size < 10) {

#ifdef USE_EXACT_SIGMA
		FilterGaussian(img, sigma)
#else
		cvSmooth( img, img, CV_GAUSSIAN, filter_size, filter_size );
#endif

	}
	else {
		if (img->width < 2 || img->height < 2) return;

		IplImage* sub_img = cvCreateImage(cvSize(img->width / 2, img->height / 2), img->depth, img->nChannels);

		cvPyrDown( img, sub_img );

		FastFilter( sub_img, sigma / 2.0 );

		cvResize( sub_img, img, CV_INTER_LINEAR );

		cvReleaseImage( &sub_img );
	}

}
void sandEnhancement(IplImage* src,IplImage *result)
{
	int sigma=src->height >>1;
	int scale=2;

	IplImage *A, *B, *C, *fA, *fB, *fC, *fsA, *fsB, *fsC, *fsD, *fsE, *fsF;
	IplImage* src_fl  = cvCreateImage(cvGetSize(src),IPL_DEPTH_32F,src->nChannels);
	IplImage* src_fl1 = cvCreateImage(cvGetSize(src),IPL_DEPTH_32F,src->nChannels);
	IplImage* src_fl2 = cvCreateImage(cvGetSize(src),IPL_DEPTH_32F,src->nChannels);
	fsA = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_32F, 1);
	fsB = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_32F, 1);
	fsC = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_32F, 1);
	fsD = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_32F, 1);
	fsE = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_32F, 1);
	fsF = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_32F, 1);
	float a=0.0,b=0.0,c=0.0;

	cvConvertScale(src,src_fl,1.7,1.0);//所有图像元素增加1.0保证cvlog正常    参数scale在1~100000变化没有影响，0.**1变化有影响。
	cvLog(src_fl,src_fl1);
	//cvConvertScale(src_fl1,src_fl1,1.0,0);//增加的句子
	FastFilter( src_fl, sigma );// cvSmooth(src_fl,src_fl2,CV_GAUSSIAN,0,0,sigma); //SSR算法核心 高斯模糊
	cvConvertScale(src_fl,src_fl,1.0,1.0);
	cvLog(src_fl,src_fl2);
	//  cvLog(src_fl2,src_fl2);
	//Retinex公式: Log(R(x,y))=Log(I(x,y))-Log(Gauss(I(x,y)))
	cvSub(src_fl1,src_fl2,src_fl);

	if (src->nChannels > 1)
	{
		A = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
		B = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
		C = cvCreateImage(cvSize(src->width, src->height),src->depth, 1);

		// Divide image into channels, convert and store sum
		cvSplit( src, A, B, C, NULL );

		cvConvert( A, fsA );
		cvConvert( B, fsB );
		cvConvert( C, fsC );

		cvReleaseImage( &A );
		cvReleaseImage( &B );
		cvReleaseImage( &C );

		// Sum components
		cvAdd( fsA, fsB, fsD );
		cvAdd( fsD, fsC, fsD );
		cvScale(fsD,fsD,1.0,1);
		cvScale(fsA,fsA,1.0,1);
		cvScale(fsB,fsB,1.0,1);
		cvScale(fsC,fsC,1.0,1);
		// Normalize weights
		cvDiv( fsA, fsD, fsA, 125);
		cvDiv( fsB, fsD, fsB, 125);
		cvDiv( fsC, fsD, fsC, 125);


		//cvConvertScale( fsA, fsA,1.0,0);
		//cvConvertScale( fsB, fsB,1.0,0);
		//cvConvertScale( fsC, fsC,1.0,0);
		cvConvertScale( fsA, fsA, 1, 1); //如果是( fsA, fsA,1.0,0); 则会有类似溢出的现象。2014-8-25
		cvConvertScale( fsB, fsB, 1, 1);
		cvConvertScale( fsC, fsC, 1, 1);

		// Log weights
		cvLog( fsA, fsA ); 
		cvLog( fsB, fsB );
		cvLog( fsC, fsC );

		// Divide retinex image, weight accordingly and recombine
		cvSplit( src_fl, fsD, fsE, fsF, NULL );

		cvMul( fsD, fsA, fsD, 46);
		cvMul( fsE, fsB, fsE, 46);
		cvMul( fsF, fsC, fsF, 46);
		cvConvertScale( fsD, fsD, 192,-30);
		cvConvertScale( fsE, fsE, 192,-30);
		cvConvertScale( fsF, fsF, 192,-30);
		cvMerge( fsD, fsE, fsF, NULL,src_fl );
	}

	//计算图像的均值、方差，SSR算法的核心之一
	//使用GIMP中转换方法：使用图像的均值方差等信息进行变换
	//没有添加溢出判断
	CvScalar mean;
	CvScalar dev;
	cvAvgSdv(src_fl,&mean,&dev,NULL);//计算图像的均值和标准差
	double min[3];
	double max[3];
	double maxmin[3];
	for (int i=0;i<3;i++)
	{
		min[i]=mean.val[i]-scale*dev.val[i];
		max[i]=mean.val[i]+scale*dev.val[i];
		maxmin[i]=max[i]-min[i];
	}
	float* data2=(float*)src_fl->imageData;
	int tem_r1,tem_g1,tem_b1;
		for (int i=0;i<src_fl2->width;i++)
		{
			for(int j=0;j<src_fl2->height;j++)
			{
				data2[j*src_fl->widthStep/4+3*i+0]=255*(data2[j*src_fl->widthStep/4+3*i+0]-min[0])/maxmin[0];
				data2[j*src_fl->widthStep/4+3*i+1]=255*(data2[j*src_fl->widthStep/4+3*i+1]-min[1])/maxmin[1];
				data2[j*src_fl->widthStep/4+3*i+2]=255*(data2[j*src_fl->widthStep/4+3*i+2]-min[2])/maxmin[2];
				tem_b1= data2[j*src_fl->widthStep/4+3*i+0];
				tem_g1= data2[j*src_fl->widthStep/4+3*i+1];
				tem_r1=data2[j*src_fl->widthStep/4+3*i+2];
				if(tem_b1>255)	tem_b1=255;		if(tem_b1<0)		tem_b1=0;
				if(tem_g1>255)	tem_g1=255;		if(tem_g1<0)		tem_g1=0;
				if(tem_r1>255)	tem_r1=255;		if(tem_r1<0)		tem_r1=0;
				data2[j*src_fl->widthStep/4+3*i+0]=tem_b1;
				data2[j*src_fl->widthStep/4+3*i+1]=tem_g1;
				data2[j*src_fl->widthStep/4+3*i+2]=tem_r1;
			}
		}

	// Restore
	cvConvertScale(src_fl,result,1,0);

	// Release temp images
	cvReleaseImage(&src_fl);
	cvReleaseImage(&src_fl1);
	cvReleaseImage(&src_fl2);
	cvReleaseImage( &fsA );
	cvReleaseImage( &fsB );
	cvReleaseImage( &fsC );
	cvReleaseImage( &fsD );
	cvReleaseImage( &fsE );
	cvReleaseImage( &fsF );
}
