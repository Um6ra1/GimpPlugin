#define _USE_MATH_DEFINES
#include <Windows.h>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <vector>
#include <algorithm>
#include "../GimpCommon.h"
#include "../Dll.h"
#include "Dialog.h"

#pragma comment(lib, "advapi32.lib")

#define REP(idx, max)	for(int idx = 0, idx##Max = max; idx < idx##Max; idx ++)
#define CEILDIV(n, d)	(((n) + (d) - 1) / (d))
#define DEG2RAD(deg)	((deg) * M_PI / 180.0)
#define RAD2DEG(deg)	((deg) * 180.0 / M_PI)

void	Query();
void	Run(PCSTR psName, int params, const GimpParam *pSrc, int *pReturns, GimpParam **ppDst);

const GimpPlugInInfo	PLUG_IN_INFO = { NULL, NULL, Query, Run };

class Random {
	HCRYPTPROV	hProv_;
	
public:
	double	GetRand() {
		double	x;
		::CryptGenRandom(hProv_, sizeof(x), (BYTE *)&x);
		return x;
	}
	
	Random() {	::CryptAcquireContext(&hProv_, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);	}
	~Random() {	::CryptReleaseContext(hProv_, NULL);	}
};

PLUGIN_PARAMS	g_PluginParams = g_defaultParams;
HINSTANCE g_hInst;

int __stdcall	WinMain(HINSTANCE hInst, HINSTANCE, char *psCmdLine, int nCmdShow) {
	//::DoModal(g_hInst, &g_PluginParams);
	g_hInst	= hInst;
	Dll	libFFI("libffi-6.dll");	if(libFFI.Invalid())	return -1;
	Dll	libGIo("libgio-2.0-0.dll");	if(libGIo.Invalid())	return -1;
	Dll	libGLib("libglib-2.0-0.dll");	if(libGLib.Invalid())	return -1;
	Dll	libGMod("libgmodule-2.0-0.dll");	if(libGMod.Invalid())	return -1;
	Dll	libGObj("libgobject-2.0-0.dll");	if(libGObj.Invalid())	return -1;
	Dll	libIntl("libintl-8.dll");	if(libIntl.Invalid())	return -1;

	Dll	libGimpBase("libgimpbase-2.0-0.dll");	if(libGimpBase.Invalid())	return -1;
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return -1;
	
	Func	pfnGimpMain	=  (Func)libGimp.GetFunction("gimp_main");
	
	return (int)pfnGimpMain(&PLUG_IN_INFO, __argc, __argv);
}

PCSTR	PLUGIN_NAME		= "plug-in-LineNova";
PCSTR	PLUGIN_BLURB	= "LineNova";
PCSTR	PLUGIN_HELP		= "Draw line nova.";
PCSTR	PLUGIN_AUTHOR	= "Um6ra1";
PCSTR	PLUGIN_COPYRIGHT		= "Copyright (C) Um6ra1";
PCSTR	PLUGIN_DATE				= "2016";
PCSTR	PLUGIN_MENULABEL	= "_LineNova";
PCSTR	PLUGIN_IMGTYPES		= "RGB*, GRAY*";

PCSTR	PLUGIN_MENUPATH		= "<Image>/Filters/VsNative-Plugin";

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
	Func pfGimpSetData = libGimp.GetFunction("gimp_procedural_db_set_data");
	Func pfGimpGetData = libGimp.GetFunction("gimp_procedural_db_get_data");

	*pDstNum	= 1;
	*ppDst		= values;
	values[0].type	= GIMP_PDB_STATUS;
	values[0].data.d_status	= GIMP_PDB_SUCCESS;
	
	switch(pSrc[0].data.d_int32) {
		case GIMP_RUN_INTERACTIVE:
			pfGimpGetData(PLUGIN_NAME, &g_PluginParams);
			if(!::DoModal(g_hInst, &g_PluginParams))	return;
			pfGimpSetData(PLUGIN_NAME, &g_PluginParams, sizeof(g_PluginParams));
			break;
			
		case GIMP_RUN_NONINTERACTIVE:
			break;
			
		case GIMP_RUN_WITH_LAST_VALS:
			pfGimpGetData(PLUGIN_NAME, &g_PluginParams);
			break;
	}

	GimpDrawable *pDrawable = (GimpDrawable *)pfGimpDrawableGet(pSrc[2].data.d_drawable);

	::ImgProc(libGimp, pDrawable);
	pfGimpDisplaysFlush();
	pfGimpDrawableDetach(pDrawable);
}

struct Point {
	double x, y;
	Point() {}
	Point(double x_, double y_) : x(x_), y(y_) {}
};

#define CROSS_PROD(px, py, qx, qy)	((px) * (qy) - (py) * (qx))
inline Point InterSection(Point &p1, Point &p2, Point &q1, Point &q2) {
	double	n = CROSS_PROD(q2.x - q1.x, q2.y - q1.y, p1.x - q1.x, p1.y - q1.y);
	double	d = CROSS_PROD(q2.x - q1.x, q2.y - q1.y, p1.x - p2.x, p1.y - p2.y);
	return Point((p2.x - p1.x) * n / d, (p2.y - p1.y) * n / d);
}

void	ImgProc(Dll &libGimp, GimpDrawable *pDrawable) {
	int			lineNum = g_PluginParams.lineNum;
	double	degCornerAng = g_PluginParams.degCornerAngX100 / 100.0;
	double	offset = g_PluginParams.offset;
	double	variation	= g_PluginParams.variation;

	Func	pfGimpMaskBounds = libGimp.GetFunction("gimp_drawable_mask_bounds");
	Func	pfGimpDrawableOffsets = libGimp.GetFunction("gimp_drawable_offsets");
	Func	pfGimpImageSelectPolygon = libGimp.GetFunction("gimp_image_select_polygon");
	Func	pfGimpSelectionNone = libGimp.GetFunction("gimp_selection_none");
	Func	pfGimpDrawableGetImage = libGimp.GetFunction("gimp_drawable_get_image");
	Func	pfGimpEditFill = libGimp.GetFunction("gimp_edit_fill");

	int x1, y1, x2, y2;
	pfGimpMaskBounds(pDrawable->drawable_id, &x1, &y1, &x2, &y2);
	int offsetX, offsetY;
	pfGimpDrawableOffsets(pDrawable->drawable_id, &offsetX, &offsetY);

	int width = x2 - x1;
	int height = y2 - y1;
	double radius = max(width, height);

	Random	random;
	Point	center(offsetX + width * 0.5, offsetY + height * 0.5);
	std::vector<Point>	vertices(lineNum * 3);
	double radAngle	= DEG2RAD(degCornerAng);
	REP(i, lineNum) {
		double ofs = offset + ::fmod(random.GetRand(), variation) - variation * 0.5;
		double degDir = 360.0 * (double)i / (double)lineNum;

		radAngle = DEG2RAD(degDir - degCornerAng);
		vertices[3 * i + 0] = Point(center.x + radius * ::cos(radAngle), center.y + radius * ::sin(radAngle));
		radAngle = (2.0 * M_PI) * (double)i / (double)lineNum;
		vertices[3 * i + 1] = Point(center.x + ofs * ::cos(radAngle), center.y + ofs * ::sin(radAngle));
		radAngle = DEG2RAD(degDir + degCornerAng);
		vertices[3 * i + 2] = Point(center.x + radius * ::cos(radAngle), center.y + radius * ::sin(radAngle));
	}
	UINT32 imageID = (UINT32)pfGimpDrawableGetImage(pDrawable->drawable_id);
	pfGimpImageSelectPolygon(imageID, GIMP_CHANNEL_OP_ADD, 2 * vertices.size(), &vertices[0]);

	if (g_PluginParams.bFill)
		pfGimpEditFill(pDrawable->drawable_id, GIMP_FOREGROUND_FILL);
	if (!g_PluginParams.bLeaveSelection)
		pfGimpSelectionNone(imageID);
}
