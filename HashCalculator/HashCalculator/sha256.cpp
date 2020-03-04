/*************************** HEADER FILES ***************************/
#include "stdafx.h"
#include "sha256.h"
#include <stdlib.h>
#include <cstring>
#include <sstream>
#include <iomanip>

using namespace utils::crypto;


/*********************** EXTERN INTERFACE ***********************/
int sha256_encode(const char* in_buf, const int inbuf_len, char* out_buf, int& outbuf_len)
{
	SHA256 sha256;
	return sha256.generate(in_buf, inbuf_len, out_buf, outbuf_len);
}

/*********************** CLASS MEMBER FUNCTIONS ***********************/
void SHA256::sha256_transform(SHA256_CTX *ctx, const utils::crypto::BYTE data[])
{
	UINT a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

	for (i = 0, j = 0; i < 16; ++i, j += 4)
		m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
	for (; i < 64; ++i)
		m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];
	f = ctx->state[5];
	g = ctx->state[6];
	h = ctx->state[7];

	for (i = 0; i < 64; ++i)
	{
		t1 = h + EP1(e) + CH(e, f, g) + K[i] + m[i];
		t2 = EP0(a) + MAJ(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
	ctx->state[5] += f;
	ctx->state[6] += g;
	ctx->state[7] += h;
}

void SHA256::factor_init()
{
	K[0] = 0x428a2f98; K[1] = 0x71374491; K[2] = 0xb5c0fbcf; K[3] = 0xe9b5dba5;
	K[4] = 0x3956c25b; K[5] = 0x59f111f1; K[6] = 0x923f82a4; K[7] = 0xab1c5ed5;
	K[8] = 0xd807aa98; K[9] = 0x12835b01; K[10] = 0x243185be; K[11] = 0x550c7dc3;
	K[12] = 0x72be5d74; K[13] = 0x80deb1fe; K[14] = 0x9bdc06a7; K[15] = 0xc19bf174;
	K[16] = 0xe49b69c1; K[17] = 0xefbe4786; K[18] = 0x0fc19dc6; K[19] = 0x240ca1cc;
	K[20] = 0x2de92c6f; K[21] = 0x4a7484aa; K[22] = 0x5cb0a9dc; K[23] = 0x76f988da;
	K[24] = 0x983e5152; K[25] = 0xa831c66d; K[26] = 0xb00327c8; K[27] = 0xbf597fc7;
	K[28] = 0xc6e00bf3; K[29] = 0xd5a79147; K[30] = 0x06ca6351; K[31] = 0x14292967;
	K[32] = 0x27b70a85; K[33] = 0x2e1b2138; K[34] = 0x4d2c6dfc; K[35] = 0x53380d13;
	K[36] = 0x650a7354; K[37] = 0x766a0abb; K[38] = 0x81c2c92e; K[39] = 0x92722c85;
	K[40] = 0xa2bfe8a1; K[41] = 0xa81a664b; K[42] = 0xc24b8b70; K[43] = 0xc76c51a3;
	K[44] = 0xd192e819; K[45] = 0xd6990624; K[46] = 0xf40e3585; K[47] = 0x106aa070;
	K[48] = 0x19a4c116; K[49] = 0x1e376c08; K[50] = 0x2748774c; K[51] = 0x34b0bcb5;
	K[52] = 0x391c0cb3; K[53] = 0x4ed8aa4a; K[54] = 0x5b9cca4f; K[55] = 0x682e6ff3;
	K[56] = 0x748f82ee; K[57] = 0x78a5636f; K[58] = 0x84c87814; K[59] = 0x8cc70208;
	K[60] = 0x90befffa; K[61] = 0xa4506ceb; K[62] = 0xbef9a3f7; K[63] = 0xc67178f2;
}

void SHA256::sha256_init(SHA256_CTX *ctx)
{
	ctx->datalen = 0;
	ctx->bitlen = 0;
	ctx->state[0] = 0x6a09e667;
	ctx->state[1] = 0xbb67ae85;
	ctx->state[2] = 0x3c6ef372;
	ctx->state[3] = 0xa54ff53a;
	ctx->state[4] = 0x510e527f;
	ctx->state[5] = 0x9b05688c;
	ctx->state[6] = 0x1f83d9ab;
	ctx->state[7] = 0x5be0cd19;
}

void SHA256::sha256_update(SHA256_CTX *ctx, const utils::crypto::BYTE data[], size_t len)
{
	UINT i;

	for (i = 0; i < len; ++i)
	{
		ctx->data[ctx->datalen] = data[i];
		ctx->datalen++;
		if (ctx->datalen == 64)
		{
			sha256_transform(ctx, ctx->data);
			ctx->bitlen += 512;
			ctx->datalen = 0;
		}
	}
}

void SHA256::sha256_final(SHA256_CTX *ctx, utils::crypto::BYTE hash[])
{
	UINT i;

	i = ctx->datalen;

	//pad whatever data is left in the buffer.
	if (ctx->datalen < 56)
	{
		ctx->data[i++] = 0x80;
		while (i < 56)
			ctx->data[i++] = 0x00;
	}
	else
	{
		ctx->data[i++] = 0x80;
		while (i < 64)
			ctx->data[i++] = 0x00;
		sha256_transform(ctx, ctx->data);
		memset(ctx->data, 0, 56);
	}

	//append to the padding the total message's length in bits and transform.
	ctx->bitlen += ctx->datalen * 8;
	ctx->data[63] = static_cast<BYTE>(ctx->bitlen);
	ctx->data[62] = static_cast<BYTE>(ctx->bitlen >> 8);
	ctx->data[61] = static_cast<BYTE>(ctx->bitlen >> 16);
	ctx->data[60] = static_cast<BYTE>(ctx->bitlen >> 24);
	ctx->data[59] = static_cast<BYTE>(ctx->bitlen >> 32);
	ctx->data[58] = static_cast<BYTE>(ctx->bitlen >> 40);
	ctx->data[57] = static_cast<BYTE>(ctx->bitlen >> 48);
	ctx->data[56] = static_cast<BYTE>(ctx->bitlen >> 56);
	sha256_transform(ctx, ctx->data);

	//since this implementation uses little endian byte ordering and SHA uses big endian,
	//reverse all the bytes when copying the final state to the output hash.
	for (i = 0; i < 4; ++i)
	{
		hash[i] = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 4] = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 8] = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
	}
}

std::string SHA256::hexdump(const utils::crypto::BYTE *buf, const int num)
{
	std::stringstream dump;
	dump << std::hex;
	for (int i = 0; i < num; i++)
		dump << std::setfill('0') << std::setw(2) <<(unsigned int)buf[i];
	return dump.str();
}

int SHA256::generate(const char *in_buf, const int inbuf_len, char *out_buf, int &outbuf_len)
{
	//check input param
	if ( (nullptr == in_buf) || (inbuf_len <= 0) )
	{
		//log_error(utils, "input parameter invalid.");
		return -1;
	}

	if ((nullptr == out_buf) || (outbuf_len < SHA256_OUTPUT_LENGTH))
	{
		//log_error(utils, "output parameter invalid.");
		return -2;
	}

	//copy input chars
	BYTE *in_byte = new (std::nothrow) BYTE[inbuf_len + 1];
	if ( nullptr == in_byte )
	{
		//log_error(utils, "allocate memory failed.");
		return -3;
	}

	memcpy(in_byte, in_buf, inbuf_len);

	//sha256 encode
	BYTE buf[SHA256_BLOCK_SIZE];
	sha256_init(&ctx);
	sha256_update(&ctx, in_byte, inbuf_len);
	sha256_final(&ctx, buf);

	// dump sha256 message digest to string
	std::string out_str = hexdump(buf, SHA256_BLOCK_SIZE);
	if (out_str.empty())
	{
		//log_error(utils, "hexdump(...) error.");
		return -4;
	}

	outbuf_len = out_str.length();
	memset(out_buf, 0, SHA256_OUTPUT_LENGTH);
	memcpy(out_buf, out_str.c_str(), outbuf_len);

	//when used complete, release data
	if (in_byte)
	{
		delete[] in_byte;
		in_byte = nullptr;
	}

	//return value: 0 means success
	return 0;
}
