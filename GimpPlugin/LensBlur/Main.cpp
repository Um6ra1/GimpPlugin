
#include <Windows.h>
#include <cstdio>
#include "Typedefs.h"
#include "gimp.h"
#include <vector>

#pragma comment(lib, "user32.lib")

typedef PVOID (__cdecl *PFNFUNC)(...);

G_BEGIN_DECLS
	void	Query();
	void	Run(PCSTR psName, int params, const GimpParam *pSrc, int *pReturns, GimpParam **ppDst);
G_END_DECLS

const GimpPlugInInfo	PLUG_IN_INFO =
{
	NULL, NULL, Query, Run
};
PCSTR PLUGIN_NAME = "plug-in-LensBlur";

void	Msg(PCSTR psFmt, ...)
{
#define MSG_BUF_LEN	0x200
	va_list	ap;
	va_start(ap, psFmt);
	char	sMsg[MSG_BUF_LEN];
	::vsprintf(sMsg, psFmt, ap);
	va_end(ap);
	::MessageBoxA(NULL, sMsg, "DEBUG MSG", MB_OK | MB_ICONEXCLAMATION);
}

class Dll
{
	HMODULE	hModule_;
	bool	bAlreadyLoaded;
	
public:
	bool	Invalid()	{	return hModule_ == NULL;	}
	
	PFNFUNC	GetFunction(PCSTR psFuncName)
	{
		FARPROC	pfnProc;
		
		if( !(pfnProc = ::GetProcAddress(hModule_, psFuncName)) )
		{	::Msg("Not found such function: %s", psFuncName);	}
		return (PFNFUNC)pfnProc;
	}
	Dll(PCSTR psDllName) : bAlreadyLoaded(false)
	{
		if( (hModule_ = ::GetModuleHandle(psDllName)) )
		{
			//::Msg("Already loaded: %s", psDllName);
			bAlreadyLoaded = true;
			return;
		}
	
		if( !(hModule_ = ::LoadLibrary(psDllName)) )
		{	::Msg("Error loading: %s", psDllName);	}
	}
	~Dll()	{	if(!bAlreadyLoaded) ::FreeLibrary(hModule_);	}
};

//MAIN()
int __stdcall	WinMain(HINSTANCE hInst, HINSTANCE, char *psCmdLine, int nCmdShow)
{
	typedef int (*LPFNGIMPMAIN)(const GimpPlugInInfo *info, int argc, char *argv[]);

	// If PATH that includes these dlls is not high priority, loading dlls may fail! 
	Dll	libFFI("libffi-6.dll");	if(libFFI.Invalid())	return -1;
	Dll	libGIo("libgio-2.0-0.dll");	if(libGIo.Invalid())	return -1;
	Dll	libGLib("libglib-2.0-0.dll");	if(libGLib.Invalid())	return -1;
	Dll	libGMod("libgmodule-2.0-0.dll");	if(libGMod.Invalid())	return -1;
	Dll	libGObj("libgobject-2.0-0.dll");	if(libGObj.Invalid())	return -1;
	Dll	libIntl("libintl-8.dll");	if(libIntl.Invalid())	return -1;

	Dll	libGimpBase("libgimpbase-2.0-0.dll");	if(libGimpBase.Invalid())	return -1;
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return -1;
	
	LPFNGIMPMAIN	pfnGimpMain	=  (LPFNGIMPMAIN)libGimp.GetFunction("gimp_main");
	if(!pfnGimpMain)
	{	::MessageBox(0, "Error loading gimp_main", "ERROR", MB_OK);	return -1;	}	
	
	return (*pfnGimpMain)(&PLUG_IN_INFO, __argc, __argv);
}

void	Query()
{
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return;

	PFNFUNC pfnGimpInstallProcedure	= libGimp.GetFunction("gimp_install_procedure");
	PFNFUNC pfnGimpPluginMenuRegister	= libGimp.GetFunction("gimp_plugin_menu_register");
	
	static GimpParamDef	args[] =
	{
		{GIMP_PDB_INT32, "run-mode", "Run mode"},
		{GIMP_PDB_IMAGE, "image", "Input image"},
		{GIMP_PDB_DRAWABLE, "drawable", "Input drawable"},
	};
	//static GimpParamDef	results[] =
	(*pfnGimpInstallProcedure)
	(
		PLUGIN_NAME,
		"Blur the image",
		"Invert color.",
		"Um6ra1",
		"Copyright (C) Um6ra1",
		"2016",
		"_GaussianBlur",
		"RGB*, GRAY*",
		GIMP_PLUGIN,
		G_N_ELEMENTS(args), 0,
		args, NULL
	);

	int status = (int)(*pfnGimpPluginMenuRegister)(PLUGIN_NAME, "<Image>/Filters/VsNative-Plugin");
	if(!status)	::Msg("gimp_plugin_menu_register() failed!");
}

void	ImgProc(Dll &libGimp, GimpDrawable *pDrawable);

void	Run(PCSTR psName, int srcNum, const GimpParam *pSrc, int *pDstNum, GimpParam **ppDst)
{
	static GimpParam	values[1];
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return;

	PFNFUNC pfnGimpDrawableGet	= libGimp.GetFunction("gimp_drawable_get");
	PFNFUNC pfnGimpDrawableDetach = libGimp.GetFunction("gimp_drawable_detach");
	PFNFUNC pfnGimpDisplaysFlush = libGimp.GetFunction("gimp_displays_flush");
	PFNFUNC pfnGimpProgressInit = libGimp.GetFunction("gimp_progress_init");

	*pDstNum	= 1;
	*ppDst		= values;
	values[0].type	= GIMP_PDB_STATUS;
	values[0].data.d_status	= GIMP_PDB_SUCCESS;
	
	//if(values[0].data.d_int32 != GIMP_RUN_NONINTERACTIVE)	::Msg("Salve munde!");
	(*pfnGimpProgressInit)("Bluring...");

	GimpDrawable *pDrawable = (GimpDrawable *)(*pfnGimpDrawableGet)(pSrc[2].data.d_drawable);

	::ImgProc(libGimp, pDrawable);
	(*pfnGimpDisplaysFlush)();
	(*pfnGimpDrawableDetach)(pDrawable);
}

#define REP(i, max)	for(int i = 0, i##Max = max; i < i##Max; i ++)
#define RREP(i, min, max)	for(int i = min, i##Max = max; i < i##Max; i ++)
#define EREP(i, min, max)	for(int i = min, i##Max = max; i <= i##Max; i ++)
#define REV(i, max)	for(int i = (max) - 1; i >= 0; i --)
#define RREV(i, min, max)	for(int i = max, i##Min = min; i > i##Min; i --)
#define EREV(i, min, max)	for(int i = (max) - 1, i##Min = min; i >= i##Min; i --)

#define CLIP(x, minX, maxX)	min(max(x, minX), maxX)
#define CEILDIV(n, d)	(((n) + (d) - 1) / (d))	
#define INSB(dst, src, pos)	((dst) = (~(0xFF << (pos)) & (dst)) | ((src) << (pos)))
#define EXTB(src, pos)	(((src) & (0xFF << (pos))) >> (pos))
#define IN_RANGE(x, min, max)	((min) <= (x) && (x) < (max))
#define MERGE_RGB(r, g, b)	(((BYTE)(r) << 16) | ((BYTE)(g) << 8) | (BYTE)(b))

void LensBlur(BYTE *pDst, BYTE *pSrc, int width, int height, int channels);
void GaussBlur(BYTE *pDst, BYTE *pSrc, int width, int height, int channels);

void	ImgProc(Dll &libGimp, GimpDrawable *pDrawable)
{
	PFNFUNC	pfnGimpMaskBounds = libGimp.GetFunction("gimp_drawable_mask_bounds");
	PFNFUNC	pfnGimpDrawableBpp = libGimp.GetFunction("gimp_drawable_bpp");
	PFNFUNC	pfnGimpPixelRgnInit = libGimp.GetFunction("gimp_pixel_rgn_init");
	PFNFUNC	pfnGimpDrawableFlush = libGimp.GetFunction("gimp_drawable_flush");
	PFNFUNC	pfnGimpDrawableMergeShadow = libGimp.GetFunction("gimp_drawable_merge_shadow");
	PFNFUNC	pfnGimpDrawableUpdate = libGimp.GetFunction("gimp_drawable_update");
	PFNFUNC	pfnGimpPixelRgnGetRect = libGimp.GetFunction("gimp_pixel_rgn_get_rect");
	PFNFUNC	pfnGimpPixelRgnSetRect = libGimp.GetFunction("gimp_pixel_rgn_set_rect");
	PFNFUNC	pfnGimpTileCacheNTiles = libGimp.GetFunction("gimp_tile_cache_ntiles");
	PFNFUNC	pfnGimpTileWidth = libGimp.GetFunction("gimp_tile_width");

	int x1, y1, x2, y2;
	(*pfnGimpMaskBounds)(pDrawable->drawable_id, &x1, &y1, &x2, &y2);
	int	channels = (int)(*pfnGimpDrawableBpp)(pDrawable->drawable_id);
	int	width		= x2 - x1;
	int	height	= y2 - y1;
//	::Msg("w, h, c = %d, %d, %d", width, height, channels);
	//(*pfnGimpTileCacheNTiles)(CEILDIV(pDrawable->width, (int)(*pfnGimpTileWidth)()));
	GimpPixelRgn	rgnDst, rgnSrc;
	(*pfnGimpPixelRgnInit)(&rgnDst, pDrawable, x1, y1, width, height, true, true);
	(*pfnGimpPixelRgnInit)(&rgnSrc, pDrawable, x1, y1, width, height, false, false);

	std::vector<BYTE>	src(width * height * channels + 4);
	std::vector<BYTE>	dst(width * height * channels + 4);

	(*pfnGimpPixelRgnGetRect)(&rgnSrc, &src[0], x1, y1, width, height);
	
//	::Msg("%08X, %08X", *(UINT32 *)&src[0], *(UINT32 *)&src[4]);
	::GaussBlur(&dst[0], &src[0], width, height, channels);

	(*pfnGimpPixelRgnSetRect)(&rgnDst, &dst[0], x1, y1, width, height);

	(*pfnGimpDrawableFlush)(pDrawable);
	(*pfnGimpDrawableMergeShadow)(pDrawable->drawable_id, true);
	(*pfnGimpDrawableUpdate)(pDrawable->drawable_id, x1, y1, width, height);
}

#define EXTBYTE(x, bit)	(((x) >> bit) & 0xFF)
#define BYTES2RGB(r, g, b)	( ((BYTE)(b)) | (((BYTE)(g)) << 8) | (((BYTE)(r)) << 16) )
#define BYTE_OF_DWORD(dw, byte)	((BYTE *)&(dw))[byte]
#define CHANNELS_RGB	3

struct Point	{	int	x, y;	};

void LensBlur(BYTE *pDst, BYTE *pSrc, int width, int height, int channels)
{
	double	ra = 3;

	UINT	size = width * height;
	std::vector<double> r(size);
	std::vector<double> g(size);
	std::vector<double> b(size);
	double	factor = 10;

	REP(i, size)
	{
		UINT32	pixel = *(UINT32 *)&pSrc[channels * i];
		r[i] = ::exp(EXTBYTE(pixel, 0) * factor / 256.0);
		g[i] = ::exp(EXTBYTE(pixel, 8) * factor / 256.0);
		b[i] = ::exp(EXTBYTE(pixel, 16) * factor / 256.0);
	}

	int sr = ::ceil(2.57 * ra);
	std::vector<double>	kernel(2 * sr + 3);

	REP(i, sr + 1)	kernel[i] = ::exp(-(double)(i * i) / (2.0 * ra * ra));
	
	double	totalWeight = 0;
	EREP(yy, -sr, +sr) EREP(xx, -sr, +sr)	totalWeight += ::exp(-(double)(xx * xx + yy * yy) / (2.0 * ra * ra));

	REP(y, height) REP(x, width)
	{
		double	ch[CHANNELS_RGB] = {};
		EREP(yy, -sr, +sr) EREP(xx, -sr, +sr)
		{
			Point	pt = { CLIP(x + xx, 0, width - 1), CLIP(y + yy, 0, height - 1) };

			double	weight = ::exp(-(double)(xx * xx + yy * yy) / (2.0 * ra * ra));
			//double	weight = kernel[::abs(xx)] * kernel[::abs(yy)];
		//	REP(c, CHANNELS_RGB)	ch[c] += weight * (double)EXTBYTE(pixel, c * 8);
			ch[0]	+= weight * r[width * pt.y + pt.x];
			ch[1] += weight * g[width * pt.y + pt.x];
			ch[2] += weight * b[width * pt.y + pt.x];
		}
		UINT32	pixel = 0xFF000000;
	//	REP(c, CHANNELS_RGB)	pixel |= (BYTE)(ch[c] / totalWeight) << (c * 8);
		pixel |= (BYTE)(::log(ch[0] / totalWeight) * 256.0 / factor) << 0;
		pixel |= (BYTE)(::log(ch[1] / totalWeight) * 256.0 / factor) << 8;
		pixel |= (BYTE)(::log(ch[2] / totalWeight) * 256.0 / factor) << 16;
		*(UINT32 *)&pDst[channels * (width * y + x)] = pixel;
	}
}

void GaussBlur(BYTE *pDst, BYTE *pSrc, int width, int height, int channels)
{
	double	ra = 3;

	int sr = ::ceil(2.57 * ra);
	std::vector<double>	kernel(2 * sr + 3);

	REP(i, sr + 1)	kernel[i] = ::exp(-(double)(i * i) / (2.0 * ra * ra));
	
	double	totalWeight = 0;
	EREP(yy, -sr, +sr) EREP(xx, -sr, +sr)	totalWeight += ::exp(-(double)(xx * xx + yy * yy) / (2.0 * ra * ra));

	REP(y, height) REP(x, width)
	{
		double	ch[CHANNELS_RGB] = {};
		EREP(yy, -sr, +sr) EREP(xx, -sr, +sr)
		{
			Point	pt = { CLIP(x + xx, 0, width - 1), CLIP(y + yy, 0, height - 1) };
			//UINT32 pixel = *(UINT32 *)&pSrc[channels * (width * pt.y + pt.x)];
			BYTE *pPixel = &pSrc[channels * (width * y + x)];
			//double	weight = ::exp(-(double)(xx * xx + yy * yy) / (2.0 * ra * ra));
			double	weight = kernel[::abs(xx)] * kernel[::abs(yy)];
			REP(c, CHANNELS_RGB)	ch[c]	+= weight * (double)pPixel[c];//(double)EXTBYTE(pixel, c * 8);
		}
		BYTE *pPixel = &pDst[channels * (width * y + x)];
		REP(c, CHANNELS_RGB)	pPixel[c] = (BYTE)(ch[c] / totalWeight);
	}
}


