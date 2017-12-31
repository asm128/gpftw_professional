#include "cho_log.h"
#include "cho_eval.h"

#include <exception>	// for ::std::exception

#ifndef CHO_ARRAY_VIEW_H_2398472395543
#define CHO_ARRAY_VIEW_H_2398472395543

namespace cho
{
	template <typename _tElement>
	class array_view {
	protected:
		// Properties / Member Variables
							_tElement				* Data						= 0;
							uint32_t				Count						= 0;
	public:
		// Constructors
		inline constexpr							array_view					()																			noexcept	= default;
		inline										array_view					(_tElement* dataElements, uint32_t elementCount)										: Data(dataElements), Count(elementCount)										{
			throw_if(0 == dataElements && 0 != elementCount, ::std::exception(""), "Invalid parameters.");	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		template <size_t _elementCount>
		inline constexpr							array_view					(_tElement (&_dataElements)[_elementCount])									noexcept	: Data(_dataElements), Count(_elementCount)										{}

		template <size_t _elementCount>
		inline constexpr							array_view					(_tElement (&_dataElements)[_elementCount], uint32_t elementCount)			noexcept	: Data(_dataElements), Count(::cho::min(_elementCount, elementCount))			{}

		// Operators
							_tElement&				operator[]					(uint32_t index)																		{ throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); throw_if(index >= Count, ::std::exception(""), "Invalid index."); return Data[index]; }
							const _tElement&		operator[]					(uint32_t index)													const				{ throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); throw_if(index >= Count, ::std::exception(""), "Invalid index."); return Data[index]; }

		// Methods
		inline				_tElement*				begin						()																			noexcept	{ return Data;			}
		inline				_tElement*				end							()																			noexcept	{ return Data + Count;	}

		inline constexpr	const _tElement*		begin						()																	const	noexcept	{ return Data;			}
		inline constexpr	const _tElement*		end							()																	const	noexcept	{ return Data + Count;	}

		inline constexpr	uint32_t				size						()																	const	noexcept	{ return Count;			}
	};

	// array_view common typedefs
	typedef				::cho::array_view<char_t			>	view_string			;
	typedef				::cho::array_view<ubyte_t			>	view_ubyte			;
	typedef				::cho::array_view<byte_t			>	view_byte			;
	typedef				::cho::array_view<float				>	view_float32		;
	typedef				::cho::array_view<double			>	view_float64		;
	typedef				::cho::array_view<uint8_t			>	view_uint8			;
	typedef				::cho::array_view<uint16_t			>	view_uint16			;
	typedef				::cho::array_view<uint32_t			>	view_uint32			;
	typedef				::cho::array_view<uint64_t			>	view_uint64			;
	typedef				::cho::array_view<int8_t			>	view_int8			;
	typedef				::cho::array_view<int16_t			>	view_int16			;
	typedef				::cho::array_view<int32_t			>	view_int32			;
	typedef				::cho::array_view<int64_t			>	view_int64			;

	// array_view<const> common typedefs
	typedef				::cho::array_view<const char_t		>	view_const_string	;
	typedef				::cho::array_view<const ubyte_t		>	view_const_ubyte	;
	typedef				::cho::array_view<const byte_t		>	view_const_byte		;
	typedef				::cho::array_view<const float		>	view_const_float32	;
	typedef				::cho::array_view<const double		>	view_const_float64	;
	typedef				::cho::array_view<const uint8_t		>	view_const_uint8	;
	typedef				::cho::array_view<const uint16_t	>	view_const_uint16	;
	typedef				::cho::array_view<const uint32_t	>	view_const_uint32	;
	typedef				::cho::array_view<const uint64_t	>	view_const_uint64	;
	typedef				::cho::array_view<const int8_t		>	view_const_int8		;
	typedef				::cho::array_view<const int16_t		>	view_const_int16	;
	typedef				::cho::array_view<const int32_t		>	view_const_int32	;
	typedef				::cho::array_view<const int64_t		>	view_const_int64	;

}

#endif CHO_ARRAY_VIEW_H_2398472395543
