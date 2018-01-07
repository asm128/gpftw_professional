#include "cho_error.h"
#include "cho_debug.h"
#include "cho_size.h"

#include <cstdio>

#if defined(CHO_WINDOWS)
#	include <crtdbg.h>
#	include <Windows.h>
#endif

#ifndef CHO_LOG_H_8927349654687654365
#define CHO_LOG_H_8927349654687654365

#if defined CHO_WINDOWS
#	define base_debug_print(text, charCount)						OutputDebugStringA(text); charCount
#else
#	define base_debug_print(text, charCount)						printf("%s", text); charCount
#endif 

namespace cho
{
	void															_cho_print_system_errors			(const char* prefix, uint32_t prefixLen);

	template<const size_t _sizePrefix, typename... TArgs>		
	void															_cho_debug_printf					(int severity, const char (&prefix)[_sizePrefix], uint32_t prefixLength, const char* format, const TArgs... args)			{
		base_debug_print(prefix, prefixLength);
		char																customDynamicString	[8192]			= {0};
		const size_t														stringLength						= sprintf_s(customDynamicString, format, args...);
		base_debug_print(customDynamicString, (int)stringLength);
		base_debug_print("\n", 1);
		if(2 >= severity)
			::cho::_cho_print_system_errors(prefix, prefixLength);
	}
}

#define debug_printf(severity, severityStr, format, ...)																																	\
	do { 																																													\
		static constexpr const char											prefixFormat	[]								= ":%u:" severityStr ":" __FILE__ "(%u){" __FUNCTION__ "}:";	\
		static char															prefixString	[::cho::size(prefixFormat) + 8]	= {}; 															\
		static const int 													prefixLength									= ::sprintf_s(prefixString, prefixFormat, severity, __LINE__);	\
		::cho::_cho_debug_printf(severity, prefixString, prefixLength == -1 ? 0 : prefixLength, format, __VA_ARGS__);																		\
	} while(0)

#if defined (CHO_ERROR_PRINTF_ENABLED)
#	define error_printf(format, ...)								do { debug_printf(1, "error"	, format, __VA_ARGS__); CHO_PLATFORM_CRT_BREAKPOINT(); } while(0)
#else
#	define error_printf(format, ...)								do { __VA_ARGS__; CHO_PLATFORM_CRT_BREAKPOINT(); } while(0)
#endif	

#if defined (CHO_WARNING_PRINTF_ENABLED)
#	define warning_printf(format, ...)								debug_printf(2, "warning"	, format, __VA_ARGS__)
#else
#	define warning_printf(format, ...)								do { __VA_ARGS__; } while(0)
#endif	

#if defined (CHO_INFO_PRINTF_ENABLED)
#	define info_printf(format, ...)									debug_printf(3, "info"		, format, __VA_ARGS__)
#else
#	define info_printf(format, ...)									do { __VA_ARGS__; } while(0)
#endif	

#if defined (CHO_SUCCESS_PRINTF_ENABLED)
#	define success_printf(format, ...)								debug_printf(4, "info"		, format, __VA_ARGS__)
#else
#	define success_printf(format, ...)								do { __VA_ARGS__; } while(0)
#endif	

#define throw_if(condition, exception, format, ...)				if(condition) { error_printf	(format, __VA_ARGS__); base_debug_print("Condition: " #condition, -1); throw(exception);	}
#define error_if(condition, format, ...)						if(condition) { error_printf	(format, __VA_ARGS__); base_debug_print("Condition: " #condition, -1); 						}
#define warn_if(condition, format, ...)							if(condition) { warning_printf	(format, __VA_ARGS__); base_debug_print("Condition: " #condition, -1); 						}
#define info_if(condition, format, ...)							if(condition) { info_printf		(format, __VA_ARGS__); base_debug_print("Condition: " #condition, -1); 						}

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

#define retnul_error_if(condition, format, ...)					retval_error_if	( 0, condition, format, __VA_ARGS__)
#define retnul_warn_if(condition, format, ...)					retval_warn_if	( 0, condition, format, __VA_ARGS__)
#define retnul_info_if(condition, format, ...)					retval_info_if	( 0, condition, format, __VA_ARGS__)

#define reterr_error_if(condition, format, ...)					retval_error_if	(-1, condition, format, __VA_ARGS__)
#define reterr_warn_if(condition, format, ...)					retval_warn_if	(-1, condition, format, __VA_ARGS__)
#define reterr_info_if(condition, format, ...)					retval_info_if	(-1, condition, format, __VA_ARGS__)

#define retwarn_error_if(condition, format, ...)				retval_error_if	( 1, condition, format, __VA_ARGS__)
#define retwarn_warn_if(condition, format, ...)					retval_warn_if	( 1, condition, format, __VA_ARGS__)
#define retwarn_info_if(condition, format, ...)					retval_info_if	( 1, condition, format, __VA_ARGS__)

#if defined (CHO_ERROR_PRINTF_ENABLED)
// Non-propagable retval_error call.
#	define cho_rve_ecall(retVal, chol_call, format, ...) do {																														\
		::cho::error_t cho_errCall_ = (chol_call);  																																\
		if(cho_errCall_ < 0) {																																						\
			debug_printf(0, "error", "%s: 0x%X.", #chol_call, cho_errCall_);																										\
			error_printf(format, __VA_ARGS__); 																																		\
			return retVal; 																																							\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #chol_call, cho_errCall_);																										\
		}																																											\
	} while(0)

// Non-propagable retval_error error-warning call.
#	define cho_rve_ewcall(retVal, chol_call, format, ...) do {																														\
		if(::cho::error_t cho_errCall_ = (chol_call)) { 																															\
			if(cho_errCall_ < 0) {																																					\
				debug_printf(0, "error", "%s: 0x%X.", #chol_call, cho_errCall_);																									\
				error_printf(format, __VA_ARGS__); 																																	\
				return retval; 																																						\
			}																																										\
			else {																																									\
				warning_printf("%s: 0x%X.", #chol_call, cho_errCall_);																												\
			}																																										\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #chol_call, cho_errCall_);																										\
		}																																											\
	} while(0)

//
#	define cho_rv_hrcall(retVal, hr_call) do {																																		\
		::HRESULT errCall_ = (hr_call);  																																			\
		if FAILED(errCall_) {																																						\
			debug_printf(0, "error", "%s: (0x%X) : '%s'.", #hr_call, errCall_, ::cho::getWindowsErrorAsString(errCall_).c_str());													\
			return retVal; 																																							\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #hr_call, errCall_);																												\
		}																																											\
	} while(0)

//
#	define cho_rve_hrcall(retVal, hr_call, format, ...) do {																														\
		::HRESULT errCall_ = (hr_call);  																																			\
		if FAILED(errCall_) {																																						\
			debug_printf(0, "error", "%s: (0x%X) : '%s' - " format, #hr_call, errCall_, ::cho::getWindowsErrorAsString(errCall_).c_str(), __VA_ARGS__);								\
			return retVal; 																																							\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #hr_call, errCall_);																												\
		}																																											\
	} while(0)

// --------------------------------------------------------------------
// Propagable retval_error call.
#	define cho_pecall(chol_call, ...) do {																																			\
		::cho::error_t cho_errCall_ = (chol_call);  																																\
		if(cho_errCall_ < 0) {																																						\
			debug_printf(0, "error", "%s: 0x%X", #chol_call, cho_errCall_);																											\
			error_printf(__VA_ARGS__); 																																				\
			return cho_errCall_; 																																					\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #chol_call, cho_errCall_);																										\
		}																																											\
	} while(0)

// Propagable retval_error error-warning call.
#	define cho_pewcall(chol_call, ...) do {																																			\
		if(::cho::error_t cho_errCall_ = (chol_call)) { 																															\
			if(cho_errCall_ < 0) {																																					\
				debug_printf(0, "error", "%s: 0x%X", #chol_call, cho_errCall_);																										\
				error_printf(__VA_ARGS__); 																																			\
				return cho_errCall_; 																																				\
			}																																										\
			else {																																									\
				warning_printf("%s: 0x%X.", #chol_call, cho_errCall_);																												\
			}																																										\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #chol_call, cho_errCall_);																										\
		}																																											\
	} while(0)

#else
#	define cho_rve_ecall(retVal, chol_call, ...) do {																																\
		if(::cho::failed(chol_call))  																																				\
			return retval; 																																							\
	} while(0)

#	define cho_rve_hrcall(retVal, chol_call, ...) do {																																\
		if(FAILED(chol_call))  																																						\
			return retval; 																																							\
	} while(0)

#	define cho_rv_hrcall(retVal, chol_call) do {																																	\
		if(FAILED(chol_call))  																																						\
			return retval; 																																							\
	} while(0)

#	define cho_pecall(chol_call, ...) do {																																			\
		::cho::error_t cho_errCall_ = (chol_call);																																	\
		if(::cho::failed(cho_errCall_)) 																																			\
			return cho_errCall_; 																																					\
	} while(0)

#	define cho_rve_ewcall											cho_rve_ecall	// Non-propagable retval_error error-warning call.
#	define cho_pewcall												cho_pecall			// Propagable retval_error error-warning call.
#endif

#define cho_necall(chol_call, ...)								cho_rve_ecall (-1, chol_call, __VA_ARGS__)	// Non-propagable error call.
#define cho_newcall(chol_call, ...)								cho_rve_ewcall(-1, chol_call, __VA_ARGS__)	// Non-propagable error-warning call.
#define cho_hrcall(hr_call)										cho_rv_hrcall (-1, hr_call)					// HRESULT call.
#define cho_hrecall(hr_call, ...)								cho_rve_hrcall(-1, hr_call, __VA_ARGS__)	// HRESULT call.

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

#endif // CHO_LOG_H_8927349654687654365
