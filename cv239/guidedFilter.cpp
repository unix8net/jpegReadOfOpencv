/* 
* Copyright(c) 2014,SWUST.
* All rights reserved. 
* 
* 文件名称：guidedFilter.cpp
* 摘    要：guidedFilter实现文件

* 作    者：刁扬桀
* 完成日期：2014年2月27日
*/ 

#include "stdafx.h"
#include <cv.h>	
using namespace cv;
using namespace std;
float eps=0.01;	
int   iteration=5;   
bool my_cvDilate(IplImage *src, IplImage* dst, bool direction, int size)
{
    int height = src->height;
    int width = src->width;
    int y = 0, x =0, value = 0, i = 0;
    uchar* lpPtr = NULL, *lpTempPtr = NULL; 
 
    if( src->nChannels != 1)
    {
        return false;
    }
 
    if(direction == 1)
    {
        for(y=0; y<height; y++)
        {
            //*((uchar*)dst->imageData + y*dst->widthStep + x)
            lpPtr=(uchar*)(src->imageData+y*src->widthStep+1);
            lpTempPtr = (uchar*)(dst->imageData+y*dst->widthStep+1);
 
            for(x=size;x<width-size;x++)
            {
                value=(unsigned char)*lpPtr;
                if (value<128)
                {
                    *lpTempPtr=(unsigned char)0;
                    for(i=-size;i<size;i++)
                    {
                        if (i==0)
                        {
                            continue;
                        }
                        value=(unsigned char)*(lpPtr+i);
                        if(value>128)
                        {
                            *lpTempPtr=(unsigned char)255;
                            break;
                        }
                    }
                }
                else 
                {
                    *lpTempPtr=(unsigned char)255;
                }
                lpPtr++;
                lpTempPtr++;
            }
        }
    }
    else
    {
        for(y=size; y<height-size; y++)
        {
            lpPtr=(uchar*)(src->imageData+y*src->widthStep);
            lpTempPtr = (uchar*)(dst->imageData+y*dst->widthStep);
            for(x=0; x<width; x++)
            {
                value=(unsigned char)*lpPtr;
                if (value<128)
                {
                    *lpTempPtr=(unsigned char)0;
                    for(i=-size;i<size;i++)
                    {
                        if (i==0)
                        {
                            continue;
                        }
                        value=(unsigned char)*(lpPtr+i*dst->widthStep);
                        if(value>128)
                        {
                            *lpTempPtr=(unsigned char)255;
                            break;
                        }
                    }
                }
                else 
                {
                    *lpTempPtr=(unsigned char)255;
                }
                lpPtr++;
                lpTempPtr++;
            }
        }
    }
    return true;
}
 
bool my_cvErode(IplImage *src, IplImage *dst, bool direction, int size)
{
    int height = src->height;
    int width = src->width;
    int y = 0, x =0, value = 0, i = 0;
    uchar* lpPtr = NULL, *lpTempPtr = NULL; 
 
    if( src->nChannels != 1)
    {
        return false;
    }
 
    //dst = my_cvCloneImage(src);
 
    if(direction == 1)
    {
        for(y=0; y<height; y++)
        {
            lpPtr=(uchar*)(src->imageData+y*src->widthStep+1);
            lpTempPtr = (uchar*)(dst->imageData+y*dst->widthStep+1);
 
            for(x=size;x<width-size;x++)
            {
                value=(unsigned char)*lpPtr;
                if (value>128)
                {
                    *lpTempPtr=(unsigned char)255;
                    for(i=-size;i<size;i++)
                    {
                        if (i==0)
                        {
                            continue;
                        }
                        value=(unsigned char)*(lpPtr+i);
                        if(value<128)
                        {
                            *lpTempPtr=(unsigned char)0;
                            break;
                        }
                    }
                }
                else 
                {
                    *lpTempPtr=(unsigned char)0;
                }
                lpPtr++;
                lpTempPtr++;
            }
        }
    }
    else
    {
        for(y=size; y<height-1; y++)
        {
            lpPtr=(uchar*)(src->imageData+y*src->widthStep);
            lpTempPtr = (uchar*)(dst->imageData+y*dst->widthStep);
            for(x=0; x<width; x++)
            {
                value=(unsigned char)*lpPtr;
                if (value>128)
                {
                    *lpTempPtr=(unsigned char)255;
                    for(i=-size;i<size;i++)
                    {
                        if (i==0)
                        {
                            continue;
                        }
                        value=(unsigned char)*(lpPtr+i*dst->widthStep);
                        if(value<128)
                        {
                            *lpTempPtr=(unsigned char)0;
                            break;
                        }
                    }
                }
                else 
                {
                    *lpTempPtr=(unsigned char)0;
                }
                lpPtr++;
                lpTempPtr++;
            }
        }
    }
 
    return true;
}
Mat boxfilter(Mat im,int r)
{
#if 0
	CvMat imCum = cvMat(im.rows,im.cols,CV_32FC1);
	CvMat imT = cvMat(im.rows,im.cols,CV_32FC1);
	CvMat imCumT = cvMat(im.rows,im.cols,CV_32FC1);
	CvMat imDst = cvMat(im.rows,im.cols,CV_32FC1);
#else
	Mat imCum(im.rows,im.cols,CV_32FC1);
	Mat imT(im.cols,im.rows,CV_32FC1);		//im的转置
	Mat imCumT(im.cols,im.rows,CV_32FC1);	//imCumT的转置
	Mat imDst(im.rows,im.cols,CV_32FC1);	//=Mat::zeros(im.rows,im.cols,CV_32FC1);
#endif
//j行i列
//cumulative sum over Y axis
//转置再计算，然后在转回来  imT=im'     
imT=im.t()	;//转置+
for(int j=0;j<imT.rows;j++)
{
	float* da= (float*)(imCumT.ptr(j));//ptr返回行指针
	float* dasrc= (float*)(imT.ptr(j));
	for(int i=0;i<imT.cols;i++)
	{
		if(i==0)
			da[i]=dasrc[i];
		else
			da[i]=dasrc[i]+da[i-1];
	}
}
imCum=imCumT.t();

	//difference over Y axis

	for(int j=0;j<=r;j++)
	{
		float* da= (float*)(imCum.ptr(j+r));
		float* dat= (float*)(imDst.ptr(j));

		for(int i=0;i<im.cols;i++)
		{
			dat[i]=da[i];
		}
	}

	for(int j=r+1;j<=im.rows-r-1;j++)
	{
		float* da1= (float*)(imCum.ptr(j+r));
		float* da2= (float*)(imCum.ptr(j-r-1));
		float* dat= (float*)(imDst.ptr(j));
		for(int i=0;i<im.cols;i++)
		{
			dat[i]=da1[i]-da2[i];
		}
	}

	for(int j=im.rows-r;j<im.rows;j++)
	{
		float* da1= (float*)(imCum.ptr(im.rows-1));
		float* da2= (float*)(imCum.ptr(j-r-1));
		float* dat= (float*)(imDst.ptr(j));

		for(int i=0;i<im.cols;i++)
		{
			dat[i]=da1[i]-da2[i];
		}
	}

//cumulative sum over X axis

	for(int j=0;j<im.rows;j++)
	{
		float* da= (float*)(imCum.ptr(j));
		float* dat= (float*)(imDst.ptr(j));
		for(int i=0;i<im.cols;i++)
		{
			if(i==0)
				 da[i]=dat[i];
			else
				da[i]=dat[i]+da[i-1];
		}
	}

	//difference over X axis
	for(int j=0;j<im.rows;j++)
	{
		float* da= (float*)(imCum.ptr(j));
		float* dat= (float*)(imDst.ptr(j));

		for(int i=0;i<=r;i++)     //i列
		{
			dat[i]=da[i+r];
		}
	}
	for(int j=0;j<im.rows;j++)
	{
		float* da= (float*)(imCum.ptr(j));
		float* dat= (float*)(imDst.ptr(j));

		for(int i=r+1;i<=im.cols-r-1;i++)
		{
			dat[i]=da[i+r]-da[i-r-1];
		}
	}
	for(int j=0;j<im.rows;j++)
	{
		float* da= (float*)(imCum.ptr(j));
		float* dat= (float*)(imDst.ptr(j));

		for(int i=im.cols-r;i<im.cols;i++)
		{
			dat[i]=da[im.cols-1]-da[i-r-1];
		}
	}	
	return imDst;
}
void c_cvDilate(CvMat* src, CvMat* dst, int iterations = 1)
{
	int i, ii, jj, j, ti, tj, leni, lenj, tindex;
	int height = src->height, width = src->width;
	uchar *srcData = 0;
	uchar *dstData = 0;
	int tMaxOrMin = 0;
	while (iterations--)
	{
		for (i = 0; i < height; ++i)
		{
			srcData = (uchar*)(src->data.ptr );
			dstData = (uchar*)(dst->data.ptr + i * dst->step);
			for (j = 0; j < width; ++j)
			{
				tMaxOrMin = 0;
				ti = i - 1, leni, tj = j - 1, lenj;
				if(ti < 0) 
				{
					leni = 2 + ti; 
					ti = 0; 
				}
				else
				{
					leni = i+1;
				}
				if(tj < 0) 
				{
					lenj = 2 + tj; 
					tj = 0; 
				}
				else
				{
					lenj = j + 1;
				}
				for(ii = ti; (ii <=  leni) && (ii < height); ++ii)
				{
					tindex = ii * src->step;
					for (jj = tj; (jj <= lenj) && (jj < width) ; ++jj)
					{
						if (*(srcData + tindex + jj) > tMaxOrMin)
						{
							tMaxOrMin = *(srcData + tindex + jj);
						}
					}
				}
				*(dstData + j) = tMaxOrMin;
			}
		}
	}
}
void c_cvErode(CvMat* src, CvMat* dst, int iterations = 1)
{
	int i, ii, jj, j, ti, tj, leni, lenj, tindex;
	int height = src->height, width = src->width;
	uchar *srcData = 0;
	uchar *dstData = 0;
	int tMaxOrMin = -1;
	while (iterations--)
	{
		for (i = 0; i < height; ++i)
		{
			srcData = (uchar*)(src->data.ptr );
			dstData = (uchar*)(dst->data.ptr + i * dst->step);
			for (j = 0; j < width; ++j)
			{
				tMaxOrMin = 1000;
				ti = i - 1, leni, tj = j - 1, lenj;
				if(ti < 0) 
				{
					leni = 2 + ti; 
					ti = 0; 
				}
				else
				{
					leni = i+1;
				}
				if(tj < 0) 
				{
					lenj = 2 + tj; 
					tj = 0; 
				}
				else
				{
					lenj = j + 1;
				}
				for(ii = ti; (ii <=  leni) && (ii < height); ++ii)
				{
					tindex = ii * src->step;
					for (jj = tj; (jj <= lenj) && (jj < width); ++jj)
					{
						if (*(srcData + tindex + jj) < tMaxOrMin)
						{
							tMaxOrMin = *(srcData + tindex + jj);
						}
					}
				}
				*(dstData + j) = tMaxOrMin;
			}
		}
	}
}
void guidedFilter(IplImage *Imin,IplImage *jdark,int r)
{
	#define  USE_BOTH_IP
	IplImage *jdark_zoomout,*Imin_zoomout;
	int NN=4;
	int width_z=Imin->width/NN;
	int height_z=Imin->height/NN;
	jdark_zoomout=cvCreateImage(cvSize(width_z,height_z),8,1); //缩放
	Imin_zoomout=cvCreateImage(cvSize(width_z,height_z),8,1);
	cvResize(Imin,Imin_zoomout); 


	//粗Jdark-> Mat input_P  【Imin滤波后作为输入】FILTERING INPUT
#ifdef USE_BOTH_IP
	//粗jdark作为导向滤波的输入                                                                    
# if 1
	cvErode(Imin_zoomout,jdark_zoomout,cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT),iteration);    	    
	cvDilate(jdark_zoomout,jdark_zoomout,cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT),iteration);   
#endif
#if 0
	my_cvErode(Imin_zoomout,jdark_zoomout,1,3);    	    
	my_cvDilate(jdark_zoomout,jdark_zoomout,1,3); 
#endif
#if 0
	CvMat*imin_mat=cvCreateMat(height_z,width_z,CV_8UC1);
	cvConvert(Imin_zoomout,imin_mat);
	CvMat* jdark_mat=cvCreateMat(height_z,width_z,CV_8UC1);
	cvConvert(jdark_zoomout,jdark_mat);

	//cvErode(Imin_zoomout,jdark_zoomout,cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT),iteration);    	    
	//cvDilate(jdark_zoomout,jdark_zoomout,cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT),iteration); 
	c_cvErode(imin_mat,jdark_mat);
	c_cvDilate(jdark_mat,jdark_mat);
	cvGetImage(jdark_mat,jdark_zoomout);

#endif
	Mat input_P(jdark_zoomout->height,jdark_zoomout->width,CV_32FC1);                                         
	for(int y=0;y<jdark_zoomout->height;y++)                                                                    
	{
		unsigned char *jdarkRow= (unsigned char*)(jdark_zoomout->imageData+y*jdark_zoomout->widthStep);         
		float* inputRow= (float*)(input_P.ptr(y));		
		for (int x=0;x<jdark_zoomout->width;x++)                                                                
		{  	 
			inputRow[x]=(float)(jdarkRow[x])/255;
		}	
	}
#endif
	//Imin -> Mat     【Imin作为导向图。】
	Mat guide_I(Imin_zoomout->height,Imin_zoomout->width,CV_32FC1);                                              
	for(int y=0;y<Imin_zoomout->height;y++)
	{
		unsigned char *row1= (unsigned char*)(Imin_zoomout->imageData+y*Imin_zoomout->widthStep);                
		float* gdRow= (float*)(guide_I.ptr(y));		
		for (int x=0;x<Imin_zoomout->width;x++)                                                                      
		{  	
			gdRow[x]=(float)(row1[x])/255;	
		}		
	}
	//导向图guide_I,输入图 input_P`   
	Mat oneone =Mat::ones(Imin_zoomout->height,Imin_zoomout->width,CV_32FC1);
	Mat  N(height_z,width_z,CV_32FC1);   //N:the size of each local patch; N=(2r+1)^2 except for boundary pixels.
	Mat mean_I(height_z,width_z,CV_32FC1);   	
	Mat mean_II(height_z,width_z,CV_32FC1); 
	N=boxfilter(oneone,r);   
	mean_I=boxfilter(guide_I,r)/N;
	mean_II=boxfilter(guide_I.mul(guide_I),r)/N;
#ifdef USE_BOTH_IP
	Mat mean_P(height_z,width_z,CV_32FC1);
	Mat mean_IP(height_z,width_z,CV_32FC1); 
	Mat a_a(height_z,width_z,CV_32FC1);
	Mat b_b(height_z,width_z,CV_32FC1);

	mean_P=boxfilter(input_P,r)/N;	
	mean_IP=boxfilter(guide_I.mul(input_P),r)/N;
	a_a= (mean_IP - mean_I.mul(mean_P))/(mean_II- mean_I.mul(mean_I) +eps);			//公式（5） //其中【局部块的协方差cov_IP】=mean_IP - mean_I.mul(mean_P)
	b_b= mean_P-a_a.mul(mean_I);													//公式（6）
#endif
#ifdef USE_SINGLE_I
	Mat a_a = (mean_II - mean_I.mul(mean_I))/(mean_II- mean_I.mul(mean_I) +eps);     
	Mat b_b = mean_I-a_a.mul(mean_I);                                                
#endif	
	//Mat mean_a=	boxfilter(a_a,r)/N;
	//Mat mean_b=	boxfilter(b_b,r)/N;
	Mat output_Jdark= a_a.mul(guide_I)+b_b;
	//Mat output_Jdark= mean_a.mul(guide_I)+mean_b;
	//mat->iplimage;      30ms使用指针后1ms 
	for(int y=0;y<jdark_zoomout->height;y++)                                                             
	{
		unsigned char *row1= (unsigned char*)(jdark_zoomout->imageData+y*jdark_zoomout->widthStep);     
		float* rowjdark= (float*)(output_Jdark.ptr(y));		
		for (int x=0;x<jdark_zoomout->width;x++)                                                        
		{  	
			if (rowjdark[x]<0) rowjdark[x]=0; //注：output_jdark有<0的数！！
			row1[x] =255*rowjdark[x];	  
		}		
	}

	cvResize(jdark_zoomout,jdark);           //放大至原尺寸

	//cvReleaseImage(&jdark_zoomout);
	cvReleaseImage(&Imin_zoomout);
}

