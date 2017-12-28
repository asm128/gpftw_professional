#include "ftw_platform.h"

#ifndef FTW_EVAL_H_2983742983047239874
#define FTW_EVAL_H_2983742983047239874

#if defined(FTW_WINDOWS) && !defined(NOMINMAX)
#	define NOMINMAX
#endif

namespace ftw 
{
	template<typename _tValue>	static inline constexpr		const _tValue&					min				(const _tValue& a, const _tValue& b)			noexcept	{ return (a < b) ? a : b; }
	template<typename _tValue>	static inline constexpr		const _tValue&					max				(const _tValue& a, const _tValue& b)			noexcept	{ return (a > b) ? a : b; }
} // namespace

#endif // FTW_EVAL_H_2983742983047239874