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

		inline constexpr	uint32_t								size						()																	const	noexcept	{ return Size.x * Size.y;			}
		inline constexpr	uint32_t								width						()																	const	noexcept	{ return Size.x;					}
		inline constexpr	uint32_t								height						()																	const	noexcept	{ return Size.y;					}
	};

	template<typename _tCell>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src)		{
		for(uint32_t y = 0, yMax = ::cho::min(dst.height(), src.height()); y < yMax; ++y)
		for(uint32_t x = 0, xMax = ::cho::min(dst.width (), src.width ()); x < xMax; ++x) 
			dst[y][x]													= src[y][x];
		return 0;
	}
#pragma pack(pop)
}

#endif // CHO_GRID_VIEW_H_3423423