#pragma once

void	Msg(PCSTR psFmt, ...) {
#define MSG_BUF_LEN	0x200
	va_list	ap;
	va_start(ap, psFmt);
	char	sMsg[MSG_BUF_LEN];
	::vsprintf(sMsg, psFmt, ap);
	va_end(ap);
	::MessageBoxA(NULL, sMsg, "DEBUG MSG", MB_OK | MB_ICONEXCLAMATION);
}

typedef PVOID (__cdecl *PFNFUNC)(...);

class Dll {
	HMODULE	hModule_;
	bool	bAlreadyLoaded_;
	
public:
	bool	Invalid()	{	return hModule_ == NULL;	}
	
	PFNFUNC	GetFunction(PCSTR psFuncName) {
		FARPROC	pfnProc = ::GetProcAddress(hModule_, psFuncName);
		
		if( !pfnProc )	::Msg("Not found such function: %s", psFuncName);
		return (PFNFUNC)pfnProc;
	}
	Dll(PCSTR psDllName) : bAlreadyLoaded_(false) {
		if( (hModule_ = ::GetModuleHandle(psDllName)) ) {
			//::Msg("Already loaded: %s", psDllName);
			bAlreadyLoaded_ = true;
			return;
		}
		if( !(hModule_ = ::LoadLibrary(psDllName)) ) ::Msg("Error loading: %s", psDllName);
	}
	~Dll()	{	if(!bAlreadyLoaded_) ::FreeLibrary(hModule_);	}
};
