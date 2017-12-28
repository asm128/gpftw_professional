#include "ftw_error.h"

#ifndef FTW_LOG_H_892734908273498273498237
#define FTW_LOG_H_892734908273498273498237

#if defined FTW_WINDOWS
#	define base_debug_printf(text, charCount)					OutputDebugString(text); charCount
#else
#	define base_debug_printf(text, charCount)					printf("%s", text); charCount
#endif 

namespace ftw
{
	template<const size_t _sizePrefix, typename... TArgs>	
	static	void									_ftw_debug_printf					(int severity, const char* severityStr, const char (&prefix)[_sizePrefix], const char* format, const TArgs... args)			{
		base_debug_printf(prefix, _sizePrefix-1);
		char											customDynamicString	[4096]			= {0};
		const size_t									stringLength						= sprintf_s(customDynamicString, format, args...);
		base_debug_printf(customDynamicString, (int)stringLength);
		base_debug_printf("\n", 1);
		if(2 < severity)
			_ftw_print_system_errors(prefix);
	}
}

#define debug_printf(severity, severityStr, format, ...)																										\
	do { 																																						\
		static constexpr const char									infoFormat[]							= ":" severityStr ":" __FILE__ "(%u): ";			\
		static char													infoString[512]							= {}; 												\
		static const int 											formattingResult						= ::sprintf_s(infoString, infoFormat, __LINE__);	\
		base_debug_printf(infoString); 																															\
		char														userString[4096]						= {}; 												\
		::ftw::_ftw_debug_printf(severityStr, format, __VA_ARGS__);																								\
	} while(0)

#define error_printf(format, ...)								debug_printf(0, "error"		, format, __VA_ARGS__)
#define warning_printf(format, ...)								debug_printf(1, "warning"	, format, __VA_ARGS__)
#define info_printf(format, ...)								debug_printf(2, "info"		, format, __VA_ARGS__)

#define error_if(condition, format, ...)						if(condition) error_printf	(format, __VA_ARGS__)
#define warn_if(condition, format, ...)							if(condition) warning_printf(format, __VA_ARGS__)
#define info_if(condition, format, ...)							if(condition) info_printf	(format, __VA_ARGS__)

#define ret_error_if(condition, format, ...)					if(condition) { error_printf	(format, __VA_ARGS__); return; }
#define ret_warn_if(condition, format, ...)						if(condition) { warning_printf	(format, __VA_ARGS__); return; }
#define ret_info_if(condition, format, ...)						if(condition) { info_printf		(format, __VA_ARGS__); return; }

#define break_error_if(condition, format, ...)					if(condition) { error_printf	(format, __VA_ARGS__); break; }
#define break_warn_if(condition, format, ...)					if(condition) { warning_printf	(format, __VA_ARGS__); break; }
#define break_info_if(condition, format, ...)					if(condition) { info_printf		(format, __VA_ARGS__); break; }

#define continue_error_if(condition, format, ...)				if(condition) { error_printf	(format, __VA_ARGS__); continue; }
#define continue_warn_if(condition, format, ...)				if(condition) { warning_printf	(format, __VA_ARGS__); continue; }
#define continue_info_if(condition, format, ...)				if(condition) { info_printf		(format, __VA_ARGS__); continue; }

#define retval_error_if(retVal, condition, format, ...)			if(condition) { error_printf(format, __VA_ARGS__); return retVal; }

#define retwarn_error_if(condition, format, ...)				retval_error_if(1, condition, format, __VA_ARGS__)
#define retwarn_warn_if(condition, format, ...)					retval_error_if(1, condition, format, __VA_ARGS__)
#define retwarn_info_if(condition, format, ...)					retval_error_if(1, condition, format, __VA_ARGS__)

#define retwarn_error_if(condition, format, ...)				retval_error_if(1, condition, format, __VA_ARGS__)
#define retwarn_warn_if(condition, format, ...)					retval_error_if(1, condition, format, __VA_ARGS__)
#define retwarn_info_if(condition, format, ...)					retval_error_if(1, condition, format, __VA_ARGS__)

#endif // FTW_LOG_H_892734908273498273498237
