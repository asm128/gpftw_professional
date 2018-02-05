#include "cho_array.h"
#include "cho_grid_view.h"

#ifndef CHO_TEXTURE_H_902387498237
#define CHO_TEXTURE_H_902387498237

namespace cho
{
	template<typename _tTexel>
	struct STexture {
							::cho::array_pod<_tTexel>							Texels										;
							::cho::grid_view<_tTexel>							View										;

		inline				::cho::array_view<_tTexel>							operator[]									(uint32_t index)								{ return View[index]; }
		inline	const		::cho::array_view<_tTexel>							operator[]									(uint32_t index)	const						{ return View[index]; }

							::cho::error_t										resize										(uint32_t newSizeX, uint32_t newSizeY)			{ cho_necall(Texels.resize(newSizeX * newSizeY), "cannot resize?"); View = {Texels.begin(), newSizeX, newSizeY}; return 0; }
		inline				::cho::error_t										resize										(::cho::SCoord2<uint32_t> newSize)				{ return resize(newSize.x, newSize.y); }
	};
}

#endif // CHO_TEXTURE_H_902387498237
