/**
 * @file		BmpUtil.h
 * @brief Bitmap utility
 * @author	Um6r41
 * @date 2018/6/04
 */
 
#pragma once

#include <vector>
#include "Typedefs.h"

#if !defined(_WIN32_WINNT ) && !defined(_MSC_VER )
#endif	// _MSC_VER
typedef u32	Fxpt2Dot30;
#pragma pack(push,1)
typedef struct {
	Fxpt2Dot30	ciexyzX;
	Fxpt2Dot30	ciexyzY;
	Fxpt2Dot30	ciexyzZ;
} CieXyz;
typedef struct {
	CieXyz	ciexyzRed;
	CieXyz	ciexyzGreen;
	CieXyz	ciexyzBlue;
} CieXyzTriple;

typedef struct {
    u16 bfType;		// =="BM"==0x424D
    u32 bfSize;		// entire size of this file[bytes]
    u16 bfReserved1;	// ==0
    u16 bfReserved2;	// ==0
    u32 bfOffBits;		// Offset of pixel bit
} BitmapFileHeader;

typedef union {
	// OS/2
	struct {
		u32 bcSize;
		s16 bcWidth;
		s16 bcHeight;
		u16 bcPlanes;
		u16 bcBitCount;
	};

	// Windows
	struct {
		u32	biSize;		// Structure size(40:windows, 12:OS/2)
		s32	biWidth;		// Image width
		s32	biHeight;		// Image height
		u16	biPlanes;		// ==1
		u16	biBitCount;	// Color bit count (1, 4, 8, 16, 24, 32)
		u32	biCompression;	// BI_RGB, BI_RLE4, BI_RLE8, BI_BITFIELDS
		u32	biSizeImage;		// Bytes of image
		s32	biXPelsPerMeter;	// Horizontal resolution
		s32	biYPelsPerMeter;	// Vertical resolution
		u32	biClrUsed;			// Color table count
		u32	biClrImportant;		// Important color count
	};

	// Ver 4(BITMAPV4HEADER)
	struct {
		u32	bV4Size;
		s32	bV4Width;
		s32	bV4Height;	
		u16	bV4Planes;
		u16	bV4BitCount;
		u32  bV4Compression;	
		u32  bV4SizeImage;
		s32   bV4XPelsPerMeter;
		s32   bV4YPelsPerMeter;
		u32  bV4ClrUsed;
		u32  bV4ClrImportant;

		u32	bV4RedMask;	// Masks
		u32	bV4GreenMask;
		u32	bV4BlueMask;
		u32	bV4AlphaMask;
		
		u32	bV4CSType;	// Color domain type
		CieXyzTriple	bV4Endpoints;	// CIEXYZ(bV4CSType == 0)
		
		u32	bV4GammaRed;	// Gamma values
		u32	bV4GammaGreen;
		u32	bV4GammaBlue;
	};

	// Ver 5(BITMAPV5HEADER)
	struct {
		u32	bV5Size;
		s32	bV5Width;
		s32	bV5Height;	
		u16	bV5Planes;
		u16	bV5BitCount;
		u32  bV5Compression;	
		u32  bV5SizeImage;
		s32	bV5XPelsPerMeter;
		s32 	bV5YPelsPerMeter;
		u32  bV5ClrUsed;
		u32  bV5ClrImportant;

		u32	bV5RedMask;	// Masks
		u32	bV5GreenMask;
		u32	bV5BlueMask;
		u32	bV5AlphaMask;
		
		u32	bV5CSType;	// Color domain type
		CieXyzTriple bV5Endpoints;	// CIEXYZ(bV4CSType == 0)
		
		u32	bV5GammaRed;	// Gamma values
		u32	bV5GammaGreen;
		u32	bV5GammaBlue;

		u32	bV5Intent;			// sRGB color domain type
		u32	bV5ProfileData;	// Color domain profile offset
		u32	bV5ProfileSize;	// Color domain profile size
		u32	bV5Reserved;
	};
} BitmapInfoHeader;

/*================
	Color palette
 ================*/
typedef struct {
	u8	rgbtBlue;
	u8	rgbtGreen;
	u8	rgbtRed;
} RGBTriple;

typedef struct {
	u8	rgbBlue;
	u8	rgbGreen;
	u8	rgbRed;
	u8	rgbReserved;
} RGBQuad;
#pragma pack(pop)
class CBitmap {
	BitmapInfoHeader m_bmpInfoHeader;
	//BITMAPINFOHEADER m_bmpInfoHeader;
	std::vector<RGBQuad> m_palettes;
	ImgProc::Image m_image;

public:
	int Load(LPCSTR lpszFileName);
	int LoadFromBuf(u8 *pImg, BitmapInfoHeader *pBi);
	int Save(LPCSTR lpszFileName);
	int GetClipboard();
	int SetClipboard();
	
	void Test(){
		Reserve(100, 100);
		
		REP(y, 100) REP(x, 100)
			*(u32*)&m_image.buf[100*y+x] = x < 25 ? 0x000000 :
			x < 50 ? 0x0000FF :
			x < 75 ? 0x00FF00 :
			0xFF0000;
	}
	
	void	Delete();
	int	Reserve(int width, int height, int bitcount = 24);
	void	SetSize(int width, int height);
	
	ImgProc::Image &Image() { return m_image; }
	RGBA *Buff() { return &m_image.buf[0]; }
	u32	BuffSize() { return m_image.width * m_image.height * 4; }
	int Width() { return m_image.width; }
	int Height() { return m_image.height; }

	CBitmap& operator =(const CBitmap &o);
	CBitmap()	{}
	~CBitmap()	{}
};
