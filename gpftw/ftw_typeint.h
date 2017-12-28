#include "ftw_platform.h"
#include <cstdint>

#ifndef FTW_TYPEINT_H_29873490827342
#define FTW_TYPEINT_H_29873490827342

namespace ftw
{
	typedef signed char						schar_t	, sbyte_t	;
	typedef char							char_t	, byte_t	;
	typedef unsigned char					uchar_t	, ubyte_t	;
#if defined FTW_WINDOWS
#	define	REFCOUNT_T long
#else
#	define	REFCOUNT_T intptr_t
#endif
	typedef volatile REFCOUNT_T				refcount_t			;
}

#endif // FTW_TYPEINT_H_29873490827342
