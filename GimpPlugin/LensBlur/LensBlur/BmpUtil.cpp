/**
 * @file		BmpUtil.cpp
 * @brief Bitmap utility
 * @author	Um6r41
 * @date 2018/6/04
 */
 
#define _CRT_SECURE_NO_WARNINGS 1
#include <cstring>
#include <cstdio>
#include <Windows.h>
#include "BmpUtil.h"

//#pragma comment(lib, "user32.lib")
//#pragma comment(lib, "gdi32.lib")

int FileSize(FILE *fp) {
	fpos_t cur, pos;
	fgetpos(fp, &cur);
	fseek(fp, 0, SEEK_END);
	fgetpos(fp, &pos);
	fseek(fp, cur, SEEK_SET);
	return pos;
}

int CBitmap::Load(LPCSTR pszFileName) {
	FILE	*fp = fopen(pszFileName, "rb");
	if(!fp) return -1;

	BitmapFileHeader header;
	fread(&header, sizeof(header), 1, fp);

	int dibSize = FileSize(fp) - sizeof(header);
	std::vector<u8> buf( dibSize );
	printf("dibsize=%d, filesize=%d, header=%d, buf=%d\n", dibSize, FileSize(fp), sizeof(header), buf.size());
	fread(&buf[0], buf.size(), 1, fp);
	fclose(fp);
	LoadFromBuf(&buf[header.bfOffBits-sizeof(header)], (BitmapInfoHeader *)&buf[0]);
	return 0;
}

int CBitmap::LoadFromBuf(u8 *pImg, BitmapInfoHeader *pBih) {
	//m_bmpInfoHeader = *(BITMAPINFOHEADER *)pBih;
	m_bmpInfoHeader = *pBih;
	
	int width = pBih->biWidth;
	int height	= (pBih->biHeight < 0) ? -pBih->biHeight : pBih->biHeight;
	
	m_image.SetSize(width, height);
	u32	*pBuf = (u32 *)&m_image.buf[0];
	int bytesPerPixel = pBih->biBitCount / 8;
	int bytesPerLine = ( (width + (m_bmpInfoHeader.biBitCount % 32 != 0) ) * bytesPerPixel >> 2) << 2;	// ceil(.../4)*4, this value must be divisible by 4!
		
	printf("(size, w, h, p, bc, cp, si, x, y, cu, ci)=(%x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x)\n", pBih->biSize,
		pBih->biWidth, pBih->biHeight, pBih->biPlanes, pBih->biBitCount, pBih->biCompression, pBih->biSizeImage, pBih->biXPelsPerMeter, pBih->biYPelsPerMeter, pBih->biClrUsed, pBih->biClrImportant);
	switch(pBih->biBitCount) {
		case 1: {
			m_palettes.resize(2);
		} break;
		
		case 4: {
			m_palettes.resize(16);
		} break;
		
		case 16:
		case 32: {
			puts("32bit");
			m_bmpInfoHeader.biCompression = BI_RGB;
			m_bmpInfoHeader.biSize = 40;
			u32 mask =  0xFFFFFFFF;
			if(pBih->biCompression == BI_BITFIELDS) {
				puts("Masked");
				u32 *pMasks = (u32 *)(pBih+1);
				mask =  pMasks[0] | pMasks[1] | pMasks[2] | pMasks[3];
			}
			if(pBih->biHeight < 0) // upper to lower
				REP(y, height) REV(x, width)
					pBuf[width*y+x] = mask & *(u32 *)&pImg[(width*y+x)*4];

			else // lower to upper 
				REV(y, height) REP(x, width)
					pBuf[width*y+x] = mask & *(u32 *)&pImg[(width*y+x)*4];
		} break;
		
		case 8: {
			puts("8bit");
			m_bmpInfoHeader.biBitCount = 24;
			u32 *pPalettes = (u32 *)( (u8 *)pBih + pBih->biSize );

			if(pBih->biHeight < 0) // start writing upper to lower
				REP(y, height)	REP(x, width) pBuf[width*y+x] = pPalettes[ pImg[bytesPerLine*y + x] ];
			else // start writing lower to upper 
				REV(y, height)	REP(x, width) pBuf[width*y+x] = pPalettes[ pImg[bytesPerLine*y + x] ];
		} break;
		
		case 24: {
			puts("24bit");
			if(pBih->biHeight < 0) // upper to lower
				REP(y, height) REP(x, width)
					pBuf[width*y+x] = 0x00FFFFFF & *(u32 *)&pImg[bytesPerLine*y + 3*x];

			else // lower to upper 
				REV(y, height) REP(x, width)
					pBuf[width*y+x] = 0x00FFFFFF & *(u32 *)&pImg[bytesPerLine*y + 3*x];
		} break;
	}
	
	return 0;
}

int CBitmap::Save(LPCSTR pszFileName) {
	int width	= m_image.width;
	int height	= m_image.height;
	int bytesPerPixel = m_bmpInfoHeader.biBitCount / 8;
	int bytesPerLine = ( (width + (m_bmpInfoHeader.biBitCount % 32 != 0) ) * bytesPerPixel >> 2) << 2;
	int writeSize = bytesPerLine * height;
	
	BitmapFileHeader header = {};
	header.bfType	= 0x4D42;
	header.bfSize	= sizeof(BitmapFileHeader) + m_bmpInfoHeader.biSize + writeSize;
	header.bfOffBits	= sizeof(BitmapFileHeader) + m_bmpInfoHeader.biSize;
	
	//m_bmpInfoHeader.biBitCount = 24;
	m_bmpInfoHeader.biClrUsed = 0;
	m_bmpInfoHeader.biClrImportant = 0;
	m_bmpInfoHeader.biSizeImage = bytesPerLine*height;
	
	FILE	*fp = fopen(pszFileName, "wb");
	if(!fp) return -1;
	fwrite(&header, sizeof(header), 1, fp);
	fwrite(&m_bmpInfoHeader, m_bmpInfoHeader.biSize, 1, fp);
    
	u32 *pBuf = (u32 *)&m_image.buf[0];
	
	std::vector<BYTE>	imgData(writeSize + 4);
	printf("SaveSize:%d, %d\n", width, height);

	switch(m_bmpInfoHeader.biBitCount) {
		case 24: {
			if(m_bmpInfoHeader.biHeight < 0) // Top to bottom
				REP(y, height)	REP(x, width)
					*(u32 *)&imgData[bytesPerLine*y + 3*x] = pBuf[width*y+x];
			else // Bottom to top
				REV(y, height)	REP(x, width)
					*(u32 *)&imgData[bytesPerLine*y + 3*x] = pBuf[width*y+x];
		} break;
		
		case 32: {
			if(m_bmpInfoHeader.biHeight < 0) // Top to bottom
				REP(y, height)	REP(x, width)
					*(u32 *)&imgData[(width*y+x)*4] = pBuf[y * width + x];
			else // Bottom to top
				REV(y, height)	REP(x, width)
					*(u32 *)&imgData[(width*y+x)*4] = pBuf[y * width + x];
		} break;
	}
	fwrite(&imgData[0], writeSize, 1, fp);
	fclose(fp);
	printf("Saved!\n", width, height);
	
	return 0;
}

// Ref: http://denki.nara-edu.ac.jp/~yabu/edu/jyoho/code/c++builder6/clipboard/Unit1.cpp
int CBitmap::GetClipboard() {
	int bAvaliBmp = IsClipboardFormatAvailable(CF_BITMAP);
	int bAvaliDib = IsClipboardFormatAvailable(CF_DIB);
	int bAvaliDibV5 = IsClipboardFormatAvailable(CF_DIBV5);
	
	if(bAvaliBmp || bAvaliDib || bAvaliDibV5) {
		if( !OpenClipboard(NULL) ) return -1;
		HANDLE hData = GetClipboardData(CF_DIBV5);
		if(!hData) return -1;//hData = GetClipboardData(CF_DIB);
		CloseClipboard();
		if(!hData || hData == INVALID_HANDLE_VALUE) return -1;
		
		LPBITMAPINFO pBi = (LPBITMAPINFO)GlobalLock(hData);
		if(!pBi) return -1;
		BITMAPV5HEADER *pBv5 = (BITMAPV5HEADER *)&pBi->bmiHeader;
		
		int offset = pBv5->bV5Size + pBv5->bV5ClrUsed * (pBv5->bV5BitCount > 24 ? sizeof(RGBQuad) : sizeof(RGBTriple));
		if (pBv5->bV5Compression == BI_BITFIELDS) offset += 12;	// mask part 3 * (r,g,b,0)
		u8 *pBuf = (u8 *)pBi + offset; //sizeof(BITMAPV5HEADER); // Must be used: gmi.biSize !!!

		fprintf(stderr, "Size: %d * %d, bit=%d, size=%d, biCompres=%d\n",
			pBv5->bV5Width, pBv5->bV5Height,
			pBv5->bV5BitCount, pBv5->bV5SizeImage,pBv5->bV5Compression);
		LoadFromBuf(pBuf, (BitmapInfoHeader *)&pBi->bmiHeader);
		GlobalUnlock(hData);

		return 0;
	}
	return -1;
}

int CBitmap::SetClipboard() {
	HANDLE hGMem = GlobalAlloc(GHND, m_bmpInfoHeader.biSize + m_image.buf.size());
	BITMAPINFOHEADER *pBih = (BITMAPINFOHEADER *)GlobalLock(hGMem);
	*pBih = *(BITMAPINFOHEADER *)&m_bmpInfoHeader;
	//pBih->biBitCount = 32;
	//pBih->biCompression = BI_RGB;
	
	int width	= m_image.width;
	int height	= m_image.height;
	int bytesPerPixel = m_bmpInfoHeader.biBitCount / 8;
	int bytesPerLine = ( (width + (m_bmpInfoHeader.biBitCount % 32 != 0) ) * bytesPerPixel >> 2) << 2;
//	int writeSize = bytesPerLine * height;
	
	printf("Setting... w=%d, h=%d", width, height);
	int offset = pBih->biSize + pBih->biClrUsed * (pBih->biBitCount > 24 ? sizeof(RGBQuad) : sizeof(RGBTriple));
	u8 *imgData = (u8 *)pBih + offset;
	u32 *pBuf = (u32 *)&m_image.buf[0];
	
	switch(m_bmpInfoHeader.biBitCount) {
		case 24: {
			if(m_bmpInfoHeader.biHeight < 0) // Top to bottom
				REP(y, height)	REP(x, width) *(RGBTriple *)&imgData[bytesPerLine*y + 3*x] = *(RGBTriple *)&pBuf[width*y+x];
			else // Bottom to top
				REV(y, height)	REP(x, width) *(RGBTriple *)&imgData[bytesPerLine*y + 3*x] = *(RGBTriple *)&pBuf[width*y+x];
		} break;
		
		case 32: {
			if(m_bmpInfoHeader.biHeight < 0) // Top to bottom
				REP(y, height)	REP(x, width) *(u32 *)&imgData[(width*y+x)*4] = pBuf[width*y+x];
			else // Bottom to top
				REV(y, height)	REP(x, width) *(u32 *)&imgData[(width*y+x)*4] = pBuf[width*y+x];
		} break;
	}
	GlobalUnlock(hGMem);
	
	if( !OpenClipboard(NULL) ) return -1;
	EmptyClipboard();
	SetClipboardData(CF_DIB, hGMem);
	CloseClipboard();
	
	puts("OK");
	return 0;
}

void CBitmap::Delete() {
	{ std::vector<RGBA>	arr;	m_image.buf.swap(arr); }
	{ std::vector<RGBQuad>	arr;	m_palettes.swap(arr); }
}

int CBitmap::Reserve(int width, int height, int bitcount) {
	Delete();
	
	m_image.SetSize(width, height);

	m_bmpInfoHeader.biSize	= 40;
	m_bmpInfoHeader.biWidth	= width;
	m_bmpInfoHeader.biHeight	= height;
	m_bmpInfoHeader.biPlanes	= 1;
	m_bmpInfoHeader.biBitCount	= bitcount;
	m_bmpInfoHeader.biCompression	= 0;
	m_bmpInfoHeader.biSizeImage	= 0;
	m_bmpInfoHeader.biXPelsPerMeter	= 2834;
	m_bmpInfoHeader.biYPelsPerMeter	= 2834;
	m_bmpInfoHeader.biClrUsed	= 0;
	m_bmpInfoHeader.biClrImportant	= 0;
	
	return 0;
}

void CBitmap::SetSize(int width, int height) {
	int bytesPerPixel	= m_bmpInfoHeader.biBitCount >> 3;
	
	m_image.SetSize(width, height);
	m_bmpInfoHeader.biWidth = width;
	m_bmpInfoHeader.biHeight = height;
}

CBitmap& CBitmap::operator =(const CBitmap &o) {
	Delete();

	m_bmpInfoHeader	= o.m_bmpInfoHeader;
	m_image = o.m_image;
    
	return *this;
}
