#include "cho_array_view.h"		// for cho::array_view<>
#include "cho_coord.h"

#ifndef CHO_GRID_VIEW_H_3423423
#define CHO_GRID_VIEW_H_3423423

namespace cho
{
#pragma pack(push, 1)
	template <typename _tElement>
	class grid_view {
	protected:
		// Properties / Member Variables
							_tElement								* Data						= 0;
							::cho::SCoord2<uint32_t>				Size						= {0, 0};
	public:
		typedef				_tElement								TElement;
		// Constructors
		inline constexpr											grid_view					()																			noexcept	= default;
		inline														grid_view					(_tElement* dataElements, uint32_t gridWidth, uint32_t gridHeight)						: Data{dataElements}, Size{gridWidth, gridHeight}	{
			throw_if(0 == dataElements && 0 != Size.x && 0 != Size.y, ::std::exception(""), "Invalid parameters.");	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		// Operators
							::cho::array_view<_tElement>			operator[]					(uint32_t row)																			{ throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); throw_if(row >= Size.y, ::std::exception(""), "Invalid row."); return ::cho::array_view<_tElement			>(&Data[row * Size.x], Size.x); }
							::cho::array_view<const _tElement>		operator[]					(uint32_t row)														const				{ throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); throw_if(row >= Size.y, ::std::exception(""), "Invalid row."); return ::cho::array_view<const _tElement	>(&Data[row * Size.x], Size.x); }

		// Methods
		inline constexpr	const _tElement*						begin						()																	const	noexcept	{ return Data;						}
		inline constexpr	const _tElement*						end							()																	const	noexcept	{ return Data + size();				}

		inline constexpr	_tElement*								begin						()																			noexcept	{ return Data;						}
		inline constexpr	_tElement*								end							()																			noexcept	{ return Data + size();				}

		inline constexpr	::cho::SCoord2<uint32_t>				metrics						()																	const	noexcept	{ return {Size,x. Size.y};			}
		inline constexpr	uint32_t								size						()																	const	noexcept	{ return Size.x * Size.y;			}
		inline constexpr	uint32_t								width						()																	const	noexcept	{ return Size.x;					}
		inline constexpr	uint32_t								height						()																	const	noexcept	{ return Size.y;					}
	};
#pragma pack(pop)

	// array_view common typedefs
	typedef				::cho::grid_view<char_t				>	view2d_char				;
	typedef				::cho::grid_view<ubyte_t			>	view2d_ubyte			;
	typedef				::cho::grid_view<byte_t				>	view2d_byte				;
	typedef				::cho::grid_view<float				>	view2d_float32			;
	typedef				::cho::grid_view<double				>	view2d_float64			;
	typedef				::cho::grid_view<uint8_t			>	view2d_uint8			;
	typedef				::cho::grid_view<uint16_t			>	view2d_uint16			;
	typedef				::cho::grid_view<uint32_t			>	view2d_uint32			;
	typedef				::cho::grid_view<uint64_t			>	view2d_uint64			;
	typedef				::cho::grid_view<int8_t				>	view2d_int8				;
	typedef				::cho::grid_view<int16_t			>	view2d_int16			;
	typedef				::cho::grid_view<int32_t			>	view2d_int32			;
	typedef				::cho::grid_view<int64_t			>	view2d_int64			;

	// array_view<const> common typedefs
	typedef				::cho::grid_view<const char_t		>	view2d_const_char		;
	typedef				::cho::grid_view<const ubyte_t		>	view2d_const_ubyte		;
	typedef				::cho::grid_view<const byte_t		>	view2d_const_byte		;
	typedef				::cho::grid_view<const float		>	view2d_const_float32	;
	typedef				::cho::grid_view<const double		>	view2d_const_float64	;
	typedef				::cho::grid_view<const uint8_t		>	view2d_const_uint8		;
	typedef				::cho::grid_view<const uint16_t		>	view2d_const_uint16		;
	typedef				::cho::grid_view<const uint32_t		>	view2d_const_uint32		;
	typedef				::cho::grid_view<const uint64_t		>	view2d_const_uint64		;
	typedef				::cho::grid_view<const int8_t		>	view2d_const_int8		;
	typedef				::cho::grid_view<const int16_t		>	view2d_const_int16		;
	typedef				::cho::grid_view<const int32_t		>	view2d_const_int32		;
	typedef				::cho::grid_view<const int64_t		>	view2d_const_int64		;

}

#endif // CHO_GRID_VIEW_H_3423423
