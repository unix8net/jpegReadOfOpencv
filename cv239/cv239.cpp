// cv239.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
//#include "highgui.h"
#include <time.h>
#include <iostream>
#include "grfmt_jpeg.h"
#include "cvmatdo.h"
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

int main()
{
	clock_t start, end;

	JpegDecoder jpegDector;
	jpegDector.setSource("21.jpg");
	jpegDector.readHeader();
	int height = jpegDector.height();
	int width = jpegDector.width();
	cout<<"高为:"<<height<<"   宽为："<<width<<endl;
	cout<<"通道数："<<CV_MAT_CN(jpegDector.type())<<endl;
	cout<<"sign："<<jpegDector.signature()<<endl;
	
	CvMat *mat = cvCreateMat(jpegDector.height(), jpegDector.width(),(jpegDector.type()));
	jpegDector.readData(*mat);
	
	unsigned char * data;
	for(int i=0;i<10;i++)
	{
		data = (uchar*)(mat->data.ptr + i*mat->step);
		for(int j=0;j<10;j++)
		{
			printf("%d ",data[j*3]);
			printf("%d ",data[j*3+1]);
			printf("%d   ",data[j*3+2]);
		}
		printf("\n");
	}
	printf("------cvMat-------\n\n");
	cvReleaseMat(&mat);
	//CvMat * imgSrc = cvLoadImageM("1.jpg");
#if 0
	IplImage *imgSrc = cvLoadImage("21.jpg");
	IplImage *imgRes = cvCreateImage(cvGetSize(imgSrc),8,3);
	start = clock();
	cout<<imgSrc->nChannels<<"  "<<imgSrc->depth<<endl;
	//IplImage *imgRes = cvCreateImageHeader(cvGetSize(imgSrc),8,3);
	//cvNamedWindow("test source", CV_WINDOW_AUTOSIZE);
	cvShowImage("source", imgSrc);
	end = clock();
	//cvNamedWindow("test result", CV_WINDOW_AUTOSIZE);
	cvShowImage("result", imgRes);
#endif	
	//printf("Run time:%f ",(double)(end - start) );
	//cvWaitKey(0);
	system("pause");
}