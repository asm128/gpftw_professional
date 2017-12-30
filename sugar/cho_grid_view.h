#include "cho_array_view.h"		// for cho::array_view<>

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
							uint32_t								Width						= 0;
							uint32_t								Height						= 0;
	public:
		// Constructors
		inline constexpr											grid_view					()																			= default;
		inline														grid_view					(_tElement* dataElements, uint32_t gridWidth, uint32_t gridHeight)			: Data(dataElements), Width(gridWidth), Height(gridHeight)	{
			throw_if(0 == dataElements && 0 != Width && 0 != Height, ::std::exception(""), "Invalid parameters.");	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		// Operators
							::cho::array_view<_tElement>			operator[]					(uint32_t row)																{ throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); throw_if(row >= Height, ::std::exception(""), "Invalid row."); return ::cho::array_view<_tElement			>(&Data[row*Width], Width); }
							::cho::array_view<const _tElement>		operator[]					(uint32_t row)														const	{ throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); throw_if(row >= Height, ::std::exception(""), "Invalid row."); return ::cho::array_view<const _tElement	>(&Data[row*Width], Width); }

		// Methods
		inline constexpr	const _tElement*						begin						()																	const	{ return Data;						}
		inline constexpr	const _tElement*						end							()																	const	{ return Data + size();				}

		inline				_tElement*								begin						()																			{ return Data;						}
		inline				_tElement*								end							()																			{ return Data + size();				}

		inline constexpr	uint32_t								size						()																	const	{ return Width * Height;			}
		inline constexpr	uint32_t								width						()																	const	{ return Width	;					}
		inline constexpr	uint32_t								height						()																	const	{ return Height	;					}
	};
#pragma pack(pop)
}

#endif // CHO_GRID_VIEW_H_3423423