/// Copyright 2010-2017 - asm128
#include "cho_platform.h"

#ifndef CHO_BIT_H_034792701397
#define CHO_BIT_H_034792701397

namespace cho
{
	template<typename _tField>		constexpr				_tField		bit_make	(uint8_t bitIndex)											noexcept	{ 
//#if defined(CHO_WINDOWS) && defined(_DEBUG)
//		if(bitIndex >= (sizeof(_tField) * 8), "")
//			OutputDebugString("Out of range bit.");
//#endif
		return 1 << bitIndex; 
	}
	template<typename _tBitField>	inline					_tBitField	bit_set		(_tBitField& inout_FlagVariable, _tBitField in_BitsToSet  )	noexcept	{ return ((inout_FlagVariable & in_BitsToSet) == in_BitsToSet)	? inout_FlagVariable					: inout_FlagVariable |= in_BitsToSet	;}
	template<typename _tBitField>	inline					_tBitField	bit_clear	(_tBitField& inout_FlagVariable, _tBitField in_BitsToClear)	noexcept	{ return (inout_FlagVariable & in_BitsToClear)					? inout_FlagVariable &= ~in_BitsToClear : inout_FlagVariable					;}
	template<typename _tBitField>	inline constexpr		bool		bit_true	(const _tBitField in_FlagVariable, const _tBitField in_Bit)	noexcept	{ return ((in_FlagVariable & in_Bit) == in_Bit) ? true : false; }
	template<typename _tBitField>	inline constexpr		bool		bit_false	(const _tBitField in_FlagVariable, const _tBitField in_Bit)	noexcept	{ return ((in_FlagVariable & in_Bit) != in_Bit) ? true : false; }

#define	gbit_make(bitIndex)								( 1 << (bitIndex) )
#define	gbit_set(inout_FlagVariable, in_BitsToSet  )	( (((inout_FlagVariable) & (in_BitsToSet)) == (in_BitsToSet))	? (inout_FlagVariable)						: (inout_FlagVariable) |= (in_BitsToSet)	)
#define	gbit_clear(inout_FlagVariable, in_BitsToClear)	( ((inout_FlagVariable) & (in_BitsToClear))						? (inout_FlagVariable) &= ~(in_BitsToClear)	: (inout_FlagVariable)						)
#define	gbit_true(in_FlagVariable, in_Bit)				( (((in_FlagVariable) & (in_Bit)) == (in_Bit)) ? true : false )
#define	gbit_false(in_FlagVariable, in_Bit)				( (((in_FlagVariable) & (in_Bit)) != (in_Bit)) ? true : false )
}

#endif // CHO_BIT_H_034792701397
