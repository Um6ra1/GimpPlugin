#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS 1
#include <Windows.h>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <vector>
#include <algorithm>
#include "../GimpCommon.h"
#include "../Dll.h"
#include "Dialog.h"
#include "FFTImage.h"
#include "BmpUtil.h"

void Query();
void Run(PCSTR psName, int params, const GimpParam *pSrc, int *pReturns, GimpParam **ppDst);

const GimpPlugInInfo PLUG_IN_INFO = { NULL, NULL, Query, Run };
PLUGIN_PARAMS g_PluginParams = g_defaultParams;
HINSTANCE g_hInst;

int __stdcall WinMain(HINSTANCE hInst, HINSTANCE, char *psCmdLine, int nCmdShow) {
	g_hInst	= hInst;
	Dll	libFFI("libffi-6.dll"); if(libFFI.Invalid()) return -1;
	Dll	libGIo("libgio-2.0-0.dll"); if(libGIo.Invalid()) return -1;
	Dll	libGLib("libglib-2.0-0.dll"); if(libGLib.Invalid()) return -1;
	Dll	libGMod("libgmodule-2.0-0.dll"); if(libGMod.Invalid()) return -1;
	Dll	libGObj("libgobject-2.0-0.dll"); if(libGObj.Invalid()) return -1;
	Dll	libIntl("libintl-8.dll"); if(libIntl.Invalid()) return -1;
	Dll	libGimpBase("libgimpbase-2.0-0.dll"); if(libGimpBase.Invalid()) return -1;
	Dll	libGimp("libgimp-2.0-0.dll"); if(libGimp.Invalid()) return -1;
	Func gimp_main = (Func)libGimp.GetFunction("gimp_main");
	
	return (int)gimp_main(&PLUG_IN_INFO, __argc, __argv);
}

PCSTR PLUGIN_NAME = "plug-in-LensBlur";
PCSTR PLUGIN_BLURB = "LensBlur";
PCSTR PLUGIN_HELP = "Lens bluring plugin.";
PCSTR PLUGIN_AUTHOR	= "Um6ra1";
PCSTR PLUGIN_COPYRIGHT = "Copyright (C) Um6ra1";
PCSTR PLUGIN_DATE = "2018";
PCSTR PLUGIN_MENULABEL = "_LensBlur";
PCSTR PLUGIN_IMGTYPES = "RGB*, GRAY*";
PCSTR PLUGIN_MENUPATH = "<Image>/Filters/VsNative-Plugin";

void Query() {
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return;
	Func gimp_install_procedure	= libGimp.GetFunction("gimp_install_procedure");
	Func gimp_plugin_menu_register	= libGimp.GetFunction("gimp_plugin_menu_register");
	
	static GimpParamDef	args[] = {
		{GIMP_PDB_INT32, (char *)"run-mode", (char *)"Run mode"},
		{GIMP_PDB_IMAGE, (char *)"image", (char *)"Input image"},
		{GIMP_PDB_DRAWABLE, (char *)"drawable", (char *)"Input drawable"},
	};
	//static GimpParamDef	results[] =
	gimp_install_procedure(
		PLUGIN_NAME,
		PLUGIN_BLURB,
		PLUGIN_HELP,
		PLUGIN_AUTHOR,
		PLUGIN_COPYRIGHT,
		PLUGIN_DATE,
		PLUGIN_MENULABEL,
		PLUGIN_IMGTYPES,
		GIMP_PLUGIN,
		G_N_ELEMENTS(args), 0,
		args, NULL
	);

	int status = (int)gimp_plugin_menu_register(PLUGIN_NAME, PLUGIN_MENUPATH);
	if(!status)	Msg("gimp_plugin_menu_register() failed!");
}

void Proc(Dll &libGimp, GimpDrawable *pDrawable) {
	Func gimp_drawable_mask_bounds = libGimp.GetFunction("gimp_drawable_mask_bounds");
	Func gimp_drawable_bpp = libGimp.GetFunction("gimp_drawable_bpp");
	Func gimp_pixel_rgn_init = libGimp.GetFunction("gimp_pixel_rgn_init");
	Func gimp_drawable_flush = libGimp.GetFunction("gimp_drawable_flush");
	Func gimp_drawable_merge_shadow = libGimp.GetFunction("gimp_drawable_merge_shadow");
	Func gimp_drawable_update = libGimp.GetFunction("gimp_drawable_update");
	Func gimp_pixel_rgn_get_rect = libGimp.GetFunction("gimp_pixel_rgn_get_rect");
	Func gimp_pixel_rgn_set_rect = libGimp.GetFunction("gimp_pixel_rgn_set_rect");

	int x1, y1, x2, y2;
	gimp_drawable_mask_bounds(pDrawable->drawable_id, &x1, &y1, &x2, &y2);
	int	channels = (int)gimp_drawable_bpp(pDrawable->drawable_id);
	if (channels != 4) {
		Msg("Channels must be RGBA! Your image has %d channels", channels);
		return;
	}
	int	w = x2 - x1;
	int	h = y2 - y1;
	//Msg("w, h, c = %d, %d, %d", w, h, channels);
	GimpPixelRgn rgnDst, rgnSrc;
	gimp_pixel_rgn_init(&rgnDst, pDrawable, x1, y1, w, h, true, true);
	gimp_pixel_rgn_init(&rgnSrc, pDrawable, x1, y1, w, h, false, false);

	ImgProc::Image img(w, h);
	gimp_pixel_rgn_get_rect(&rgnSrc, &img.buf[0], x1, y1, w, h);

	//	Msg("%08X, %08X", *(UINT32 *)&src[0], *(UINT32 *)&src[4]);
	CBitmap ker;
	if (ker.Load(g_PluginParams.kernelFilename) < 0) Msg("Failed to load lernel!");
	ImgProc::FFTConvImage(img, ker.Image());

	gimp_pixel_rgn_set_rect(&rgnDst, &img.buf[0], x1, y1, w, h);
	gimp_drawable_flush(pDrawable);
	gimp_drawable_merge_shadow(pDrawable->drawable_id, true);
	gimp_drawable_update(pDrawable->drawable_id, x1, y1, w, h);
}

void Run(PCSTR psName, int srcNum, const GimpParam *pSrc, int *pDstNum, GimpParam **ppDst) {
	Dll	libGimp("libgimp-2.0-0.dll"); if(libGimp.Invalid()) return;
	Func gimp_drawable_get	= libGimp.GetFunction("gimp_drawable_get");
	Func gimp_drawable_detach = libGimp.GetFunction("gimp_drawable_detach");
	Func gimp_displays_flush = libGimp.GetFunction("gimp_displays_flush");
	Func gimp_progress_init = libGimp.GetFunction("gimp_progress_init");
	Func gimp_procedural_db_set_data = libGimp.GetFunction("gimp_procedural_db_set_data");
	Func gimp_procedural_db_get_data = libGimp.GetFunction("gimp_procedural_db_get_data");
	static GimpParam values[1];

	*pDstNum = 1;
	*ppDst = values;
	values[0].type = GIMP_PDB_STATUS;
	values[0].data.d_status = GIMP_PDB_SUCCESS;
	
	switch(pSrc[0].data.d_int32) {
		case GIMP_RUN_INTERACTIVE:
			gimp_procedural_db_get_data(PLUGIN_NAME, &g_PluginParams);
			if(!DoModal(g_hInst, &g_PluginParams)) return;
			gimp_procedural_db_set_data(PLUGIN_NAME, &g_PluginParams, sizeof(g_PluginParams));
			break;
			
		case GIMP_RUN_NONINTERACTIVE:
			break;
			
		case GIMP_RUN_WITH_LAST_VALS:
			gimp_procedural_db_get_data(PLUGIN_NAME, &g_PluginParams);
			break;
	}

	GimpDrawable *pDrawable = (GimpDrawable *)gimp_drawable_get(pSrc[2].data.d_drawable);
	Proc(libGimp, pDrawable);
	gimp_displays_flush();
	gimp_drawable_detach(pDrawable);
}
