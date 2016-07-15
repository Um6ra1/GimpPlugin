
#include <Windows.h>
#include <cstdio>
#include "Typedefs.h"
#include "gimp.h"

#pragma comment(lib, "user32.lib")

G_BEGIN_DECLS
	void	Query();
	void	Run(LPCSTR lpszName, int params, const GimpParam *lpSrc, int *lpReturns, GimpParam **lplpDst);
G_END_DECLS

const GimpPlugInInfo	PLUG_IN_INFO =
{
	NULL, NULL, Query, Run
};

void	Msg(LPCSTR lpszFmt, ...)
{
#define MSG_BUF_LEN	0x200
	va_list	ap;
	va_start(ap, lpszFmt);
	char	szMsg[MSG_BUF_LEN];
	::vsprintf(szMsg, lpszFmt, ap);
	va_end(ap);
	::MessageBoxA(NULL, szMsg, "DEBUG MSG", MB_OK | MB_ICONEXCLAMATION);
}

class Dll
{
	HMODULE	hModule_;
	bool	bAlreadyLoaded;
	
public:
	bool	Invalid()	{	return hModule_ == NULL;	}
	
	FARPROC	GetFunction(LPCSTR lpszFuncName)
	{
		FARPROC	lpfnProc;
		
		if( !(lpfnProc = ::GetProcAddress(hModule_, lpszFuncName)) )
		{	::Msg("Not found such function: %s", lpszFuncName);	}
		return lpfnProc;
	}
	Dll(LPCSTR lpszDllName) : bAlreadyLoaded(false)
	{
		if( (hModule_ = ::GetModuleHandle(lpszDllName)) )
		{
			//::Msg("Already loaded: %s", lpszDllName);
			bAlreadyLoaded = true;
			return;
		}
	
		if( !(hModule_ = ::LoadLibrary(lpszDllName)) )
		{	::Msg("Error loading: %s", lpszDllName);	}
	}
	~Dll()	{	if(!bAlreadyLoaded) ::FreeLibrary(hModule_);	}
};

//MAIN()
int __stdcall	WinMain(HINSTANCE hInst, HINSTANCE, char *lpszCmdLine, int nCmdShow)
{
	::Msg("WinMain");
	
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
	
	LPFNGIMPMAIN	lpfnGimpMain	=  (LPFNGIMPMAIN)libGimp.GetFunction("gimp_main");
	if(!lpfnGimpMain)
	{	::MessageBox(0, "Error loading gimp_main", "ERROR", MB_OK);	return -1;	}	
	
	return (*lpfnGimpMain)(&PLUG_IN_INFO, __argc, __argv);
}

/*
int	main(int argc, char *argv[])	// for Console Application
{
	return ::WinMain(::GetModuleHandle(0), NULL, NULL, SW_SHOWDEFAULT);
//	return gimp_main(&PLUG_IN_INFO, __argc, __argv);
}
*/
void	Query()
{
	::Msg("Query");
	
	int	status;
	typedef void (__cdecl *LPFNGIMP_INSTALL_PROC)(LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR, GimpPDBProcType, int, int, const GimpParamDef *, const GimpParamDef *);
	typedef bool (__cdecl *LPFNGIMP_PLUGIN_MENU_REGISTER)(LPCSTR, LPCSTR);
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return;
	
	LPFNGIMP_INSTALL_PROC lpfnGimpInstallProcedure	= (LPFNGIMP_INSTALL_PROC)libGimp.GetFunction("gimp_install_procedure");
	if(!lpfnGimpInstallProcedure)	return;
	LPFNGIMP_PLUGIN_MENU_REGISTER lpfnGimpPluginMenuRegister	= (LPFNGIMP_PLUGIN_MENU_REGISTER)libGimp.GetFunction("gimp_plugin_menu_register");
	if(!lpfnGimpPluginMenuRegister)	return;
	
	static GimpParamDef	args[] =
	{
		{GIMP_PDB_INT32, "run-mode", "Run mode"},
		{GIMP_PDB_IMAGE, "image", "Input image"},
		{GIMP_PDB_DRAWABLE, "drawable", "Input drawable"},
	};
	//static GimpParamDef	results[] =
	(*lpfnGimpInstallProcedure)
	(
		"plug-in-VsTest",
		"plug-in-VsTest",
		 "Displays \"Hello, world!\" in a dialog",
		"????",
		"Copyright (C) Um6ra1",
		"2016",
		"_plug-in-VsTest",
		"RGB*, GRAY*",
		GIMP_PLUGIN,
		G_N_ELEMENTS(args), 0,
		args, NULL
	);

	status = (*lpfnGimpPluginMenuRegister)("plug-in-VsTest", "<Image>/Filters/VsNative-Plugin");
	if(!status)	::Msg("gimp_plugin_menu_register() failed!");
}

void	Run(LPCSTR lpszName, int srcNum, const GimpParam *lpSrc, int *lpDstNum, GimpParam **lplpDst)
{
	::Msg("Run");
	
//	typedef GimpDrawable *(__cdecl *LPFNGIMP_DRAWABLE_GET)(INT32);
	
	static GimpParam	values[1];
//	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return;
//	LPFNGIMP_DRAWABLE_GET lpfnGimpDrawableGet	= (LPFNGIMP_DRAWABLE_GET)libGimp.GetFunction("gimp_drawable_get");
//	if(!lpfnGimpDrawableGet)	return;
	
	*lpDstNum	= 1;
	*lplpDst		= values;
	
	values[0].type	= GIMP_PDB_STATUS;
	values[0].data.d_status	= GIMP_PDB_SUCCESS;
	
	if(values[0].data.d_int32 != GIMP_RUN_NONINTERACTIVE)	::Msg("Salve munde!");
}
