#include "cho_array.h"
#include "cho_grid_view.h"

#ifndef CHO_GRID_H_9802374982374
#define CHO_GRID_H_9802374982374

namespace cho
{
	template<typename _tCell, size_t _sizeWidth, size_t _sizeDepth>
	struct grid_static : public grid_view<_tCell> {
							typedef										_tCell									TCell;
							typedef										grid_view<_tCell>						TGridView;

		static constexpr	const uint32_t								Width									= (uint32_t)_sizeWidth;
		static constexpr	const uint32_t								Depth									= (uint32_t)_sizeDepth;

							TCell										Cells	[_sizeDepth][_sizeWidth]		= {};

		inline constexpr												grid_static								()																		: grid_view(&Cells[0][0], _sizeWidth, _sizeDepth)	{}

							::cho::error_t								read									(const byte_t* input, uint32_t* inout_bytesRead)						{
			ree_if(0 == input, "Invalid input pointer!");
			TGridView															inputGrid								= {(_tCell*)input, Width, Depth};
			*inout_bytesRead												+= sizeof(_tCell) * size();
			for(uint32_t y = 0; y < Depth; ++y) {
				for(uint32_t x = 0; x < Width; ++x)
					::cho::podcpy(&Cells[y][x], inputGrid[y][x]);
			}
			return 0;
		}

							::cho::error_t								write									(byte_t* input, uint32_t* inout_bytesWritten)		const				{
			ree_if(0 == input && 0 == inout_bytesWritten, "Invalid input!");
			if(0 != inout_bytesWritten)
				*inout_bytesWritten												+= sizeof(_tCell) * size();	// Just return the size required to store this.

			if(0 == input) 
				return 0;

			TGridView															newStorage								= {(_tCell*)input, Width, Depth};
			for(uint32_t y = 0; y < Depth; ++y) {
				for(uint32_t x = 0; x < Width; ++x)
					::cho::podcpy(&newStorage[i], &Data[i]);
			}
			return 0;
		}
	};
}

#endif // CHO_GRID_H_9802374982374
