/// Copyright 2016-2018 - asm128
#include "cho_platform.h"
#if defined(CHO_WINDOWS)
#	include <minwindef.h>
#endif

#ifndef CHO_RUNTIME_H_28903749028374
#define CHO_RUNTIME_H_28903749028374

namespace cho 
{
	struct SRuntimeValues {
#if defined(CHO_WINDOWS)
		::HINSTANCE																		hInstance									= {}; 
		::HINSTANCE																		hPrevInstance								= {}; 
		::LPSTR																			lpCmdLine									= {}; 
		::INT																			nShowCmd									= {}; 
#endif
	};
}

#endif // CHO_RUNTIME_H_28903749028374

