#pragma once
//////////////////////////////////////////////////////////////////////////
#ifndef WIN32
#	ifndef __LINUX__
#		define __LINUX__
#	endif
#	ifndef FALSE
#		define FALSE 0
#	endif
#	ifndef TRUE
#		define TRUE 1
#	endif
#endif
//////////////////////////////////////////////////////////////////////////
namespace stdtype {
#if !defined(UIB_STD_TYPE_DEF)
#	define  UIB_STD_TYPE_DEF
#	ifdef WIN32
#		define ULL
#	else
#		define ULL LLU
#	endif
	typedef unsigned long long	ULongLong;
	typedef ULongLong		    TResourceID;
	typedef unsigned long long  TTimestamp;
	typedef TTimestamp          TDateTime;
	typedef long                TRESULT;

	typedef unsigned char		TUint8; 
	typedef unsigned short		TUint16;
	typedef unsigned long		TUint32;
	typedef ULongLong			TUint64;

	typedef char				TInt8;
	typedef short				TInt16;
	typedef long				TInt32;
	typedef long long			TInt64;

	typedef unsigned char       TByte;
	typedef float               TFloat32;
	typedef double              TFloat64;
	//////////////////////////////////////////////////////////////////////////
#	define MAX_UINT64_VALUE (0xFFFFFFFFFFFFFFFFULL)
#	define MAX_RESOURCE_ID	(0xFFFFFFFFFFFFFFFFULL)

#	define HI_32_MASK		(0xFFFFFFFF00000000ULL)
#	define LO_32_MASK		(0x00000000FFFFFFFFULL)

#	define H2_16_MASK		(0xFFFF000000000000ULL)
#	define H1_16_MASK		(0x0000FFFF00000000ULL)
#	define L2_16_MASK		(0x00000000FFFF0000ULL)
#	define L1_16_MASK		(0x000000000000FFFFULL)
#endif
}
//2) 缺省类定义
#if !defined(UIB_CLASS_DEFAULT_DEF) && !defined(UIB_CLASS_VIRTUAL_DEF)
#	define UIB_CLASS_DEFAULT_DEF(CLASSNAME) \
	public: \
		CLASSNAME(); CLASSNAME(const CLASSNAME& o); \
		const CLASSNAME& operator=(const CLASSNAME& o); \
		~CLASSNAME()
#	define UIB_CLASS_VIRTUAL_DEF(CLASSNAME) \
	public: \
		CLASSNAME(); CLASSNAME(const CLASSNAME& o); \
		const CLASSNAME& operator=(const CLASSNAME& o); \
		virtual ~CLASSNAME()	
#endif //UIB_CLASS_DEFAULT_DEF
//3) 
#if !defined(UIB_INTERFACE_DEFAULT_DEF)
#	define UIB_INTERFACE_DEFAULT_DEF(CLASSNAME) \
	protected: \
		CLASSNAME(); CLASSNAME(const CLASSNAME& o); \
		const CLASSNAME& operator=(const CLASSNAME& o); \
	public: \
		virtual ~CLASSNAME()
#	define UIB_INTERFACE_DEFAULT_IMPL(CLASSNAME) \
		CLASSNAME::CLASSNAME() {} \
		CLASSNAME::~CLASSNAME() {}
#endif //UIB_INTERFACE_DEFAULT_DEF
//////////////////////////////////////////////////////////////////////////
//3) 
#if !defined(UIB_CLASS_RTTI_DEF)
#	define UIB_CLASS_RTTI_DEF(VALUE_) \
	public: \
		static TUint32 GetClassType(void) { return VALUE_; } \
		virtual TUint32 GetType(void) const { return VALUE_; } \
		virtual bool IsTypeOf(TUint32 type_) { return (type_ == VALUE_); }
#endif
//////////////////////////////////////////////////////////////////////////
//4)
#if !defined(UIB_EXT_AUTO_PTR_DEF) && !defined(UIB_EXT_AUTO_PTR_IMPL)
#	define UIB_EXT_AUTO_PTR_DEF(DATA_TYPE,DATA_TYPE_VAR) \
		class DATA_TYPE_VAR : public std::auto_ptr< DATA_TYPE > \
		{ \
		public: \
			DATA_TYPE_VAR(); \
			virtual ~DATA_TYPE_VAR(); \
		}
#	define UIB_EXT_AUTO_PTR_IMPL(DATA_TYPE,DATA_TYPE_VAR) \
		DATA_TYPE_VAR::DATA_TYPE_VAR() : std::auto_ptr< DATA_TYPE >() { } \
		DATA_TYPE_VAR::~DATA_TYPE_VAR() { DATA_TYPE* p = release(); delete p;  }
#endif //
//////////////////////////////////////////////////////////////////////////