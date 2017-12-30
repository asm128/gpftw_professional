#include "cho_platform.h"
#if defined(FTW_WINDOWS)
#	include <minwindef.h>
#endif

#ifndef FTW_RUNTIME_H_28903749028374
#define FTW_RUNTIME_H_28903749028374

namespace cho 
{
	struct SRuntimeValues {
#if defined(FTW_WINDOWS)
		::HINSTANCE																		hInstance									= {}; 
		::HINSTANCE																		hPrevInstance								= {}; 
		::LPSTR																			lpCmdLine									= {}; 
		::INT																			nShowCmd									= {}; 
#endif
	};
}

#endif // FTW_RUNTIME_H_28903749028374

