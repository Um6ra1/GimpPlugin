#pragma once

void	Msg(PCSTR psFmt, ...) {
#define MSG_BUF_LEN	0x200
	va_list ap;
	va_start(ap, psFmt);
	char sMsg[MSG_BUF_LEN];
	::vsprintf(sMsg, psFmt, ap);
	va_end(ap);
	::MessageBoxA(NULL, sMsg, "DEBUG MSG", MB_OK | MB_ICONEXCLAMATION);
}

typedef PVOID (__cdecl *Func)(...);

class Dll {
	HMODULE	hModule_;
	bool	bAlreadyLoaded_;

public:
	bool Invalid() { return hModule_ == NULL; }
	
	Func	GetFunction(PCSTR psFuncName) {
		FARPROC pfProc = ::GetProcAddress(hModule_, psFuncName);
		
		if( !pfProc ) ::Msg("Not found such function: %s", psFuncName);
		return (Func)pfProc;
	}
	Dll(PCSTR psDllName) : bAlreadyLoaded_(false) {
		if( (hModule_ = ::GetModuleHandle(psDllName)) ) {
			//::Msg("Already loaded: %s", psDllName);
			bAlreadyLoaded_ = true;
			return;
		}
		if( !(hModule_ = ::LoadLibrary(psDllName)) ) ::Msg("Error loading: %s", psDllName);
	}
	~Dll()	{ if(!bAlreadyLoaded_) ::FreeLibrary(hModule_); }
};
