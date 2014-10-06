#ifndef _MEMDO_H_1
#define _MEMDO_H_1
#include "cxtypes.h"
#define  CV_MALLOC_ALIGN    16
#define cvFree(ptr) (cvFree_(*(ptr)), *(ptr)=0)

template<typename _Tp> static inline _Tp* alignPtr(_Tp* ptr, int n=(int)sizeof(_Tp))
{
    return (_Tp*)(((size_t)ptr + n-1) & -n);
}

CV_INLINE void* cvAlignPtr( const void* ptr, int align CV_DEFAULT(32) )
{
    assert( (align & (align-1)) == 0 );
    return (void*)( ((size_t)ptr + align - 1) & ~(size_t)(align-1) );
}

CV_INLINE int cvAlign( int size, int align )
{
    assert( (align & (align-1)) == 0 && size < INT_MAX );
    return (size + align - 1) & -align;
}

CV_INLINE void* fastMalloc( size_t size )
{
    uchar* udata = (uchar*)malloc(size + sizeof(void*) + CV_MALLOC_ALIGN);
    if(!udata)
        return 0;
    uchar** adata = alignPtr((uchar**)udata + 1, CV_MALLOC_ALIGN);
    adata[-1] = udata;
    return adata;
}


CV_INLINE void fastFree(void* ptr)
{
    if(ptr)
    {
        uchar* udata = ((uchar**)ptr)[-1];
        assert(udata < (uchar*)ptr &&
               ((uchar*)ptr - udata) <= (ptrdiff_t)(sizeof(void*)+CV_MALLOC_ALIGN)); 
        free(udata);
    }
}
CV_INLINE void cvFree_( void* ptr )
{
    fastFree( ptr );
}
#endif