//
//  lava_base.h
//
//  Created by Colin on 2019-06-14.
//  Copyright (c) 2019 Sumscope. All rights reserved.
//
#pragma once
#include <stdint.h>

#ifdef LAVA_WIN
//#include <vld.h>
#endif

#ifdef __lv_in
#undef __lv_in
#define __lv_in 
#else
#define __lv_in 
#endif

#ifdef __lv_out
#undef __lv_out
#define __lv_out  
#else
#define __lv_out
#endif

#ifdef __lv_inout
#undef __lv_inout
#define __lv_inout  
#else
#define __lv_inout
#endif

#ifdef not_implemented
#undef not_implemented
#define not_implemented -1  
#else
#define not_implemented -1
#endif

#ifndef abstract
#define abstract
#endif

//#ifndef nullptr
//#define nullptr 0
//#endif

#ifdef _max_path
#undef _max_path
#define _max_path	512  
#else
#define _max_path	512
#endif

#ifdef _min_path
#undef _min_path
#define _min_path	256  
#else
#define _min_path	256
#endif

#ifdef MAX_RECORD_FIELD
#undef MAX_RECORD_FIELD
#define MAX_RECORD_FIELD		4095
#else
#define MAX_RECORD_FIELD		4095
#endif

#ifdef MAX_USERNAME_LEN
#undef MAX_USERNAME_LEN
#define MAX_USERNAME_LEN		128
#else
#define MAX_USERNAME_LEN		128
#endif

#ifdef MAX_PASSWORD_LEN
#undef MAX_PASSWORD_LEN
#define MAX_PASSWORD_LEN		128
#else
#define MAX_PASSWORD_LEN		128
#endif

#ifdef MAX_CLIENT_ID_LEN
#undef MAX_CLIENT_ID_LEN
#define MAX_CLIENT_ID_LEN		128
#else
#define MAX_CLIENT_ID_LEN		128
#endif

#ifdef INFINITE
#undef INFINITE
#define INFINITE  0xFFFFFFFF 
#else
#define INFINITE  0xFFFFFFFF
#endif

#if defined(LAVA_LINUX) || defined(LAVA_MACOSX)
#define sprintf_s snprintf
#endif

#ifdef LAVA_LINUX
#ifndef __stdcall
#define __stdcall
#endif
#endif

#ifdef LAVA_WIN
	#define barrier()	MemoryBarrier()
	#define mb()		_ReadWriteBarrier()
	#define rmb()		_ReadBarrier()
	#define wmb()		_WriteBarrier()
	#ifdef CONFIG_SMP 
		#define smp_mb() mb() 
		#define smp_rmb() rmb() 
		#define smp_wmb() wmb() 
		#define smp_read_barrier_depends() read_barrier_depends() 
		#define set_mb(var, value) do { (void) xchg(&var, value); } while (0) 
	#else 
		#define smp_mb()  barrier() 
		#define smp_rmb() barrier() 
		#define smp_wmb() barrier() 
		#define smp_read_barrier_depends() do { } while(0) 
		#define set_mb(var, value) do { var = value; barrier(); } while (0) 
	#endif
	#define smt_pause()	_mm_pause()
#else // LAVA_MACOSX or LAVA_LINUX
	#ifdef X64
		#define mb() 	asm volatile("mfence":::"memory")
		#define rmb()	asm volatile("lfence":::"memory")
		#define wmb()	asm volatile("sfence" ::: "memory")
	#else
		#define X86_FEATURE_XMM2 (0*32+26)					/* Streaming SIMD Extensions-2 */
		#define alternative(oldinstr, newinstr, feature)	\
		asm volatile ("661:\n\t" oldinstr "\n662:\n"		\
		  ".section .altinstructions,\"a\"\n"				\
		  "   .align 4\n"									\
		  "   .long 661b\n"         /* label */				\
		  "   .long 663f\n"			/* new instruction */	\
		  "   .byte %c0\n"          /* feature bit */		\
		  "   .byte 662b-661b\n"    /* sourcelen */			\
		  "   .byte 664f-663f\n"    /* replacementlen */	\
		  ".previous\n"										\
		  ".section .altinstr_replacement,\"ax\"\n"			\
		  "663:\n\t" newinstr "\n664:\n" /* replacement */	\
		  ".previous" :: "i" (feature) : "memory")
		#define barrier() __asm__ __volatile__("": : :"memory") 
		#define mb()  alternative("lock; addl $0,0(%%esp)", "mfence", X86_FEATURE_XMM2) //asm volatile("mfence":::"memory")  
		#define rmb() alternative("lock; addl $0,0(%%esp)", "lfence", X86_FEATURE_XMM2) //asm volatile("lfence":::"memory") 
		#define wmb() alternative("lock; addl $0,0(%%esp)", "sfence", X86_FEATURE_XMM2)	//asm volatile("sfence":::"memory")  
	#endif
	#ifdef CONFIG_SMP 
		#define smp_mb() mb() 
		#define smp_rmb() rmb() 
		#define smp_wmb() wmb() 
		#define smp_read_barrier_depends() read_barrier_depends() 
		#define set_mb(var, value) do { (void) xchg(&var, value); } while (0) 
	#else 
		#define smp_mb() barrier() 
		#define smp_rmb() barrier() 
		#define smp_wmb() barrier() 
		#define smp_read_barrier_depends() do { } while(0) 
		#define set_mb(var, value) do { var = value; barrier(); } while (0) 
	#endif
	#define smt_pause() __asm__ __volatile__( "rep; nop" : : : "memory" );
#endif

#ifdef LAVA_WIN
#define PATHSEP		"\\"
#define PATHSEPCHAR	'\\'
#define DLLPREFIX	""
#define DLLSUFFIX	".dll"
#define EXESUFFIX	".exe"
#define CONFIGPATH	"\\..\\config\\"
#define LOGPATH		"\\..\\log\\"
#elif defined (LAVA_MACOSX)
#define PATHSEP		"/"
#define PATHSEPCHAR	'/'
#define DLLPREFIX	"lib"
#define DLLSUFFIX	".dylib"
#define EXESUFFIX	""
#define CONFIGPATH	"/../config/"
#define LOGPATH		"/../log/"
#else // LAVA_LINUX
#define PATHSEP		"/"
#define PATHSEPCHAR	'/'
#define DLLPREFIX	"lib"
#define DLLSUFFIX	".so"
#define EXESUFFIX	""
#define CONFIGPATH	"/../config/"
#define LOGPATH		"/../log/"
#endif

#define release_pointer(p)	\
	if ( p )			\
	{					\
		delete p;		\
		p = nullptr;	\
	}

#define release_array_pointer(p)	\
	if ( p )			\
	{					\
		delete []p;		\
		p = nullptr;	\
	}

namespace std
{
    namespace tr1 {}
}

namespace lava
{
	namespace base
	{
		class noncopyable
		{
		protected:
			noncopyable() {}
			~noncopyable() {}
			noncopyable(const noncopyable&) = delete;
			noncopyable& operator = (const noncopyable&) = delete;
		};

		template< class locker >
		class scopelocker : public noncopyable
		{
		public:
			explicit scopelocker(locker& l) : m_lock(l)
			{
				m_lock.lock();
			}
			~scopelocker()
			{
				m_lock.unlock();
			}

		private:
			locker&		m_lock;
		};

		template< class locker >
		class scopelocker_try : public noncopyable
		{
		public:
			explicit scopelocker_try(locker& l) : m_lock(l), m_try_ok(false)
			{
				m_try_ok = m_lock.try_lock();
			}
			~scopelocker_try()
			{
				if (m_try_ok)
					m_lock.unlock();
			}
			bool try_ok()
			{
				return m_try_ok;
			}

		private:
			locker&		m_lock;
			bool		m_try_ok;
		};

		template< class locker >
		class scopeunlocker : public noncopyable
		{
		public:
			explicit scopeunlocker(locker& l) : m_lock(l)
			{
				m_lock.unlock();
			}
			~scopeunlocker()
			{
				m_lock.lock();
			}

		private:
			locker&		m_lock;
		};

		struct i_object
		{
			virtual int32_t reserved(void*, void**) = 0;
			virtual int32_t add_ref() = 0;
			virtual int32_t release() = 0;
		};

		template< typename i_plain_type >
		class i_auto_ptr
		{
		public:
			i_auto_ptr() : m_ptr(nullptr) {}
			template<typename derived_type>
			i_auto_ptr(derived_type* object)
			{
				m_ptr = static_cast<i_plain_type*>(object);
				if (m_ptr)
					m_ptr->add_ref();
			}
			i_auto_ptr(i_plain_type* ptr_) : m_ptr(ptr_)
			{
				if (m_ptr)
					m_ptr->add_ref();
			}
			i_auto_ptr(const i_auto_ptr& src_) : m_ptr(src_.m_ptr)
			{
				if (m_ptr)
					m_ptr->add_ref();
			}
			i_auto_ptr(i_auto_ptr&& src_) : m_ptr(src_.m_ptr)
			{
				src_.m_ptr = nullptr;
			}
			~i_auto_ptr()
			{
				if (m_ptr)
					m_ptr->release();
			}
			i_auto_ptr& operator = (const i_auto_ptr& rhs_)
			{
				if ( this == &rhs_ )
					return *this;

				if (m_ptr)
					m_ptr->release();

				m_ptr = rhs_.m_ptr;
				if (m_ptr)
					m_ptr->add_ref();

				return *this;
			}
			i_auto_ptr& operator = (i_auto_ptr&& rhs_)
			{
				if (m_ptr)
					m_ptr->release();

				m_ptr = rhs_.m_ptr;
				rhs_.m_ptr = nullptr;

				return *this;
			}
			bool operator == (const i_auto_ptr& rhs_) const
			{
				return m_ptr == rhs_.m_ptr;
			}
			bool operator == (const i_plain_type* rhs_) const
			{
				return m_ptr == rhs_;
			}
			bool operator < (const i_auto_ptr& rhs_) const
			{
				return m_ptr < rhs_.m_ptr;
			}
			i_plain_type* operator -> () const
			{
				return m_ptr;
			}
			operator bool() const
			{
				return m_ptr != nullptr;
			}
			void attach(i_plain_type* ptr_)
			{
				if (m_ptr)
					m_ptr->release();
				m_ptr = ptr_;
			}
			i_plain_type* detach()
			{
				i_plain_type* tmp = m_ptr;
				m_ptr = nullptr;
				return tmp;
			}
			i_plain_type* get() const
			{
				return m_ptr;
			}

		private:
			i_plain_type*	m_ptr;
		};

		template<class T>
		struct stack_object : public T
		{
			virtual int32_t reserved(void*, void**){ return not_implemented; }
			virtual int32_t add_ref() { return 0; /*nothing to do;*/ }
			virtual int32_t release() { return 0; /*nothing to do;*/ }
		};

#define i_object_impl(count_type)	\
		public:\
		virtual int32_t reserved(void*, void**)	\
		{	return not_implemented;	}		\
		virtual int32_t add_ref()	\
		{	return ++m_ref_count;	}		\
		virtual int32_t release()	\
		{	if ( --m_ref_count == 0 ) { delete this;return 0;} return m_ref_count; }	\
		private:\
		count_type	m_ref_count = { 0 };
	}
}