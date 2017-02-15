#include <Windows.h>
#include <cstdio>
#include <vector>
#include "../GimpCommon.h"
#include "../Dll.h"
#include "../Macros.h"

#pragma comment(lib, "user32.lib")

G_BEGIN_DECLS
	void	Query();
	void	Run(PCSTR psName, int params, const GimpParam *pSrc, int *pReturns, GimpParam **ppDst);
G_END_DECLS

const GimpPlugInInfo	PLUG_IN_INFO = { NULL, NULL, Query, Run };

int __stdcall	WinMain(HINSTANCE hInst, HINSTANCE, char *psCmdLine, int nCmdShow) {
	Dll	libFFI("libffi-6.dll");	if(libFFI.Invalid())	return -1;
	Dll	libGIo("libgio-2.0-0.dll");	if(libGIo.Invalid())	return -1;
	Dll	libGLib("libglib-2.0-0.dll");	if(libGLib.Invalid())	return -1;
	Dll	libGMod("libgmodule-2.0-0.dll");	if(libGMod.Invalid())	return -1;
	Dll	libGObj("libgobject-2.0-0.dll");	if(libGObj.Invalid())	return -1;
	Dll	libIntl("libintl-8.dll");	if(libIntl.Invalid())	return -1;
	Dll	libGimpBase("libgimpbase-2.0-0.dll");	if(libGimpBase.Invalid())	return -1;
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return -1;
	
	Func	pfGimpMain = libGimp.GetFunction("gimp_main");
	if(!pfGimpMain) { ::MessageBox(0, "Error loading gimp_main", "ERROR", MB_OK);	return -1; }	
	
	return (int)pfGimpMain(&PLUG_IN_INFO, __argc, __argv);
}

PCSTR PLUGIN_NAME = "plug-in-Invert";
PCSTR PLUGIN_BLURB	= "Invert Color";
PCSTR PLUGIN_HELP = "Invert color.";
PCSTR PLUGIN_AUTHOR = "Um6ra1";
PCSTR PLUGIN_COPYRIGHT = "Copyright (C) Um6ra1";
PCSTR PLUGIN_DATE = "2017";
PCSTR PLUGIN_MENULABEL = "_Invert";
PCSTR PLUGIN_IMGTYPES = "RGB*, GRAY*";
PCSTR PLUGIN_MENUPATH = "<Image>/Filters/VsNative-Plugin";

void	Query() {
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return;

	Func pfGimpInstallProcedure	= libGimp.GetFunction("gimp_install_procedure");
	Func pfGimpPluginMenuRegister	= libGimp.GetFunction("gimp_plugin_menu_register");
	
	static GimpParamDef	args[] = {
		{GIMP_PDB_INT32, "run-mode", "Run mode"},
		{GIMP_PDB_IMAGE, "image", "Input image"},
		{GIMP_PDB_DRAWABLE, "drawable", "Input drawable"},
	};
	//static GimpParamDef	results[] =
	pfGimpInstallProcedure(
		PLUGIN_NAME, PLUGIN_BLURB,
		PLUGIN_HELP, PLUGIN_AUTHOR,
		PLUGIN_COPYRIGHT, PLUGIN_DATE,
		PLUGIN_MENULABEL, PLUGIN_IMGTYPES,
		GIMP_PLUGIN,
		G_N_ELEMENTS(args), 0,
		args, NULL
	);

	int status = (int)pfGimpPluginMenuRegister(PLUGIN_NAME, PLUGIN_MENUPATH);
	if(!status)	::Msg("gimp_plugin_menu_register() failed!");
}

void	ImgProc(Dll &libGimp, GimpDrawable *pDrawable);

void	Run(PCSTR psName, int srcNum, const GimpParam *pSrc, int *pDstNum, GimpParam **ppDst) {
	static GimpParam	values[1];
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return;

	Func pfGimpDrawableGet	= libGimp.GetFunction("gimp_drawable_get");
	Func pfGimpDrawableDetach = libGimp.GetFunction("gimp_drawable_detach");
	Func pfGimpDisplaysFlush = libGimp.GetFunction("gimp_displays_flush");
	Func pfGimpProgressInit = libGimp.GetFunction("gimp_progress_init");

	*pDstNum	= 1;
	*ppDst		= values;
	values[0].type	= GIMP_PDB_STATUS;
	values[0].data.d_status	= GIMP_PDB_SUCCESS;
	
	GimpDrawable *pDrawable = (GimpDrawable *)pfGimpDrawableGet(pSrc[2].data.d_drawable);

	::ImgProc(libGimp, pDrawable);
	pfGimpDisplaysFlush();
	pfGimpDrawableDetach(pDrawable);
}

void	ImgProc(Dll &libGimp, GimpDrawable *pDrawable) {
	GimpPixelRgn	rgnDst, rgnSrc;
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
	//::Msg("x, y, w, h = %d, %d, %d, %d", x1, y1, width, height);
	//(*pfGimpTileCacheNTiles)(CEILDIV(pDrawable->width, (int)(*pfGimpTileWidth)()));
	pfGimpPixelRgnInit(&rgnDst, pDrawable, x1, y1, width, height, true, true);
	pfGimpPixelRgnInit(&rgnSrc, pDrawable, x1, y1, width, height, false, false);

	std::vector<BYTE>	buf(width * height * channels);

	pfGimpPixelRgnGetRect(&rgnSrc, &buf[0], x1, y1, width, height);

	REP(y, height) REP(x, width) {
		UINT32 &pixel = *(UINT32 *)&buf[channels * (width * y + x)];
		REP(c, channels) if (c < 3)	{
			BYTE	&p = ((BYTE *)&pixel)[c];
			p = ~p;
		}
	}

	pfGimpPixelRgnSetRect(&rgnDst, &buf[0], x1, y1, width, height);

	pfGimpDrawableFlush(pDrawable);
	pfGimpDrawableMergeShadow(pDrawable->drawable_id, true);
	pfGimpDrawableUpdate(pDrawable->drawable_id, x1, y1, width, height);
}
