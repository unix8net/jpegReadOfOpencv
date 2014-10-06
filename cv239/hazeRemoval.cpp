/* 
* Copyright(c) 2014,SWUST.
* All rights reserved. 
* 
* 文件名称：hazeremoval.cpp
* 摘    要：去雾

* 作    者：刁扬桀
* Created on 2013.9.11.
* 完成日期：2014年2月27日  
*/ 
#include "stdafx.h"
#include <stdlib.h>
#include <cv.h>
#include "guidedFilter.h"
using namespace cv;
using namespace std;

  
float t_o=0.2;			// 去雾，透射率下限
int a=255;				// 去雾，大气光值
void hazeRemoval(const IplImage*src, IplImage *result)
{

	int r=20; 
	int h=5;						 // 在前h分之1取A值
	int ar,ag,ab;					 // 大气光值，rgb分开
	int posA_hei=0,posA_wid=0;	     // 记录Jdark最大值所在的位置。
	float w=0.9f;			         
	int x,y;              
	IplImage *jdark,*Imin;
	Imin=cvCreateImage(cvGetSize(src),8,1);	
	jdark =cvCreateImage(cvGetSize(src),8,1); 

	/*                  mim(r,g,b) =>  Imin                                 */
	for(y=0;y<src->height;y++)
	{
		unsigned char *srcRow = (unsigned char*)(src->imageData +y*src->widthStep );       
		unsigned char *IminRow= (unsigned char*)(Imin->imageData+y*Imin->widthStep);      
		for (x=0;x<src->width;x++)
		{   
			IminRow[x]=srcRow[3*x];
			if(srcRow[3*x+1]<IminRow[x]) IminRow[x]=srcRow[3*x+1];
			if(srcRow[3*x+2]<IminRow[x]) IminRow[x]=srcRow[3*x+2];	
		}
	}

	/*               guidedFilter									        */
	r=Imin->width;if (Imin->height<r)r=Imin->height;
	r=r/30;//3-29
	guidedFilter(Imin,jdark,r);

	/*                         A                                            */
	a=0;//需清零
	for(y=2;y<jdark->height/h;y++)
	{	
		unsigned char *jdarkRow= (unsigned char*)(jdark->imageData+y*jdark->widthStep);
		for (x=0;x<jdark->width;x++)
			if (jdarkRow[x]>a) 
			{
				a=jdarkRow[x];
				posA_hei=y;
				posA_wid=x;
			}		
	}
	unsigned char *aRow= (unsigned char*)(src->imageData+posA_hei*src->widthStep);
	ab=aRow[3*posA_wid];  //if (ab>240)ab=240;
	ag=aRow[3*posA_wid+1];//if (ag>240)ag=240;
	ar=aRow[3*posA_wid+2];//if (ar>240)ar=240;

	//在原图中对应位置去rgb三通道的最大值，效果最好,颜色也更加真实。——2013.10.21 
	a=ar; if(ag>a)a=ag;if(ab>a) a=ab; 
	//处理视频时，采用滑动平均求A，略

	if (a<150)
	{
		a=150;
	}
	/************************************************************************/
	/*                  Transmission map                                    */
	/************************************************************************/ 
	Mat tm(jdark->height,src->width,CV_32FC1);  
	for(y=0;y<jdark->height;y++)
	{
		unsigned char *jdarkRow= (unsigned char*)(jdark->imageData+y*jdark->widthStep);
		float  * tRow = (float*) (tm.ptr(y));		
		for (x=0;x<jdark->width;x++)
		{  	
			tRow[x]=1-w*(float)(jdarkRow[x])/a;
			if(tRow[x]>1)	tRow[x]=1;
			else if(tRow[x]<t_o)   	tRow[x]=t_o;
		}		
	}	
	/************************************************************************/
	/*                        recover J                                     */
	/************************************************************************/
	int tem_b,tem_g,tem_r;   
	for(y=0;y<src->height;y++)
	{
		unsigned char *srcRow   = (unsigned char*)(src->imageData    + y*src->widthStep   );
		unsigned char *resultRow= (unsigned char*)(result->imageData + y*result->widthStep);//
		unsigned char *jdarkRow= (unsigned char*)(jdark->imageData+y*jdark->widthStep);			
		float* tRow= (float*)(tm.ptr(y));      
		for (x=0;x<src->width;x++)
		{  	
			tem_b	= (srcRow[3*x]  -a)/tRow[x] +a;
			tem_g	= (srcRow[3*x+1]-a)/tRow[x] +a;
			tem_r	= (srcRow[3*x+2]-a)/tRow[x] +a;
			if (tem_b>255)tem_b=255;	else if (tem_b<0) tem_b=0;		resultRow[3*x]  =tem_b;
			if (tem_g>255)tem_g=255;	else if (tem_g<0) tem_g=0;		resultRow[3*x+1]=tem_g;
			if (tem_r>255)tem_r=255;	else if (tem_r<0) tem_r=0;		resultRow[3*x+2]=tem_r;
		}
	}

	cvReleaseImage(&jdark);
	cvReleaseImage(&Imin);
}

