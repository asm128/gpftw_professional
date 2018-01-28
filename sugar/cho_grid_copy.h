#include "cho_grid_view.h"

#ifndef CHO_GRID_COPY_20983429834
#define CHO_GRID_COPY_20983429834

namespace cho
{
	template<typename _tCell, typename _tCoord>
						uint32_t								grid_copy_row_calc			(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SRectangle2D<_tCoord>& srcRect, uint32_t xDstOffset, uint32_t xSrcOffset)		{
		return ::cho::max
			( 0
			, ::cho::min
				( ::cho::min
					( ::cho::min((int32_t)(src.width() - xSrcOffset), (int32_t)(dst.width() - xDstOffset))
					, (int32_t)srcRect.Size.x
					)
				, (int32_t)dstRect.Size.x
				)
			);	// 
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_row				(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SRectangle2D<_tCoord>& srcRect, uint32_t yDst, uint32_t ySrc)		{
		// Make sure everything is in range.
		const uint32_t													xDstOffset					= ::cho::clamp((int32_t)dstRect.Offset.x, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= ::cho::clamp((int32_t)srcRect.Offset.x, 0, (int32_t)src.width());			// 
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, dstRect, srcRect, xDstOffset, xSrcOffset);
		if(xDstOffset < dst.width() && xSrcOffset < src.width() && xCopyCells)
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
		return xCopyCells;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SRectangle2D<_tCoord>& srcRect)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstRect.Offset.y;
			const uint32_t													ySrc						= y + srcRect.Offset.y;
			if(yDst >= dst.height() || yDst >= (uint32_t)(dstRect.Offset.y + dstRect.Size.y))
				continue;
			if(ySrc <  src.height() && ySrc <  (uint32_t)(srcRect.Offset.y + srcRect.Size.y))
				elementsCopied												+= ::cho::grid_copy_row(dst, src, dstRect, srcRect, yDst, ySrc);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						uint32_t								grid_copy_row_calc			(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& srcRect, uint32_t xDstOffset, uint32_t xSrcOffset)		{
		return (uint32_t)::cho::max
			( 0
			, ::cho::min
				( ::cho::min((int32_t)(src.width() - xSrcOffset), (int32_t)(dst.width() - xDstOffset))
				, (int32_t)srcRect.Size.x
				)
			);	// 
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_row				(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SRectangle2D<_tCoord>& srcRect, uint32_t yDst, uint32_t ySrc)		{
		// Make sure everything is in range.
		const uint32_t													xDstOffset					= ::cho::clamp((int32_t)dstOffset.x			, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= ::cho::clamp((int32_t)srcRect.Offset.x	, 0, (int32_t)src.width());			// 
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, srcRect, xDstOffset, xSrcOffset);
		if(xDstOffset < dst.width() && xSrcOffset < src.width() && xCopyCells)
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
		return xCopyCells;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SRectangle2D<_tCoord>& srcRect)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstOffset.y;
			const uint32_t													ySrc						= y + srcRect.Offset.y;
			if(yDst >= dst.height())
				continue;
			if(ySrc < src.height() && ySrc < (uint32_t)(srcRect.Offset.y + srcRect.Size.y))
				elementsCopied												+= ::cho::grid_copy_row(dst, src, dstOffset, srcRect, yDst, ySrc);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_row				(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SCoord2<_tCoord>& srcOffset, uint32_t yDst, uint32_t ySrc)		{
		// Make sure everything is in range.
		const uint32_t													xDstOffset					= ::cho::clamp((int32_t)dstRect.Offset.x	, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= ::cho::clamp((int32_t)srcOffset.x			, 0, (int32_t)src.width());			// 
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, dstRect, xDstOffset, xSrcOffset);
		if(xDstOffset < dst.width() && xSrcOffset < src.width() && xCopyCells)
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
		return xCopyCells;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SCoord2<_tCoord>& srcOffset)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstRect.Offset.y;
			const uint32_t													ySrc						= y + srcOffset.y;
			if(yDst >= dst.height() || yDst >= (uint32_t)(dstRect.Offset.y + dstRect.Size.y))
				continue;
			if(ySrc < src.height())
				elementsCopied												+= ::cho::grid_copy_row(dst, src, dstRect, srcOffset, yDst, ySrc);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						uint32_t								grid_copy_row_calc			(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, uint32_t xDstOffset)		{
		return ::cho::min
			( ::cho::min
				( (int32_t)src.width()
				, ::cho::max(0, (int32_t)(dst.width() - xDstOffset))
				)
			, (uint32_t)dstRect.Size.x);
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_row				(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, uint32_t yDst, uint32_t ySrc)		{
		// Make sure everything is in range.
		const uint32_t													xDstOffset					= ::cho::clamp((int32_t)dstRect.Offset.x, 0, (int32_t)dst.width());			// 
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, dstRect, xDstOffset);	// 
		if(xDstOffset < dst.width() && xCopyCells)
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][0], sizeof(_tCell) * xCopyCells);
		return xCopyCells;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstRect.Offset.y;
			if(yDst < dst.height() && yDst < (uint32_t)(dstRect.Offset.y + dstRect.Size.y))
				elementsCopied												+= ::cho::grid_copy_row(dst, src, dstRect, yDst, y);
		}
		return elementsCopied;
	}

	template<typename _tCell>
						uint32_t								grid_copy_row_calc			(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, uint32_t xDstOffset, uint32_t xSrcOffset)		{
		return ::cho::max(0, (int32_t)::cho::min((int32_t)src.width() - xSrcOffset, (int32_t)dst.width() - xDstOffset));
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_row				(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SCoord2<_tCoord>& srcOffset, uint32_t yDst, uint32_t ySrc)					{
		// Make sure everything is in range.
		const uint32_t													xDstOffset					= ::cho::clamp((int32_t)dstOffset.x, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= ::cho::clamp((int32_t)srcOffset.x, 0, (int32_t)src.width());			// 
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, xDstOffset, xSrcOffset);	// 
		if(xDstOffset < dst.width() && xSrcOffset < src.width() && xCopyCells)
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
		return xCopyCells;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SCoord2<_tCoord>& srcOffset)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstOffset.y;
			const uint32_t													ySrc						= y + srcOffset.y;
			if(yDst < dst.height() && ySrc < src.height())
				elementsCopied												+= ::cho::grid_copy_row(dst, src, dstOffset, srcOffset, yDst, ySrc);
		}
		return elementsCopied;
	}

	template<typename _tCell>
						uint32_t								grid_copy_row_calc			(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, uint32_t xDstOffset)		{
		return (uint32_t)::cho::max(0, (int32_t)::cho::min((int32_t)src.width(), (int32_t)dst.width() - (int32_t)xDstOffset));
	}
	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_row				(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, uint32_t yDst, uint32_t ySrc)		{
		// Make sure everything is in range.
		const uint32_t													xDstOffset					= ::cho::clamp((int32_t)dstOffset.x, 0, (int32_t)dst.width());			// 
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, src, xDstOffset);	// 
		if(xDstOffset < dst.width() && xCopyCells)
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][0], sizeof(_tCell) * xCopyCells);
		return xCopyCells;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstOffset.y;
			if(yDst < dst.height())
				elementsCopied												+= ::cho::grid_copy_row(dst, src, dstOffset, yDst, y);
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
