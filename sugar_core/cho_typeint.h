#include "cho_platform.h"

#include <cstdint>

#ifndef CHO_TYPEINT_H_29873490827342
#define CHO_TYPEINT_H_29873490827342

typedef signed char						schar_t	, sbyte_t	;
typedef char							char_t	, byte_t	;
typedef unsigned char					uchar_t	, ubyte_t	;

namespace cho
{
#if defined CHO_WINDOWS
#	if defined(_WIN64) || defined(WIN64)
#		define	REFCOUNT_T						long long
		typedef volatile REFCOUNT_T				refcount_t			;
#	else
#		define	REFCOUNT_T						long
		typedef volatile REFCOUNT_T				refcount_t			;
#	endif
#else
#	define	REFCOUNT_T						intptr_t
	typedef REFCOUNT_T						refcount_t			;
#endif
}

#endif // CHO_TYPEINT_H_29873490827342
