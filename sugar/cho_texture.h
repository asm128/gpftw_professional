#include "cho_array.h"
#include "cho_grid_view.h"

#ifndef CHO_TEXTURE_H_902387498237
#define CHO_TEXTURE_H_902387498237

namespace cho
{
	template<typename _tTexel>
	struct STexture {
							::cho::array_pod<_tTexel>							Texels										= {};
							::cho::grid_view<_tTexel>							View										= {};
	};
}

#endif // CHO_TEXTURE_H_902387498237
