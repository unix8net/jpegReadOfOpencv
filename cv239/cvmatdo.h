#ifndef _CVMATDO_H_1
#define _CVMATDO_H_1
#include "cxtypes.h"
#include "mem.h"

CV_INLINE void icvCheckHuge( CvMat* arr )
{
    if( (int64)arr->step*arr->rows > INT_MAX )
        arr->type &= ~CV_MAT_CONT_FLAG;
}

// Creates CvMat header only
CV_INLINE CvMat*
cvCreateMatHeader( int rows, int cols, int type )
{
    type = CV_MAT_TYPE(type);

	assert((rows > 0) && (cols > 0));
    int min_step = CV_ELEM_SIZE(type)*cols;
	assert(min_step > 0);
    
    CvMat* arr = (CvMat*)fastMalloc( sizeof(*arr));

    arr->step = min_step;
    arr->type = CV_MAT_MAGIC_VAL | type | CV_MAT_CONT_FLAG;
    arr->rows = rows;
    arr->cols = cols;
    arr->data.ptr = 0;
    arr->refcount = 0;
    arr->hdr_refcount = 1;

    icvCheckHuge( arr );
    return arr;
}

CV_INLINE void
cvCreateData( CvMat* arr )
{
    if( CV_IS_MAT_HDR( arr ))
    {
        size_t step, total_size;
        CvMat* mat = (CvMat*)arr;
        step = mat->step;

		assert( mat->data.ptr == 0);
      
        if( step == 0 )
            step = CV_ELEM_SIZE(mat->type)*mat->cols;

        int64 _total_size = (int64)step*mat->rows + sizeof(int) + CV_MALLOC_ALIGN;
        total_size = (size_t)_total_size;

		assert(_total_size == (int64)total_size);
        
        mat->refcount = (int*)fastMalloc( (size_t)total_size );
        mat->data.ptr = (uchar*)cvAlignPtr( mat->refcount + 1, CV_MALLOC_ALIGN );
        *mat->refcount = 1;
    }
}

CV_INLINE CvMat*
cvCreateMat( int height, int width, int type )
{
    CvMat* arr = cvCreateMatHeader( height, width, type );
    cvCreateData( arr );

    return arr;
}
CV_INLINE  void  cvDecRefData( CvMat* arr )
{
    if( CV_IS_MAT( arr ))
    {
        CvMat* mat = (CvMat*)arr;
        mat->data.ptr = NULL;
        if( mat->refcount != NULL && --*mat->refcount == 0 )
            cvFree( &mat->refcount );
        mat->refcount = NULL;
    }

}


CV_INLINE void
cvReleaseData( CvMat* arr )
{
    if( CV_IS_MAT_HDR( arr ))
    {
        CvMat* mat = (CvMat*)arr;
        cvDecRefData( mat );
    }
}



CV_INLINE void
cvReleaseMat( CvMat** array )
{
	assert(array);
    if( *array )
    {
        CvMat* arr = *array;
          
        *array = 0;

        cvDecRefData( arr );
        cvFree( &arr );
    }
}
CV_INLINE  CvSize  cvGetMatSize( const CvMat* mat )
{
    CvSize size;
    size.width = mat->cols;
    size.height = mat->rows;
    return size;
}
#endif