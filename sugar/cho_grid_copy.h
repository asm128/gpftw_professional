#include "cho_grid_view.h"
#include "cho_bit_view.h"

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
						uint32_t								grid_copy_row_calc			(::cho::grid_view<_tCell>& dst, const ::cho::SCoord2<uint32_t>& srcMetrics, const ::cho::SRectangle2D<_tCoord>& srcRect, uint32_t xDstOffset, uint32_t xSrcOffset)		{
		return (uint32_t)::cho::max		// Make sure everything is in range.
			( 0
			, ::cho::min
				( ::cho::min((int32_t)(srcMetrics.x - xSrcOffset), (int32_t)(dst.width() - xDstOffset))
				, (int32_t)srcRect.Size.x
				)
			);	// 
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_alpha_bit			(::cho::grid_view<_tCell>& dst, const ::cho::bit_view<uint32_t>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SCoord2<uint32_t> & srcMetrics, const _tCell& color, const ::cho::SRectangle2D<_tCoord>& srcRect)		{
		const uint32_t													xDstOffset					= (uint32_t)::cho::clamp((int32_t)dstOffset.x			, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= (uint32_t)::cho::clamp((int32_t)srcRect.Offset.x		, 0, (int32_t)srcMetrics.x);			// 
		const int32_t													ySrcLimit					= ::cho::min((int32_t)(srcRect.Offset.y + srcRect.Size.y),  (int32_t)srcMetrics.y);
		const uint32_t													xCopyCells					= ::cho::grid_copy_row_calc(dst, srcMetrics, srcRect, xDstOffset, xSrcOffset);
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = (int32_t)srcRect.Size.y; y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstOffset.y;
			const int32_t													ySrc						= y + (int32_t)srcRect.Offset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= (int32_t)dst.height() || ySrc >= ySrcLimit) 
				break;
			for(uint32_t x = 0, xMax = xCopyCells; x < xMax; ++x) {
				const uint32_t xSrc = xSrcOffset + x;
				if(true == src[ySrc * srcMetrics.x + xSrc])
					dst[yDst][xDstOffset + x]									= color;
			}
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_ex					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& size_, const ::cho::SCoord2<_tCoord>& offsetDst, const ::cho::SCoord2<_tCoord>& offsetSrc)	{
		for(int32_t y = 0; y < size_.y; ++y) {
#if defined(CHO_DEBUG_GRID_COPY_EX) && (defined(DEBUG) || defined(_DEBUG)) 
			for(int32_t x = 0; x < size_.x; ++x)
				dst[y + offsetDst.y][x + offsetDst.x]							= src[y + offsetSrc.y][x + offsetSrc.x];
#else
			const uint32_t													sizeToCopy						= size_.x * sizeof(_tCell);
			memcpy(&dst[y + offsetDst.y][offsetDst.x], &src[y + offsetSrc.y][offsetSrc.x], sizeToCopy);
#endif
		}
		return ::cho::max(size_.x * size_.y, 0);
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_alpha_ex				(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& size_, const ::cho::SCoord2<_tCoord>& offsetDst, const ::cho::SCoord2<_tCoord>& offsetSrc, const _tCell& comparand)	{
		for(int32_t y = 0; y < size_.y; ++y)
			for(int32_t x = 0; x < size_.x; ++x)
				if(src[y + offsetSrc.y][x + offsetSrc.x] != comparand)
					dst[y + offsetDst.y][x + offsetDst.x]							= src[y + offsetSrc.y][x + offsetSrc.x];
		return ::cho::max(size_.x * size_.y, 0);
	}

	template<typename _tCoord>
						::cho::error_t							grid_raster_alpha_bit_ex		(const ::cho::bit_view<uint32_t>& src, const ::cho::SCoord2<uint32_t>& srcMetrics, const ::cho::SCoord2<_tCoord>& size_, const ::cho::SCoord2<_tCoord>& offsetDst, const ::cho::SCoord2<_tCoord>& offsetSrc, ::cho::array_pod<::cho::SCoord2<uint32_t>> & dstCoords)	{
		for(int32_t y = 0; y < size_.y; ++y)
			for(int32_t x = 0; x < size_.x; ++x)
				if(src[(y + offsetSrc.y) * srcMetrics.x + (x + offsetSrc.x)])
					dstCoords.push_back({(uint32_t)(x + offsetDst.x), (uint32_t)(y + offsetDst.y)});
		return ::cho::max(size_.x * size_.y, 0);
	}

	template<typename _tCoord>
						::cho::error_t							grid_raster_alpha_bit			(const ::cho::SCoord2<uint32_t>& dstMetrics, const ::cho::bit_view<uint32_t>& src, const ::cho::SCoord2<uint32_t>& srcMetrics, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SCoord2<_tCoord>& srcOffset, ::cho::array_pod<::cho::SCoord2<uint32_t>> & dstCoords)		{
		const int32_t													offsetYSrc						= srcOffset.y + -::cho::min(0, (int32_t)dstRect.Offset.y) ;
		const int32_t													offsetXSrc						= srcOffset.x + -::cho::min(0, (int32_t)dstRect.Offset.x) ;
		const int32_t													offsetYDst						= ::cho::max(0, (int32_t)dstRect.Offset.y);
		const int32_t													offsetXDst						= ::cho::max(0, (int32_t)dstRect.Offset.x);
		const int32_t													stopYDst						= (int32_t)(srcMetrics.y) + ::cho::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													stopXDst						= (int32_t)(srcMetrics.x) + ::cho::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													maxY							= ::cho::min(::cho::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dstMetrics.y - offsetYDst), (int32_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t													maxX							= ::cho::min(::cho::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dstMetrics.x - offsetXDst), (int32_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return ::cho::grid_raster_alpha_bit_ex(src, srcMetrics, ::cho::SCoord2<int32_t>{maxX, maxY}, ::cho::SCoord2<int32_t>{offsetXDst, offsetYDst}, ::cho::SCoord2<int32_t>{offsetXSrc, offsetYSrc}, dstCoords);
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_alpha					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SRectangle2D<_tCoord>& srcRect, const _tCell& comparand)		{
		const int32_t													offsetYSrc						= srcRect.Offset.y + -::cho::min(0, (int32_t)dstOffset.y) ;
		const int32_t													offsetXSrc						= srcRect.Offset.x + -::cho::min(0, (int32_t)dstOffset.x) ;
		const int32_t													offsetYDst						= ::cho::max(0, (int32_t)dstOffset.y);
		const int32_t													offsetXDst						= ::cho::max(0, (int32_t)dstOffset.x);
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::cho::min(0, (int32_t)dstOffset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::cho::min(0, (int32_t)dstOffset.x);
		const int32_t													maxY							= ::cho::min(::cho::min(stopYDst - (int32_t)srcRect.Offset.y, (int32_t)dst.metrics().y - offsetYDst), (int32_t)srcRect.Size.y);
		const int32_t													maxX							= ::cho::min(::cho::min(stopXDst - (int32_t)srcRect.Offset.x, (int32_t)dst.metrics().x - offsetXDst), (int32_t)srcRect.Size.x);
		return ::cho::grid_copy_alpha_ex(dst, src, ::cho::SCoord2<int32_t>{maxX, maxY}, ::cho::SCoord2<int32_t>{offsetXDst, offsetYDst}, ::cho::SCoord2<int32_t>{offsetXSrc, offsetYSrc}, comparand);
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SRectangle2D<_tCoord>& srcRect)		{
		const int32_t													offsetYSrc						= srcRect.Offset.y + -::cho::min(0, (int32_t)dstOffset.y) ;
		const int32_t													offsetXSrc						= srcRect.Offset.x + -::cho::min(0, (int32_t)dstOffset.x) ;
		const int32_t													offsetYDst						= ::cho::max(0, (int32_t)dstOffset.y);
		const int32_t													offsetXDst						= ::cho::max(0, (int32_t)dstOffset.x);
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::cho::min(0, (int32_t)dstOffset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::cho::min(0, (int32_t)dstOffset.x);
		const int32_t													maxY							= ::cho::min(::cho::min(stopYDst - (int32_t)srcRect.Offset.y, (int32_t)dst.metrics().y - offsetYDst), (int32_t)srcRect.Size.y);
		const int32_t													maxX							= ::cho::min(::cho::min(stopXDst - (int32_t)srcRect.Offset.x, (int32_t)dst.metrics().x - offsetXDst), (int32_t)srcRect.Size.x);
		return ::cho::grid_copy_ex(dst, src, ::cho::SCoord2<int32_t>{maxX, maxY}, ::cho::SCoord2<int32_t>{offsetXDst, offsetYDst}, ::cho::SCoord2<int32_t>{offsetXSrc, offsetYSrc});
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_alpha					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SCoord2<_tCoord>& srcOffset, const _tCell& comparand)		{
		const int32_t													offsetYSrc						= srcOffset.y + -::cho::min(0, (int32_t)dstRect.Offset.y) ;
		const int32_t													offsetXSrc						= srcOffset.x + -::cho::min(0, (int32_t)dstRect.Offset.x) ;
		const int32_t													offsetYDst						= ::cho::max(0, (int32_t)dstRect.Offset.y);
		const int32_t													offsetXDst						= ::cho::max(0, (int32_t)dstRect.Offset.x);
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::cho::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::cho::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													maxY							= ::cho::min(::cho::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetYDst), (int32_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t													maxX							= ::cho::min(::cho::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetXDst), (int32_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return ::cho::grid_copy_alpha_ex(dst, src, ::cho::SCoord2<int32_t>{maxX, maxY}, ::cho::SCoord2<int32_t>{offsetXDst, offsetYDst}, ::cho::SCoord2<int32_t>{offsetXSrc, offsetYSrc}, comparand);
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SCoord2<_tCoord>& srcOffset)		{
		const int32_t													offsetYSrc						= srcOffset.y + -::cho::min(0, (int32_t)dstRect.Offset.y) ;
		const int32_t													offsetXSrc						= srcOffset.x + -::cho::min(0, (int32_t)dstRect.Offset.x) ;
		const int32_t													offsetYDst						= ::cho::max(0, (int32_t)dstRect.Offset.y);
		const int32_t													offsetXDst						= ::cho::max(0, (int32_t)dstRect.Offset.x);
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::cho::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::cho::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													maxY							= ::cho::min(::cho::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetYDst), (int32_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t													maxX							= ::cho::min(::cho::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetXDst), (int32_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return ::cho::grid_copy_ex(dst, src, ::cho::SCoord2<int32_t>{maxX, maxY}, ::cho::SCoord2<int32_t>{offsetXDst, offsetYDst}, ::cho::SCoord2<int32_t>{offsetXSrc, offsetYSrc});
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_alpha					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SCoord2<_tCoord>& srcOffset, const _tCell& comparand)		{
		const int32_t													offsetYSrc						= srcOffset.y + -::cho::min(0, (int32_t)dstOffset.y) ;
		const int32_t													offsetXSrc						= srcOffset.x + -::cho::min(0, (int32_t)dstOffset.x) ;
		const int32_t													offsetYDst						= ::cho::max(0, (int32_t)dstOffset.y);
		const int32_t													offsetXDst						= ::cho::max(0, (int32_t)dstOffset.x);
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::cho::min(0, (int32_t)dstOffset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::cho::min(0, (int32_t)dstOffset.x);
		const int32_t													maxY							= ::cho::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetYDst);
		const int32_t													maxX							= ::cho::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetXDst);
		return ::cho::grid_copy_alpha_ex(dst, src, ::cho::SCoord2<int32_t>{maxX, maxY}, ::cho::SCoord2<int32_t>{offsetXDst, offsetYDst}, ::cho::SCoord2<int32_t>{offsetXSrc, offsetYSrc}, comparand);
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SCoord2<_tCoord>& srcOffset)		{
		const int32_t													offsetYSrc						= srcOffset.y + -::cho::min(0, (int32_t)dstOffset.y) ;
		const int32_t													offsetXSrc						= srcOffset.x + -::cho::min(0, (int32_t)dstOffset.x) ;
		const int32_t													offsetYDst						= ::cho::max(0, (int32_t)dstOffset.y);
		const int32_t													offsetXDst						= ::cho::max(0, (int32_t)dstOffset.x);
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::cho::min(0, (int32_t)dstOffset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::cho::min(0, (int32_t)dstOffset.x);
		const int32_t													maxY							= ::cho::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetYDst);
		const int32_t													maxX							= ::cho::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetXDst);
		return ::cho::grid_copy_ex(dst, src, ::cho::SCoord2<int32_t>{maxX, maxY}, ::cho::SCoord2<int32_t>{offsetXDst, offsetYDst}, ::cho::SCoord2<int32_t>{offsetXSrc, offsetYSrc});
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_alpha					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const _tCell& comparand)	{
		const int32_t													offsetYSrc						= -::cho::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													offsetXSrc						= -::cho::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													offsetYDst						= ::cho::max(0, (int32_t)dstRect.Offset.y);
		const int32_t													offsetXDst						= ::cho::max(0, (int32_t)dstRect.Offset.x);
		const int32_t													stopYDst						= (int32_t)src.metrics().y + ::cho::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													stopXDst						= (int32_t)src.metrics().x + ::cho::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													maxY							= ::cho::min(::cho::min(stopYDst, (int32_t)dst.metrics().y - offsetYDst), (int32_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t													maxX							= ::cho::min(::cho::min(stopXDst, (int32_t)dst.metrics().x - offsetXDst), (int32_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return ::cho::grid_copy_alpha_ex(dst, src, ::cho::SCoord2<int32_t>{maxX, maxY}, ::cho::SCoord2<int32_t>{offsetXDst, offsetYDst}, ::cho::SCoord2<int32_t>{offsetXSrc, offsetYSrc}, comparand);
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect)		{
		const int32_t													offsetYSrc						= -::cho::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													offsetXSrc						= -::cho::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													offsetYDst						= ::cho::max(0, (int32_t)dstRect.Offset.y);
		const int32_t													offsetXDst						= ::cho::max(0, (int32_t)dstRect.Offset.x);
		const int32_t													stopYDst						= (int32_t)src.metrics().y + ::cho::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													stopXDst						= (int32_t)src.metrics().x + ::cho::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													maxY							= ::cho::min(::cho::min(stopYDst, (int32_t)dst.metrics().y - offsetYDst), (int32_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t													maxX							= ::cho::min(::cho::min(stopXDst, (int32_t)dst.metrics().x - offsetXDst), (int32_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return ::cho::grid_copy_ex(dst, src, ::cho::SCoord2<int32_t>{maxX, maxY}, ::cho::SCoord2<int32_t>{offsetXDst, offsetYDst}, ::cho::SCoord2<int32_t>{offsetXSrc, offsetYSrc});
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy_alpha					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const _tCell& comparand)		{
		const ::cho::SCoord2<int32_t>									offsetSrc						= {-::cho::min(0, (int32_t)dstOffset.x), -::cho::min(0, (int32_t)dstOffset.y)};
		const ::cho::SCoord2<int32_t>									offsetDst						= { ::cho::max(0, (int32_t)dstOffset.x),  ::cho::max(0, (int32_t)dstOffset.y)};
		::cho::SCoord2<int32_t>											stopDst							= src.metrics().Cast<int32_t>() + ::cho::SCoord2<int32_t>{::cho::min(0, (int32_t)dstOffset.x), ::cho::min(0, (int32_t)dstOffset.y)};
		stopDst.y													= ::cho::min(stopDst.y, (int32_t)dst.metrics().y - offsetDst.y);
		stopDst.x													= ::cho::min(stopDst.x, (int32_t)dst.metrics().x - offsetDst.x);
		return ::cho::grid_copy_alpha_ex(dst, src, stopDst, offsetDst, offsetSrc, comparand);
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							grid_copy						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset)		{
		const ::cho::SCoord2<int32_t>									offsetSrc						= {-::cho::min(0, (int32_t)dstOffset.x), -::cho::min(0, (int32_t)dstOffset.y)};
		const ::cho::SCoord2<int32_t>									offsetDst						= { ::cho::max(0, (int32_t)dstOffset.x),  ::cho::max(0, (int32_t)dstOffset.y)};
		::cho::SCoord2<int32_t>											stopDst							= src.metrics().Cast<int32_t>() + ::cho::SCoord2<int32_t>{::cho::min(0, (int32_t)dstOffset.x), ::cho::min(0, (int32_t)dstOffset.y)};
		stopDst.y													= ::cho::min(stopDst.y, (int32_t)dst.metrics().y - offsetDst.y);
		stopDst.x													= ::cho::min(stopDst.x, (int32_t)dst.metrics().x - offsetDst.x);
		return ::cho::grid_copy_ex(dst, src, stopDst, offsetDst, offsetSrc);
	}

	template<typename _tCell>
						::cho::error_t							grid_copy_alpha					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const _tCell& comparand)		{
		const int32_t													maxY							= (int32_t)::cho::min(dst.metrics().y, src.metrics().y);
		const int32_t													maxX							= (int32_t)::cho::min(dst.metrics().x, src.metrics().x);
		return ::cho::grid_copy_alpha_ex(dst, src, ::cho::SCoord2<int32_t>{maxX, maxY}, ::cho::SCoord2<int32_t>{}, ::cho::SCoord2<int32_t>{}, comparand);
	}

	template<typename _tCell>
						::cho::error_t							grid_copy						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src)		{
		const int32_t													maxY							= (int32_t)::cho::min(dst.metrics().y, src.metrics().y);
		const int32_t													maxX							= (int32_t)::cho::min(dst.metrics().x, src.metrics().x);
		return ::cho::grid_copy_ex(dst, src, ::cho::SCoord2<int32_t>{maxX, maxY}, ::cho::SCoord2<int32_t>{}, ::cho::SCoord2<int32_t>{});
	}


	template<typename _tCell>
						::cho::error_t							grid_mirror_x					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src)		{
		const uint32_t													yCopyRows						= ::cho::min(dst.metrics().y, src.metrics().y);
		const uint32_t													xCopyCells						= ::cho::min(dst.metrics().x, src.metrics().x);
		uint32_t														srcMaxX							= src.metrics().x - 1;
		for(uint32_t y = 0; y < yCopyRows; ++y) 
		for(uint32_t x = 0; x < xCopyCells; ++x) 
			dst[y][x]													= src[y][srcMaxX - x];
		return xCopyCells * yCopyRows;
	}

	template<typename _tCell>
						::cho::error_t							grid_mirror_y					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src)		{
		const uint32_t													yCopyRows						= ::cho::min(dst.metrics().y, src.metrics().y);
		const uint32_t													xCopyCells						= ::cho::min(dst.metrics().x, src.metrics().x);
		uint32_t														srcMaxY							= src.metrics().y - 1;
		for(uint32_t y = 0; y < yCopyRows; ++y) 
			memcpy(&dst[y][0], &src[srcMaxY - y][0], sizeof(_tCell) * xCopyCells);
		return xCopyCells * yCopyRows;
	}

} // namespace

#endif // CHO_GRID_COPY_20983429834
