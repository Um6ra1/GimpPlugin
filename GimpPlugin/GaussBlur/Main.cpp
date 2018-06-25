#define _USE_MATH_DEFINES
#include <Windows.h>
#include <cstdio>
#include "../GimpCommon.h"
#include "../Dll.h"
#include <vector>

#define REP(i, max)	for(int i = 0, i##Max = max; i < i##Max; i ++)
#define RREP(i, min, max)	for(int i = min, i##Max = max; i < i##Max; i ++)
#define EREP(i, min, max)	for(int i = min, i##Max = max; i <= i##Max; i ++)
#define REV(i, max)	for(int i = (max) - 1; i >= 0; i --)
#define RREV(i, min, max)	for(int i = max, i##Min = min; i > i##Min; i --)
#define EREV(i, min, max)	for(int i = (max) - 1, i##Min = min; i >= i##Min; i --)

void	Query();
void	Run(PCSTR psName, int params, const GimpParam *pSrc, int *pReturns, GimpParam **ppDst);
const GimpPlugInInfo	PLUG_IN_INFO = { NULL, NULL, Query, Run };

int WINAPI	WinMain(HINSTANCE hInst, HINSTANCE, char *psCmdLine, int nCmdShow) {
	Dll libFFI("libffi-6.dll"); if(libFFI.Invalid()) return -1;
	Dll libGIo("libgio-2.0-0.dll"); if(libGIo.Invalid()) return -1;
	Dll libGLib("libglib-2.0-0.dll"); if(libGLib.Invalid()) return -1;
	Dll libGMod("libgmodule-2.0-0.dll"); if(libGMod.Invalid()) return -1;
	Dll libGObj("libgobject-2.0-0.dll"); if(libGObj.Invalid()) return -1;
	Dll libIntl("libintl-8.dll"); if(libIntl.Invalid()) return -1;

	Dll libGimpBase("libgimpbase-2.0-0.dll"); if(libGimpBase.Invalid()) return -1;
	Dll libGimp("libgimp-2.0-0.dll"); if(libGimp.Invalid()) return -1;
	
	Func pfnGimpMain = libGimp.GetFunction("gimp_main");
	
	return (int)pfnGimpMain(&PLUG_IN_INFO, __argc, __argv);
}

PCSTR PLUGIN_NAME = "plug-in-GaussBlur";
PCSTR PLUGIN_BLURB	= "Gaussian blur";
PCSTR PLUGIN_HELP = "Apply Gaussian blur.";
PCSTR PLUGIN_AUTHOR = "Um6ra1";
PCSTR PLUGIN_COPYRIGHT = "Copyright (C) Um6ra1";
PCSTR PLUGIN_DATE = "2017";
PCSTR PLUGIN_MENULABEL = "_GaussBlur";
PCSTR PLUGIN_IMGTYPES = "RGB*, GRAY*";
PCSTR PLUGIN_MENUPATH = "<Image>/Filters/VsNative-Plugin";

void	Query() {
	Dll libGimp("libgimp-2.0-0.dll"); if(libGimp.Invalid()) return;

	Func pfnGimpInstallProcedure	= libGimp.GetFunction("gimp_install_procedure");
	Func pfnGimpPluginMenuRegister	= libGimp.GetFunction("gimp_plugin_menu_register");
	
	static GimpParamDef	args[] = {
		{GIMP_PDB_INT32, "run-mode", "Run mode"},
		{GIMP_PDB_IMAGE, "image", "Input image"},
		{GIMP_PDB_DRAWABLE, "drawable", "Input drawable"},
	};
	//static GimpParamDef	results[] =
	pfnGimpInstallProcedure (
		PLUGIN_NAME,	PLUGIN_BLURB,
		PLUGIN_HELP,		PLUGIN_AUTHOR,
		PLUGIN_COPYRIGHT,		PLUGIN_DATE,
		PLUGIN_MENULABEL,		PLUGIN_IMGTYPES,
		GIMP_PLUGIN,
		G_N_ELEMENTS(args), 0,
		args, NULL
	);

	int status = (int)pfnGimpPluginMenuRegister(PLUGIN_NAME, PLUGIN_MENUPATH);
	if(!status) Msg("gimp_plugin_menu_register() failed!");
}

void GaussBlur(BYTE *pDst, BYTE *pSrc, int width, int height, int channels, int r);
void	ImgProc(Dll &libGimp, GimpDrawable *pDrawable);

void	Run(PCSTR psName, int srcNum, const GimpParam *pSrc, int *pDstNum, GimpParam **ppDst) {
	static GimpParam	values[1];
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return;

	Func pfnGimpDrawableGet	= libGimp.GetFunction("gimp_drawable_get");
	Func pfnGimpDrawableDetach = libGimp.GetFunction("gimp_drawable_detach");
	Func pfnGimpDisplaysFlush = libGimp.GetFunction("gimp_displays_flush");
	Func pfnGimpProgressInit = libGimp.GetFunction("gimp_progress_init");

	*pDstNum = 1;
	*ppDst = values;
	values[0].type = GIMP_PDB_STATUS;
	values[0].data.d_status = GIMP_PDB_SUCCESS;
	
	pfnGimpProgressInit("Bluring...");

	GimpDrawable *pDrawable = (GimpDrawable *)pfnGimpDrawableGet(pSrc[2].data.d_drawable);

	ImgProc(libGimp, pDrawable);	// Gaussian blur
	pfnGimpDisplaysFlush();
	pfnGimpDrawableDetach(pDrawable);
}

void	ImgProc(Dll &libGimp, GimpDrawable *pDrawable) {
	Func pfnGimpMaskBounds = libGimp.GetFunction("gimp_drawable_mask_bounds");
	Func pfnGimpDrawableBpp = libGimp.GetFunction("gimp_drawable_bpp");
	Func pfnGimpPixelRgnInit = libGimp.GetFunction("gimp_pixel_rgn_init");
	Func pfnGimpDrawableFlush = libGimp.GetFunction("gimp_drawable_flush");
	Func pfnGimpDrawableMergeShadow = libGimp.GetFunction("gimp_drawable_merge_shadow");
	Func pfnGimpDrawableUpdate = libGimp.GetFunction("gimp_drawable_update");
	Func pfnGimpPixelRgnGetRect = libGimp.GetFunction("gimp_pixel_rgn_get_rect");
	Func pfnGimpPixelRgnSetRect = libGimp.GetFunction("gimp_pixel_rgn_set_rect");
	Func pfnGimpTileCacheNTiles = libGimp.GetFunction("gimp_tile_cache_ntiles");
	Func pfnGimpTileWidth = libGimp.GetFunction("gimp_tile_width");

	int x1, y1, x2, y2;
	pfnGimpMaskBounds(pDrawable->drawable_id, &x1, &y1, &x2, &y2);
	int channels = (int)pfnGimpDrawableBpp(pDrawable->drawable_id);
	int width = x2 - x1;
	int height = y2 - y1;
	
	GimpPixelRgn rgnDst, rgnSrc;
	pfnGimpPixelRgnInit(&rgnDst, pDrawable, x1, y1, width, height, true, true);
	pfnGimpPixelRgnInit(&rgnSrc, pDrawable, x1, y1, width, height, false, false);

	std::vector<BYTE> src(width * height * channels + 4);
	std::vector<BYTE> dst(width * height * channels + 4);

	pfnGimpPixelRgnGetRect(&rgnSrc, &src[0], x1, y1, width, height);
	GaussBlur(&dst[0], &src[0], width, height, channels, 4);

	pfnGimpPixelRgnSetRect(&rgnDst, &dst[0], x1, y1, width, height);

	pfnGimpDrawableFlush(pDrawable);
	pfnGimpDrawableMergeShadow(pDrawable->drawable_id, true);
	pfnGimpDrawableUpdate(pDrawable->drawable_id, x1, y1, width, height);
}

#define CHANNELS	4

struct Point	{ int x, y; };

void GaussBlur(BYTE *pDst, BYTE *pSrc, int width, int height, int channels, int r) {
	int sr = ceil(2.57 * r);	// Significant radius
	int d = 2*sr + 1;	// diameter
	std::vector<double> kernel(d * d);
	double sum = 0;
	
	// Build kernel
	EREP(y, -sr, sr) EREP(x, -sr, sr) sum += kernel[d * (y+sr) + (x+sr)] = exp( -(x*x+y*y)/(2.0*r*r) );
	REP(y, d) REP(x, d) kernel[d * y + x] /= sum;
	
	// Filtering
	REP(y, height) REP(x, width) {
		double ch[CHANNELS] = {};
		EREP(yy, -sr, sr) EREP(xx, -sr, sr) {
			Point pt = { CLIP(x + xx, 0, width - 1), CLIP(y + yy, 0, height - 1) };
			BYTE *pPixel = &pSrc[channels * (width * pt.y + pt.x)];
			REP(c, channels) ch[c] += kernel[d * (yy+sr) + (xx+sr)] * (double)pPixel[c];
		}
		BYTE *pPixel = &pDst[channels * (width * y + x)];
		REP(c, channels) pPixel[c] = (BYTE)ch[c];
	}
}
