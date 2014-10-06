#ifndef _GRFMT_BASE_H_
#define _GRFMT_BASE_H_

#include "utils.h"
#include "bitstrm.h"
#include "cxtypes.h"
//#include <iostream>
using namespace std;
#include <vector>
#include <string>

#if 0
	template<typename _Tp> class  Ptr
	{
	public:
		Ptr();
		Ptr(_Tp* _obj);
		~Ptr();
		Ptr(const Ptr& ptr);
		Ptr& operator = (const Ptr& ptr);
		void addref();
		void release();
		void delete_obj();
		bool empty() const;

		_Tp* operator -> ();
		const _Tp* operator -> () const;

		operator _Tp* ();
		operator const _Tp*() const;
	protected:
		_Tp* obj;
		int* refcount;
	};
	template<typename _Tp> inline Ptr<_Tp>::Ptr() : obj(0), refcount(0) {}
	template<typename _Tp> inline Ptr<_Tp>::Ptr(_Tp* _obj) : obj(_obj)
	{
		if(obj)
		{
			refcount = (int*)malloc(sizeof(*refcount));
			*refcount = 1;
		}
		else
			refcount = 0;
	}

	template<typename _Tp> inline void Ptr<_Tp>::addref()
	{ if( refcount ) (*refcount) ++ ;}

	template<typename _Tp> inline void Ptr<_Tp>::release()
	{
		if( refcount && CV_XADD(refcount, -1) == 1 )
		{
			delete_obj();
			delete refcount;
		}
		refcount = 0;
		obj = 0;
	}

	template<typename _Tp> inline void Ptr<_Tp>::delete_obj()
	{
		if( obj ) delete obj;
	}

	template<typename _Tp> inline Ptr<_Tp>::~Ptr() { release(); }

	template<typename _Tp> inline Ptr<_Tp>::Ptr(const Ptr<_Tp>& ptr)
	{
		obj = ptr.obj;
		refcount = ptr.refcount;
		addref();
	}

	template<typename _Tp> inline Ptr<_Tp>& Ptr<_Tp>::operator = (const Ptr<_Tp>& ptr)
	{
		int* _refcount = ptr.refcount;
		if( _refcount )
			(*_refcount) ++ ;
		release();
		obj = ptr.obj;
		refcount = _refcount;
		return *this;
	}

	template<typename _Tp> inline _Tp* Ptr<_Tp>::operator -> () { return obj; }
	template<typename _Tp> inline const _Tp* Ptr<_Tp>::operator -> () const { return obj; }

	template<typename _Tp> inline Ptr<_Tp>::operator _Tp* () { return obj; }
	template<typename _Tp> inline Ptr<_Tp>::operator const _Tp*() const { return obj; }

	template<typename _Tp> inline bool Ptr<_Tp>::empty() const { return obj == 0; }
#endif


class BaseImageDecoder;
class BaseImageEncoder;
typedef BaseImageEncoder* ImageEncoder;
typedef BaseImageDecoder* ImageDecoder;

///////////////////////////////// base class for decoders ////////////////////////
class BaseImageDecoder
{
public:
    BaseImageDecoder();
    virtual ~BaseImageDecoder() {};

    int width() const { return m_width; };
    int height() const { return m_height; };
    int type() const { return m_type; };
	string signature() const {return m_signature;}
    virtual bool setSource( const string& filename );
    virtual bool setSource( const CvMat* buf );
    virtual bool readHeader() = 0;
    virtual bool readData( CvMat& img ) = 0;

    virtual size_t signatureLength() const;
    virtual bool checkSignature( const string& signature ) const;
    virtual ImageDecoder newDecoder() const;

protected:
    int  m_width;  // width  of the image ( filled by readHeader )
    int  m_height; // height of the image ( filled by readHeader )
    int  m_type;
    string m_filename;
    string m_signature;
    CvMat m_buf;
    bool m_buf_supported;
};


///////////////////////////// base class for encoders ////////////////////////////
class BaseImageEncoder
{
public:
    BaseImageEncoder();
    virtual ~BaseImageEncoder() {};
    virtual bool isFormatSupported( int depth ) const;

    virtual bool setDestination( const string& filename );
    virtual bool setDestination( vector<uchar>& buf );
    virtual bool write( const CvMat& img, const vector<int>& params ) = 0;

    virtual string getDescription() const;
    virtual ImageEncoder newEncoder() const;

protected:
    string m_description;
    
    string m_filename;
    vector<uchar>* m_buf;
    bool m_buf_supported;
};


#endif/*_GRFMT_BASE_H_*/
