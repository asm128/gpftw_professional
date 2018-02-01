#include "cho_grid_view.h"

#ifndef CHO_GRID_SCALE_20654834
#define CHO_GRID_SCALE_20654834

namespace cho
{
	template<typename _tCell>
						::cho::error_t							grid_scale					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const float xFactor, const float yFactor)		{
		for(uint32_t y = 0; y < dst.height(); ++y) 
			for(uint32_t x = 0; x < dst.width(); ++x) 
				dst[y][x]													= src[(uint32_t)(y * yFactor)][(uint32_t)(x * xFactor)];
		return 0;
	}

	template<typename _tCell>
						::cho::error_t							grid_scale					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src)		{
		const float														xFactor						= src.width () / (float)dst.width ();
		const float														yFactor						= src.height() / (float)dst.height();
		return grid_scale(dst, src, xFactor, yFactor);
	}
}

#endif // CHO_GRID_SCALE_20983429834
