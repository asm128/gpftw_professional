#include "cho_grid_view.h"

#ifndef CHO_GRID_COPY_20983429834
#define CHO_GRID_COPY_20983429834

namespace cho
{
	template<typename _tCell, typename _tCoord>
						uint32_t								grid_copy_row_calc			(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SRectangle2D<_tCoord>& srcRect, uint32_t xDstOffset, uint32_t xSrcOffset)		{
		return (uint32_t)::cho::max
			( 0
			, ::cho::min
				( ::cho::min
					( ::cho::min
						((int32_t)(src.width() - xSrcOffset), (int32_t)(dst.width() - xDstOffset))
						, (int32_t)srcRect.Size.x
						)
				, (int32_t)dstRect.Size.x
				)
			);	// 
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SRectangle2D<_tCoord>& srcRect)		{
		const uint32_t													xDstOffset					= (uint32_t)::cho::clamp((int32_t)dstRect.Offset.x, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= (uint32_t)::cho::clamp((int32_t)srcRect.Offset.x, 0, (int32_t)src.width());			// 
		const int32_t													yDstLimit					= ::cho::min((int32_t)(dstRect.Offset.y + dstRect.Size.y), (int32_t)dst.height());
		const int32_t													ySrcLimit					= ::cho::min((int32_t)(srcRect.Offset.y + srcRect.Size.y), (int32_t)src.height());
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, dstRect, srcRect, xDstOffset, xSrcOffset);
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = (int32_t)srcRect.Size.y; y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstRect.Offset.y;
			const int32_t													ySrc						= y + (int32_t)srcRect.Offset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= yDstLimit || ySrc >= ySrcLimit) 
				break;
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						uint32_t								grid_copy_row_calc			(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& srcRect, uint32_t xDstOffset, uint32_t xSrcOffset)		{
		return (uint32_t)::cho::max		// Make sure everything is in range.
			( 0
			, ::cho::min
				( ::cho::min((int32_t)(src.width() - xSrcOffset), (int32_t)(dst.width() - xDstOffset))
				, (int32_t)srcRect.Size.x
				)
			);	// 
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SRectangle2D<_tCoord>& srcRect)		{
		const uint32_t													xDstOffset					= (uint32_t)::cho::clamp((int32_t)dstOffset.x			, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= (uint32_t)::cho::clamp((int32_t)srcRect.Offset.x		, 0, (int32_t)src.width());			// 
		const int32_t													ySrcLimit					= ::cho::min((int32_t)(srcRect.Offset.y + srcRect.Size.y),  (int32_t)src.height());
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, srcRect, xDstOffset, xSrcOffset);
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = (int32_t)srcRect.Size.y; y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstOffset.y;
			const int32_t													ySrc						= y + (int32_t)srcRect.Offset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= (int32_t)dst.height() || ySrc >= ySrcLimit) 
				break;
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SCoord2<_tCoord>& srcOffset)		{
		const uint32_t													xDstOffset					= (uint32_t)::cho::clamp((int32_t)dstRect.Offset.x		, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= (uint32_t)::cho::clamp((int32_t)srcOffset.x			, 0, (int32_t)src.width());			// 
		const int32_t													yDstLimit					= ::cho::min((int32_t)(dstRect.Offset.y + dstRect.Size.y),  (int32_t)dst.height());
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, dstRect, xDstOffset, xSrcOffset);
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstRect.Offset.y;
			const int32_t													ySrc						= y + (int32_t)srcOffset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= yDstLimit || ySrc >= (int32_t)src.height()) 
				break;
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						uint32_t								grid_copy_row_calc			(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, uint32_t xDstOffset)		{
		return (uint32_t)::cho::min	// Make sure everything is in range.
			( ::cho::min
				( (int32_t)src.width()
				, ::cho::max(0, (int32_t)(dst.width() - xDstOffset))
				)
			, (int32_t)dstRect.Size.x);
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_alpha				(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const _tCell& comparand)		{
		const uint32_t													xDstOffset					= (uint32_t)::cho::clamp((int32_t)dstRect.Offset.x, 0, (int32_t)dst.width());			// 
		const int32_t													yDstLimit					= ::cho::min((int32_t)(dstRect.Offset.y + dstRect.Size.y),  (int32_t)dst.height());
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, dstRect, xDstOffset);	// 
		uint32_t														elementsCopied				= 0;
		for(int32_t ySrc = 0, yMax = (int32_t)src.height(); ySrc < yMax; ++ySrc) {
			const int32_t													yDst						= ySrc + (int32_t)dstRect.Offset.y;
			if(yDst < 0)
				continue;
			if(yDst >= yDstLimit) 
				break;
			for(uint32_t x = 0, xStop = xCopyCells; x < xStop; ++x)
				if(comparand != src[ySrc][x])
					dst[yDst][x + xDstOffset]									= src[ySrc][x];
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect)		{
		const uint32_t													xDstOffset					= (uint32_t)::cho::clamp((int32_t)dstRect.Offset.x, 0, (int32_t)dst.width());			// 
		const int32_t													yDstLimit					= ::cho::min((int32_t)(dstRect.Offset.y + dstRect.Size.y),  (int32_t)dst.height());
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, dstRect, xDstOffset);	// 
		uint32_t														elementsCopied				= 0;
		for(int32_t ySrc = 0, yMax = (int32_t)src.height(); ySrc < yMax; ++ySrc) {
			const int32_t													yDst						= ySrc + (int32_t)dstRect.Offset.y;
			if(yDst < 0)
				continue;
			if(yDst >= yDstLimit) 
				break;
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][0], sizeof(_tCell) * xCopyCells);
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell>
						uint32_t								grid_copy_row_calc			(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, uint32_t xDstOffset, uint32_t xSrcOffset)		{
		return ::cho::max(0, (int32_t)::cho::min((int32_t)src.width() - xSrcOffset, (int32_t)dst.width() - xDstOffset));	// Make sure everything is in range.
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SCoord2<_tCoord>& srcOffset)		{
		const uint32_t													xDstOffset					= (uint32_t)::cho::clamp((int32_t)dstOffset.x, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= (uint32_t)::cho::clamp((int32_t)srcOffset.x, 0, (int32_t)src.width());			// 
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, xDstOffset, xSrcOffset);	// 
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = (int32_t)src.height(); y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstOffset.y;
			const int32_t													ySrc						= y + (int32_t)srcOffset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= (int32_t)dst.height() || ySrc >= (int32_t)src.height()) 
				break;
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);	// memcpy the row from src to dst.
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell>
						uint32_t								grid_copy_row_calc			(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, uint32_t xDstOffset)		{
		return (uint32_t)::cho::max(0, (int32_t)::cho::min((int32_t)src.width(), (int32_t)dst.width() - (int32_t)xDstOffset));
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset)		{
		const uint32_t													xDstOffset					= (uint32_t)::cho::clamp((int32_t)dstOffset.x, 0, (int32_t)dst.width());			// 
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, xDstOffset);	// 
		uint32_t														elementsCopied				= 0;
		for(int32_t ySrc = 0, yMax = (int32_t)src.height(); ySrc < yMax; ++ySrc) {
			const int32_t													yDst						= ySrc + (int32_t)dstOffset.y;
			if(yDst < 0) 
				continue;
			if(yDst >= (int32_t)dst.height()) 
				break;
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][0], sizeof(_tCell) * xCopyCells);
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_alpha				(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const _tCell& comparand)		{
		const uint32_t													xDstOffset					= (uint32_t)::cho::clamp((int32_t)dstOffset.x, 0, (int32_t)dst.width());			// 
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, xDstOffset);	// 
		uint32_t														elementsCopied				= 0;
		for(int32_t ySrc = 0, yMax = (int32_t)src.height(); ySrc < yMax; ++ySrc) {
			const int32_t													yDst						= ySrc + (int32_t)dstOffset.y;
			if(yDst < 0) 
				continue;
			if(yDst >= (int32_t)dst.height()) 
				break;
			for(uint32_t x = 0, xStop = xCopyCells; x < xStop; ++x)
				if(comparand != src[ySrc][x])
					dst[yDst][x + xDstOffset]									= src[ySrc][x];
			//memcpy(&dst[yDst][xDstOffset], &src[ySrc][0], sizeof(_tCell) * xCopyCells);
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src)		{
		const uint32_t													yCopyRows					= ::cho::min(dst.height(), src.height());
		const uint32_t													xCopyCells					= ::cho::min(dst.width (), src.width ());
		for(uint32_t y = 0; y < yCopyRows; ++y) 
			memcpy(&dst[y][0], &src[y][0], sizeof(_tCell) * xCopyCells);
		return xCopyCells * yCopyRows;
	}
} // namespace

#endif // CHO_GRID_COPY_20983429834
