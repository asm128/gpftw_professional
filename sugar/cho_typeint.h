#include "cho_platform.h"

#include <cstdint>

#ifndef CHO_TYPEINT_H_29873490827342
#define CHO_TYPEINT_H_29873490827342

namespace cho
{
	typedef signed char						schar_t	, sbyte_t	;
	typedef char							char_t	, byte_t	;
	typedef unsigned char					uchar_t	, ubyte_t	;
#if defined CHO_WINDOWS
#	define	REFCOUNT_T						long
	typedef volatile REFCOUNT_T				refcount_t			;
#else
#	define	REFCOUNT_T						intptr_t
	typedef REFCOUNT_T						refcount_t			;
#endif
}

#endif // CHO_TYPEINT_H_29873490827342
