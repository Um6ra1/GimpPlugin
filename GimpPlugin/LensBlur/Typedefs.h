/**
 * @file		Typedefs.h
 * @brief Bitmap utility
 * @author	Um6r41
 * @date 2018/6/04
 */
 
 #pragma once

#define LOOP(max)	for(int i##__LINE__  = 0; i##__LINE__ < max; i##__LINE__ ++)	// DO NOT USE IN THE SAME LINE!!

#define REP(i, max)	for(int i = 0, i##Max = max; i < i##Max; i ++)
#define RREP(i, min, max)	for(int i = min, i##Max = max; i < i##Max; i ++)
#define EREP(i, min, max)	for(int i = min, i##Max = max; i <= i##Max; i ++)
#define REV(i, max)	for(int i = (max) - 1; i >= 0; i --)
#define RREV(i, min, max)	for(int i = max, i##Min = min; i > i##Min; i --)
#define EREV(i, min, max)	for(int i = (max) - 1, i##Min = min; i >= i##Min; i --)
#define SWAP(a, b) { auto temp##__LINE__ = a; a = b; b = temp##__LINE__; }
#define ALL(x) (x).begin(),(x).end()

#define NEXT_POW2(x)	( 1 << (int)(::ceil(::log2((double)(x)))) )
#define NORM255(x, max)	((u32)((x) * 255.0 / (max)))

#define EXTBYTE(x, bit)	(((x) >> bit) & 0xFF)
#define BYTES2RGB(r, g, b)	( ((u8)(b)) | (((u8)(g)) << 8) | (((u8)(r)) << 16) )
#define BYTE_OF_DWORD(dw, byte)	(((u8 *)&(dw))[byte])

#define CLIP(x, minX, maxX)	std::min(std::max(x, minX), maxX)
#define CHANNELS_RGB	3

typedef unsigned char 			u8;
typedef unsigned short	 		u16;
typedef unsigned int	 			u32;
typedef unsigned long long	u64;
typedef char 			s8;
typedef short 		s16;
typedef int				s32;
typedef long long	s64;

#ifndef _MSC_VER

#ifdef UNICODE
typedef wchar_t	TCHAR;
#else
typedef char	TCHAR;
#endif

typedef void        *LPVOID;
typedef TCHAR		*LPSTR;
typedef const TCHAR	*LPCSTR;
typedef LPVOID  HANDLE;

#else
//#include <Windows.h>
#endif	// _MSC_VER

typedef union _RGBA {
	u8 c[4];
	u8 a, r, g, b;
	u32 dw;
} RGBA;

#include <vector>
namespace ImgProc {
	struct Point {	int	x, y;	};
	struct Size {	int	width, height;	};
	
	struct	Image {
		std::vector<RGBA>	buf;
		int	width, height;

		inline void	SetSize(int width_, int height_)
		{	buf.resize( (width = width_) * (height = height_) );	}
		inline void SwapBuf(std::vector<RGBA> &buf2)
		{	buf.swap(buf2);	}

		inline Image()	{}
		inline Image(int width_, int height_)	{ SetSize(width_, height_); }
	};
}

