/// Copyright 2016-2018 - asm128
#include "cho_platform.h"
#if defined(CHO_WINDOWS)
#	include <minwindef.h>
#endif

#ifndef CHO_RUNTIME_H_28903749028374
#define CHO_RUNTIME_H_28903749028374

namespace cho 
{
#if defined(CHO_WINDOWS)
	struct SWindowsEntryPointArgs {
				HINSTANCE														hInstance							;
				HINSTANCE														hPrevInstance						;
				LPSTR															lpCmdLine							;
				int																nShowCmd							;
	};
#endif

	struct SRuntimeValuesDetail {	
#if defined(CHO_ANDROID)
				ANativeActivity													* Activity							= nullptr;
				void															* SavedState						= nullptr;
				size_t															SavedStateSize						= 0;
#elif defined(CHO_WINDOWS)
				SWindowsEntryPointArgs											EntryPointArgs						;
#else
#	error "Not implemented."
#endif
	};

	struct SRuntimeValues {
		SRuntimeValuesDetail													PlatformDetail						;
	};
}

#endif // CHO_RUNTIME_H_28903749028374

