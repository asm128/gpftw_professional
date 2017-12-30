#include "ftw_error.h"

#if defined(FTW_WINDOWS)
#	include <crtdbg.h>
#	include <Windows.h>
#endif

#include <exception>

#ifndef FTW_LOG_H_8927349654687654365
#define FTW_LOG_H_8927349654687654365

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
		if(2 <= severity)
			_ftw_print_system_errors(prefix, prefixLength);
	}
}

#define debug_printf(severity, severityStr, format, ...)																																\
	do { 																																												\
		static constexpr const char											prefixFormat	[]							= ":%u:" severityStr ":" __FILE__ "(%u){" __FUNCTION__ "}:";	\
		static char															prefixString	[sizeof(prefixFormat) + 8]	= {}; 															\
		static const int 													prefixLength								= ::sprintf_s(prefixString, prefixFormat, severity, __LINE__);	\
		::ftw::_ftw_debug_printf(severity, prefixString, prefixLength == -1 ? 0 : prefixLength, format, __VA_ARGS__);																	\
	} while(0)

#if !defined FTW_USE_DEBUG_BREAK_ON_ERROR_LOG
#	define FTW_USE_DEBUG_BREAK_ON_ERROR_LOG
#endif 

#if !defined FTW_USE_DEBUG_BREAK_ON_ERROR_LOG
#	define FTW_CRT_DEBUG_BREAK 
#else 
#	define FTW_CRT_DEBUG_BREAK _CrtDbgBreak
#endif 

#define FTW_ERROR_PRINTF_ENABLED
#define FTW_WARNING_PRINTF_ENABLED
#define FTW_INFO_PRINTF_ENABLED		

#if defined (FTW_ERROR_PRINTF_ENABLED)
#	define error_printf(format, ...)								do { debug_printf(1, "error"	, format, __VA_ARGS__); FTW_CRT_DEBUG_BREAK(); } while(0)
#else
#	define error_printf(format, ...)								do { __VA_ARGS__; FTW_CRT_DEBUG_BREAK(); } while(0)
#endif	

#if defined (FTW_WARNING_PRINTF_ENABLED)
#	define warning_printf(format, ...)								debug_printf(2, "warning"	, format, __VA_ARGS__)
#else
#	define warning_printf(format, ...)								do { __VA_ARGS__; } while(0)
#endif	

#if defined (FTW_INFO_PRINTF_ENABLED)
#	define info_printf(format, ...)									debug_printf(3, "info"		, format, __VA_ARGS__)
#else
#	define info_printf(format, ...)									do { __VA_ARGS__; } while(0)
#endif	

#if defined (FTW_SUCCESS_PRINTF_ENABLED)
#	define success_printf(format, ...)								debug_printf(4, "info"		, format, __VA_ARGS__)
#else
#	define success_printf(format, ...)								do { __VA_ARGS__; } while(0)
#endif	

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

#if defined (FTW_ERROR_PRINTF_ENABLED)
// Non-propagable retval_error call.
#	define ftw_rve_ecall(retVal, nwo_call, format, ...) do {																														\
		::ftw::error_t errCall = (nwo_call);  																																		\
		if(errCall < 0) {																																							\
			debug_printf(0, "error", "%s: 0x%X.", #nwo_call, errCall);																												\
			error_printf(format, __VA_ARGS__); 																																		\
			return retVal; 																																							\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #nwo_call, errCall);																												\
		}																																											\
	} while(0)

// Non-propagable retval_error error-warning call.
#	define ftw_rve_ewcall(retVal, nwo_call, format, ...) do {																														\
		if(::ftw::error_t errCall = (nwo_call)) { 																																	\
			if(errCall < 0) {																																						\
				debug_printf(0, "error", "%s: 0x%X.", #nwo_call, errCall);																											\
				error_printf(format, __VA_ARGS__); 																																	\
				return retval; 																																						\
			}																																										\
			else {																																									\
				warning_printf("%s: 0x%X.", #nwo_call, errCall);																													\
			}																																										\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #nwo_call, errCall);																												\
		}																																											\
	} while(0)

//
#	define ftw_rv_hrcall(retVal, hr_call) do {																																		\
		::HRESULT errCall_ = (hr_call);  																																			\
		if FAILED(errCall_) {																																						\
			debug_printf(0, "error", "%s: (0x%X) : '%s'.", #hr_call, errCall_, ::ftw::getWindowsErrorAsString(errCall_).c_str());													\
			return retVal; 																																							\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #hr_call, errCall_);																												\
		}																																											\
	} while(0)

//
#	define ftw_rve_hrcall(retVal, hr_call, format, ...) do {																														\
		::HRESULT errCall_ = (hr_call);  																																			\
		if FAILED(errCall_) {																																						\
			debug_printf(0, "error", "%s: (0x%X) : '%s' - " format, #hr_call, errCall_, ::ftw::getWindowsErrorAsString(errCall_).c_str(), __VA_ARGS__);								\
			return retVal; 																																							\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #hr_call, errCall_);																												\
		}																																											\
	} while(0)

// --------------------------------------------------------------------
// Propagable retval_error call.
#	define ftw_pecall(nwo_call, ...) do {																																			\
		::ftw::error_t errCall = (nwo_call);  																																		\
		if(errCall < 0) {																																							\
			debug_printf(0, "error", "%s: 0x%X", #nwo_call, errCall);																												\
			error_printf(__VA_ARGS__); 																																				\
			return errCall; 																																						\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #nwo_call, errCall);																												\
		}																																											\
	} while(0)
// Propagable retval_error error-warning call.
#	define ftw_pewcall(nwo_call, ...) do {																																			\
		if(::ftw::error_t errCall = (nwo_call)) { 																																	\
			if(errCall < 0) {																																						\
				debug_printf(0, "error", "%s: 0x%X", #nwo_call, errCall);																											\
				error_printf(__VA_ARGS__); 																																			\
				return errCall; 																																					\
			}																																										\
			else {																																									\
				warning_printf("%s: 0x%X.", #nwo_call, errCall);																													\
			}																																										\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #nwo_call, errCall);																												\
		}																																											\
	} while(0)

#else
#	define ftw_rve_ecall(retVal, nwo_call, ...) do {																																\
		if(::ftw::failed(nwo_call))  																																				\
			return retval; 																																							\
	} while(0)

#	define ftw_pecall(retVal, nwo_call, ...)  do {																																	\
		::ftw::error_t _ftw_errCall = ::ftw::succeeded(nwo_call);																													\
		if(::ftw::failed(_ftw_errCall)) 																																			\
			return retval; 																																							\
	} while(0)

#	define ftw_rve_ewcall											ftw_rve_ecall	// Non-propagable retval_error error-warning call.
#	define ftw_pewcall												ftw_pecall			// Propagable retval_error error-warning call.
#endif

#define ftw_necall(ftwl_call, ...)								ftw_rve_ecall (-1, ftwl_call, __VA_ARGS__)	// Non-propagable error call.
#define ftw_newcall(ftwl_call, ...)								ftw_rve_ewcall(-1, ftwl_call, __VA_ARGS__)	// Non-propagable error-warning call.
#define ftw_hrcall(hr_call)										ftw_rv_hrcall (-1, hr_call)					// HRESULT call.
#define ftw_hrecall(hr_call, ...)								ftw_rve_hrcall(-1, hr_call, __VA_ARGS__)	// HRESULT call.

#define e_if													error_if
#define w_if													warn_if
#define i_if													info_if

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

#endif // FTW_LOG_H_8927349654687654365
