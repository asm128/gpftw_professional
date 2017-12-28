#include "ftw_typeint.h"

#ifndef FTW_ERROR_H_91823749823
#define FTW_ERROR_H_91823749823

namespace ftw 
{
	typedef					int32_t					error_t;

	inline constexpr		bool					failed					(error_t errorCode)					{ return errorCode < 0; }
}

#endif // FTW_ERROR_H_91823749823
