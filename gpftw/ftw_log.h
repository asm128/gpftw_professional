#include "ftw_error.h"

#if defined(FTW_WINDOWS)
#	include <crtdbg.h>
#	include <Windows.h>
#endif

#include <exception>

#ifndef FTW_LOG_H_892734908273498273498237
#define FTW_LOG_H_892734908273498273498237

#if defined FTW_WINDOWS
#	define base_debug_print(text, charCount)						OutputDebugStringA(text); charCount
#else
#	define base_debug_print(text, charCount)						printf("%s", text); charCount
#endif 

namespace ftw
{
	void															_ftw_print_system_errors			(const char* prefix, uint32_t prefixLen);

	template<const size_t _sizePrefix, typename... TArgs>		
	void															_ftw_debug_printf					(int severity, const char (&prefix)[_sizePrefix], uint32_t prefixLength, const char* format, const TArgs... args)			{
		base_debug_print(prefix, prefixLength);
		char																customDynamicString	[8192]			= {0};
		const size_t														stringLength						= sprintf_s(customDynamicString, format, args...);
		base_debug_print(customDynamicString, (int)stringLength);
		base_debug_print("\n", 1);
		if(2 < severity)
			_ftw_print_system_errors(prefix, prefixLength);
	}
}

#define debug_printf(severity, severityStr, format, ...)																																	\
	do { 																																													\
		static constexpr const char												prefixFormat	[]							= ":%u:" severityStr ":" __FILE__ "(%u){" __FUNCTION__ "}:";	\
		static char																prefixString	[sizeof(prefixFormat) + 8]	= {}; 															\
		static const int 														prefixLength								= ::sprintf_s(prefixString, prefixFormat, severity, __LINE__);	\
		::ftw::_ftw_debug_printf(severity, prefixString, prefixLength == -1 ? 0 : prefixLength, format, __VA_ARGS__);																		\
	} while(0)

#if !defined FTW_USE_DEBUG_BREAK_ON_ERROR_LOG
#	define FTW_USE_DEBUG_BREAK_ON_ERROR_LOG
#endif 

#if !defined FTW_USE_DEBUG_BREAK_ON_ERROR_LOG
#	define FTW_CRT_DEBUG_BREAK 
#else 
#	define FTW_CRT_DEBUG_BREAK _CrtDbgBreak
#endif 

#define error_printf(format, ...)								do { debug_printf(0, "error"	, format, __VA_ARGS__); FTW_CRT_DEBUG_BREAK(); } while(0)
#define warning_printf(format, ...)									 debug_printf(1, "warning"	, format, __VA_ARGS__)
#define info_printf(format, ...)									 debug_printf(2, "info"		, format, __VA_ARGS__)

#define throw_if(condition, exception, format, ...)				if(condition) { base_debug_print(#condition, -1); error_printf	(format, __VA_ARGS__); throw(exception);	}
#define error_if(condition, format, ...)						if(condition) { base_debug_print(#condition, -1); error_printf	(format, __VA_ARGS__);						}
#define warn_if(condition, format, ...)							if(condition) { base_debug_print(#condition, -1); warning_printf(format, __VA_ARGS__);						}
#define info_if(condition, format, ...)							if(condition) { info_printf		(format, __VA_ARGS__); }

#define ret_error_if(condition, format, ...)					if(condition) { error_printf	(format, __VA_ARGS__); return;			}
#define ret_warn_if(condition, format, ...)						if(condition) { warning_printf	(format, __VA_ARGS__); return;			}
#define ret_info_if(condition, format, ...)						if(condition) { info_printf		(format, __VA_ARGS__); return;			}

#define break_error_if(condition, format, ...)					if(condition) { error_printf	(format, __VA_ARGS__); break;			}
#define break_warn_if(condition, format, ...)					if(condition) { warning_printf	(format, __VA_ARGS__); break;			}
#define break_info_if(condition, format, ...)					if(condition) { info_printf		(format, __VA_ARGS__); break;			}

#define continue_error_if(condition, format, ...)				if(condition) { error_printf	(format, __VA_ARGS__); continue;		}
#define continue_warn_if(condition, format, ...)				if(condition) { warning_printf	(format, __VA_ARGS__); continue;		}
#define continue_info_if(condition, format, ...)				if(condition) { info_printf		(format, __VA_ARGS__); continue;		}

#define retval_error_if(retVal, condition, format, ...)			if(condition) { error_printf	(format, __VA_ARGS__); return retVal;	}
#define retval_warn_if(retVal, condition, format, ...)			if(condition) { warning_printf	(format, __VA_ARGS__); return retVal;	}
#define retval_info_if(retVal, condition, format, ...)			if(condition) { info_printf		(format, __VA_ARGS__); return retVal;	}

#define retnul_error_if(condition, format, ...)					retval_error_if	(0, condition, format, __VA_ARGS__)
#define retnul_warn_if(condition, format, ...)					retval_warn_if	(0, condition, format, __VA_ARGS__)
#define retnul_info_if(condition, format, ...)					retval_info_if	(0, condition, format, __VA_ARGS__)

#define reterr_error_if(condition, format, ...)					retval_error_if	(-1, condition, format, __VA_ARGS__)
#define reterr_warn_if(condition, format, ...)					retval_warn_if	(-1, condition, format, __VA_ARGS__)
#define reterr_info_if(condition, format, ...)					retval_info_if	(-1, condition, format, __VA_ARGS__)

#define retwarn_error_if(condition, format, ...)				retval_error_if	(1, condition, format, __VA_ARGS__)
#define retwarn_warn_if(condition, format, ...)					retval_warn_if	(1, condition, format, __VA_ARGS__)
#define retwarn_info_if(condition, format, ...)					retval_info_if	(1, condition, format, __VA_ARGS__)

#define re_if													ret_error_if
#define rw_if													ret_warn_if
#define ri_if													ret_info_if

#define be_if													break_error_if
#define bw_if													break_warn_if
#define bi_if													break_info_if

#define ce_if													continue_error_if
#define cw_if													continue_warn_if
#define ci_if													continue_info_if

#define rve_if													retval_error_if

#define rne_if													retnul_error_if
#define rnw_if													retnul_warn_if 
#define rni_if													retnul_info_if

#define ree_if													reterr_error_if
#define rew_if													reterr_warn_if 
#define rei_if													reterr_info_if 

#define rwe_if													retwarn_error_if
#define rww_if													retwarn_warn_if 
#define rwi_if													retwarn_info_if 

#endif // FTW_LOG_H_892734908273498273498237
