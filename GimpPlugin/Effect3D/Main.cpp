#define _USE_MATH_DEFINES
#include <Windows.h>
#include <vector>
#include "../GimpCommon.h"
#include "../Dll.h"
#include "../Macros.h"

#pragma comment(lib, "user32.lib")

void Query();
void Run(PCSTR psName, int params, const GimpParam *pSrc, int *pReturns, GimpParam **ppDst);
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
	
	Func pfGimpMain = libGimp.GetFunction("gimp_main");
	
	return (int)pfGimpMain(&PLUG_IN_INFO, __argc, __argv);
}

PCSTR PLUGIN_NAME = "plug-in-Effect3D";
PCSTR PLUGIN_BLURB = "Effect3D";
PCSTR PLUGIN_HELP = "Misregistration effect.";
PCSTR PLUGIN_AUTHOR = "Um6ra1";
PCSTR PLUGIN_COPYRIGHT = "Copyright (C) Um6ra1";
PCSTR PLUGIN_DATE = "2017";
PCSTR PLUGIN_MENULABEL = "_Effect3D";
PCSTR PLUGIN_IMGTYPES = "RGB*, GRAY*";
PCSTR PLUGIN_MENUPATH = "<Image>/Filters/VsNative-Plugin";

void	Query() {
	Dll libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return;

	Func pfGimpInstallProcedure = libGimp.GetFunction("gimp_install_procedure");
	Func pfGimpPluginMenuRegister = libGimp.GetFunction("gimp_plugin_menu_register");
	
	static GimpParamDef	args[] = {
		{GIMP_PDB_INT32, "run-mode", "Run mode"},
		{GIMP_PDB_IMAGE, "image", "Input image"},
		{GIMP_PDB_DRAWABLE, "drawable", "Input drawable"},
	};
	pfGimpInstallProcedure (
		PLUGIN_NAME, PLUGIN_BLURB,
		PLUGIN_HELP, PLUGIN_AUTHOR,
		PLUGIN_COPYRIGHT, PLUGIN_DATE,
		PLUGIN_MENULABEL, PLUGIN_IMGTYPES,
		GIMP_PLUGIN,
		G_N_ELEMENTS(args), 0,
		args, NULL
	);

	int status = (int)pfGimpPluginMenuRegister(PLUGIN_NAME, PLUGIN_MENUPATH);
	if(!status) ::Msg("gimp_plugin_menu_register() failed!");
}

void ImgProc(Dll &libGimp, GimpDrawable *pDrawable);

void Run(PCSTR psName, int srcNum, const GimpParam *pSrc, int *pDstNum, GimpParam **ppDst) {
	static GimpParam	values[1];
	Dll libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return;

	Func pfGimpDrawableGet = libGimp.GetFunction("gimp_drawable_get");
	Func pfGimpDrawableDetach = libGimp.GetFunction("gimp_drawable_detach");
	Func pfGimpDisplaysFlush = libGimp.GetFunction("gimp_displays_flush");

	*pDstNum = 1;
	*ppDst = values;
	values[0].type = GIMP_PDB_STATUS;
	values[0].data.d_status = GIMP_PDB_SUCCESS;
	
	GimpDrawable *pDrawable = (GimpDrawable *)pfGimpDrawableGet(pSrc[2].data.d_drawable);

	::ImgProc(libGimp, pDrawable);
	pfGimpDisplaysFlush();
	pfGimpDrawableDetach(pDrawable);
}

typedef union _ARGB {
	BYTE c[4];
	struct { BYTE b, g, r, a; };
	UINT32 dw;
} ARGB;

void ImgProc(Dll &libGimp, GimpDrawable *pDrawable) {
	Func	pfGimpMaskBounds = libGimp.GetFunction("gimp_drawable_mask_bounds");
	Func	pfGimpDrawableBpp = libGimp.GetFunction("gimp_drawable_bpp");
	Func	pfGimpPixelRgnInit = libGimp.GetFunction("gimp_pixel_rgn_init");
	Func	pfGimpDrawableFlush = libGimp.GetFunction("gimp_drawable_flush");
	Func	pfGimpDrawableMergeShadow = libGimp.GetFunction("gimp_drawable_merge_shadow");
	Func	pfGimpDrawableUpdate = libGimp.GetFunction("gimp_drawable_update");
	Func	pfGimpPixelRgnGetRect = libGimp.GetFunction("gimp_pixel_rgn_get_rect");
	Func	pfGimpPixelRgnSetRect = libGimp.GetFunction("gimp_pixel_rgn_set_rect");
	Func	pfGimpTileCacheNTiles = libGimp.GetFunction("gimp_tile_cache_ntiles");
	Func	pfGimpTileWidth = libGimp.GetFunction("gimp_tile_width");

	int x1, y1, x2, y2;
	pfGimpMaskBounds(pDrawable->drawable_id, &x1, &y1, &x2, &y2);
	int channels = (int)pfGimpDrawableBpp(pDrawable->drawable_id);
	int width = x2 - x1;
	int height = y2 - y1;
	GimpPixelRgn rgnDst, rgnSrc;
	pfGimpPixelRgnInit(&rgnDst, pDrawable, x1, y1, width, height, true, true);
	pfGimpPixelRgnInit(&rgnSrc, pDrawable, x1, y1, width, height, false, false);
	
	if(channels != 4) { ::Msg("Channes != 4"); return; }

	std::vector<ARGB>	buf(width * height);

	pfGimpPixelRgnGetRect(&rgnSrc, &buf[0], x1, y1, width, height);
	
#define DRAW_SCREEN(x, y)	(BYTE)(255.0 - (double)((255 - (x)) * (255 - (y))) / 255.0)
	int displacement = 3;
	ARGB white = {0xFFFFFFFF};
	
	REP(y, height) REP(x, width) {
		ARGB &pixel = *(ARGB *)&buf[width * y + x];
		ARGB red = (x > displacement) ? *(ARGB *)&buf[width * y + (x - displacement)] : white;
		ARGB cyan = (x < width - displacement) ? *(ARGB *)&buf[width * y + (x + displacement)]: white;

		pixel.r = DRAW_SCREEN(pixel.r, red.r);
		pixel.g = DRAW_SCREEN(pixel.g, cyan.g);
		pixel.b = DRAW_SCREEN(pixel.b, cyan.b);

		//*(ARGB *)&buf[width * y + x]	= pixel;
	}

	pfGimpPixelRgnSetRect(&rgnDst, &buf[0], x1, y1, width, height);

	pfGimpDrawableFlush(pDrawable);
	pfGimpDrawableMergeShadow(pDrawable->drawable_id, true);
	pfGimpDrawableUpdate(pDrawable->drawable_id, x1, y1, width, height);
}
