/// Copyright 2010-2018 - asm128
#include "cho_platform.h"

#ifndef CHO_DEBUG_H_97826389432654
#define CHO_DEBUG_H_97826389432654

#if defined(CHO_DEBUG_ENABLED)
#	if defined(CHO_ANDROID) || defined(CHO_LINUX)
#		define CHO_PLATFORM_CRT_BREAKPOINT()	do {} while(0)
#		define CHO_PLATFORM_CRT_CHECK_MEMORY()	do {} while(0)
#	elif defined(CHO_WINDOWS)
#		include <crtdbg.h>
#		define CHO_PLATFORM_CRT_BREAKPOINT		(void)_CrtDbgBreak
#		define CHO_PLATFORM_CRT_CHECK_MEMORY()	// (void)_CrtCheckMemory
#	else
#		error "Platform not supported."
#	endif
#	define CHO_ERROR_PRINTF_ENABLED	
#	define CHO_WARNING_PRINTF_ENABLED	
#	define CHO_INFO_PRINTF_ENABLED	
//#	define CHO_SUCCESS_PRINTF_ENABLED	// Uncomment as needed
#	ifndef CHO_DISABLE_DEBUG_BREAK_ON_ERROR_LOG
#		define CHO_USE_DEBUG_BREAK_ON_ERROR_LOG
#	endif
#else
#	define CHO_PLATFORM_CRT_BREAKPOINT()		do {} while(0)
#	define CHO_PLATFORM_CRT_CHECK_MEMORY()		do {} while(0)
#	define CHO_ERROR_PRINTF_ENABLED	
//#	define CHO_WARNING_PRINTF_ENABLED	// Uncomment as needed
//#	define CHO_INFO_PRINTF_ENABLED		// Uncomment as needed
//#	define CHO_SUCCESS_PRINTF_ENABLED	// Uncomment as needed
#endif

#endif // CHO_DEBUG_H_97826389432654
