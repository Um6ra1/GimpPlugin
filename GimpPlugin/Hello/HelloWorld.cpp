#define _USE_MATH_DEFINES
#include <Windows.h>
#include <cstdio>
#include "../GimpCommon.h"
#include "../Dll.h"

#pragma comment(lib, "user32.lib")

void	Query();
void	Run(PCSTR psName, int params, const GimpParam *pSrc, int *pReturns, GimpParam **ppDst);
const GimpPlugInInfo	PLUG_IN_INFO = { NULL, NULL, Query, Run };

int WINAPI	WinMain(HINSTANCE hInst, HINSTANCE, char *psCmdLine, int nCmdShow) {
	Dll	libFFI("libffi-6.dll");
	Dll	libGIo("libgio-2.0-0.dll");
	Dll	libGLib("libglib-2.0-0.dll");
	Dll	libGMod("libgmodule-2.0-0.dll");
	Dll	libGObj("libgobject-2.0-0.dll");
	Dll	libIntl("libintl-8.dll");

	Dll	libGimpBase("libgimpbase-2.0-0.dll");
	Dll	libGimp("libgimp-2.0-0.dll");
	
	Func	pfnGimpMain	=  libGimp.GetFunction("gimp_main");
	
	return (int)pfnGimpMain(&PLUG_IN_INFO, __argc, __argv);
}

PCSTR　PLUGIN_NAME　= "plug-in-Hello";
PCSTR　PLUGIN_BLURB　= "Hello world";
PCSTR　PLUGIN_HELP　= "Hello world plugin on Windows.";
PCSTR　PLUGIN_AUTHOR　= "Um6ra1";
PCSTR　PLUGIN_COPYRIGHT　= "Copyright (C) Um6ra1";
PCSTR　PLUGIN_DATE　= "2016";
PCSTR　PLUGIN_MENULABEL　= "_HelloWorld";
PCSTR　PLUGIN_IMGTYPES　= "RGB*, GRAY*";
PCSTR　PLUGIN_MENUPATH　= "<Image>/Filters/VsNative-Plugin";

void	Query() {
	Dll	libGimp("libgimp-2.0-0.dll");

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
	if(!status)	Msg("gimp_plugin_menu_register() failed!");
}

void	ImgProc(Dll &libGimp, GimpDrawable *pDrawable);

void	Run(PCSTR psName, int srcNum, const GimpParam *pSrc, int *pDstNum, GimpParam **ppDst) {
	static GimpParam	values[1];

	*pDstNum	= 1;
	*ppDst		= values;
	values[0].type	= GIMP_PDB_STATUS;
	values[0].data.d_status	= GIMP_PDB_SUCCESS;
	
	int runMode = values[0].data.d_int32;
	if(runMode != GIMP_RUN_INTERACTIVE)	Msg("Hello world!");
}
