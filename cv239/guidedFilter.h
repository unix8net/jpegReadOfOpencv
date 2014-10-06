/* 
* Copyright(c) 2014,SWUST.
* All rights reserved. 
* 
* 文件名称：guidedFilter.h
* 摘    要：guidedFilter头文件，导向滤波功能。
*           参数：输入图Imin，输出滤波结果jdark，滤波半径r。

* 作    者：刁扬桀
* 完成日期：2014年2月27日
*/ 
#ifndef MYGUIDEDFILTER_
#define MYGUIDEDFILTER_
#include <cv.h>

void guidedFilter(IplImage *Imin,IplImage *jdark,int r);       

#endif

