#include "cho_typeint.h"

#ifndef CHO_ERROR_H_91823749823
#define CHO_ERROR_H_91823749823

namespace cho 
{
	typedef						int32_t					error_t;

	static inline constexpr		bool					failed					(const error_t errorCode)		noexcept	{ return errorCode < 0;		}
	static inline constexpr		bool					succeeded				(const error_t errorCode)		noexcept	{ return 0 <= errorCode;	}
}

#if !defined(errored)
#	define						errored(errVal)								(::cho::failed(errVal)) 
#endif
#if !defined(not_errored)
#	define						not_errored(errVal)							(::cho::succeeded(errVal)) 
#endif

#define CHO_CRASH()			{ uint64_t * _tasdas = 0; for(uint32_t i = 0; i < 0xFFFFFFFF; ++i) _tasdas[i] = 0xFFFFFFFF00000000ULL; }	// No throw? Just crash.

#endif // CHO_ERROR_H_91823749823
