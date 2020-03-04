/***************************************************************************************************
*Copyright(C): 2020
*FileName    : sha256.h
*Author      : wanglei_gmgc@163.com
*Version     : 1.0
*Date        : 2020-03-04
*Desc        : SHA256 class can generate string to sha256 message digest.
*            : [warning]: it only support ASCII, if use other coded format such as utf-8,
*            : the result will be incorrect.
*History     : Add by Lei.Wang 2020-03-04
***************************************************************************************************/
#pragma once

/*************************** HEADER FILES ***************************/
#include <stddef.h>
#include <string>

namespace utils
{
    namespace crypto
	{
		/****************************** MACROS ******************************/
		//sha256 outputs a 32 byte digest
		#define SHA256_BLOCK_SIZE		32
		#define SHA256_OUTPUT_LENGTH	(32 * 2 + 1)

		//algorithm functions define
		#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
		#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))
		#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
		#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
		#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
		#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
		#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
		#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

		/**************************** DATA TYPES ****************************/
		typedef unsigned char BYTE;	//8-bit byte
		typedef unsigned int  UINT;	//32-bit word, change to "long" for 16-bit machines

		typedef struct s_sha256
		{
			BYTE data[64];
			UINT datalen;
			unsigned long long bitlen;
			UINT state[8];
		} SHA256_CTX;

		/*********************** CLASS DECLARATIONS **********************/
		//sha256 class
		class SHA256
		{
		public:
			SHA256() { factor_init(); }
			~SHA256() {}

		public:
			//encode string to sha256 message digest
			int generate (const char *in_buf, const int inbuf_len,
				char *out_buf, int &outbuf_len);

		private:
			void sha256_transform(SHA256_CTX *ctx, const BYTE data[]);
			void factor_init();
			void sha256_init(SHA256_CTX *ctx);
			void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len);
			void sha256_final(SHA256_CTX *ctx, BYTE hash[]);
			std::string hexdump(const BYTE *buf, const int num);

		private:
			UINT K[64];
			SHA256_CTX ctx;
		};
	}
}
