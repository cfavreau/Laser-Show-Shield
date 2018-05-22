#include <windows.h>

#ifndef __MAMBA_STRUCTURES_H__
#define __MAMBA_STRUCTURES_H__

#pragma pack(1)

struct MAMBA_POINT
{
	WORD x;
	WORD y;
	WORD z;

	BYTE r;
	BYTE g;
	BYTE b;

	BYTE reserved1;
	BYTE reserved2;
	BYTE reserved3;

	BYTE intensity;
	BYTE reserved4;
	BYTE repeatpoint;
};

#pragma pack(1)

struct MAMBA_PARAMS
{
	WORD size;
	DWORD point_rate;
	bool invert_blanking;
};


#endif	// __MAMBA_STRUCTURES_H__