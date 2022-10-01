#ifndef __MYTYPES_H__
#define __MYTYPES_H__

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef long long long64;
typedef unsigned long long ulong64;

enum buf_type
{
	BT_UNKNOW = 0,
	BT_INT = 0x1,
	BT_UINT = 0x2,
	BT_SHORT = 0x3,
	BT_USHORT = 0x4,
	BT_CHAR = 0x5,
	BT_UCHAR = 0x6,
	BT_LONG = 0x7,
	BT_ULONG = 0x8,
	BT_STR = 0x9,
	BT_BIN = 0x10,
	BT_OBJ = 0x11,
	BT_DATETIME = 0x12,
	BT_VCHAR = 0x13,
	BT_LIST = 0x100,
	BT_QUEUE = 0x200,
	BT_COUNT = 0x400,
	BT_COUNT4 = 0x800,
	BT_INC = 0x1000
};

#define UM_STR			1
#define UM_ID_C			(1<<1)
#define UM_HOLD			(1<<2)
#define UM_SIZE			(1<<3)

#endif
