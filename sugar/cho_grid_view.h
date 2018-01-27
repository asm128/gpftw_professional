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
#pragma pack(pop)

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBltRow					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SRectangle2D<_tCoord>& srcRect, uint32_t yDst, uint32_t ySrc)		{
		// Make sure everything is in range.
		const uint32_t													xDstOffset					= ::cho::clamp((int32_t)dstRect.Offset.x, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= ::cho::clamp((int32_t)srcRect.Offset.x, 0, (int32_t)src.width());			// 
		const uint32_t													xCopyCells					= ::cho::max
			( 0U
			, ::cho::min
				( ::cho::min
					( ::cho::min(src.width() - xSrcOffset, dst.width() - xDstOffset)
					, (uint32_t)srcRect.Size.x
					)
				, (uint32_t)dstRect.Size.x
				)
			);	// 
		if(xDstOffset < dst.width() && xSrcOffset < src.width() && xCopyCells)
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
		return xCopyCells;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SRectangle2D<_tCoord>& srcRect)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstRect.Offset.y;
			const uint32_t													ySrc						= y + srcRect.Offset.y;
			if(yDst >= dst.height() || yDst >= (uint32_t)(dstRect.Offset.y + dstRect.Size.y))
				continue;
			if(ySrc <  src.height() && ySrc <  (uint32_t)(srcRect.Offset.y + srcRect.Size.y))
				elementsCopied												+= ::cho::bitBltRow(dst, src, dstRect, srcRect, yDst, ySrc);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBltRow						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SRectangle2D<_tCoord>& srcRect, uint32_t yDst, uint32_t ySrc)		{
		// Make sure everything is in range.
		const uint32_t													xDstOffset					= ::cho::clamp((int32_t)dstOffset.x			, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= ::cho::clamp((int32_t)srcRect.Offset.x	, 0, (int32_t)src.width());			// 
		const uint32_t													xCopyCells					= ::cho::max
			( 0U
			, ::cho::min
				( ::cho::min(src.width() - xSrcOffset, dst.width() - xDstOffset)
				, (uint32_t)srcRect.Size.x
				)
			);	// 
		if(xDstOffset < dst.width() && xSrcOffset < src.width() && xCopyCells)
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
		return xCopyCells;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SRectangle2D<_tCoord>& srcRect)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstOffset.y;
			const uint32_t													ySrc						= y + srcRect.Offset.y;
			if(yDst >= dst.height())
				continue;
			if(ySrc < src.height() && ySrc < (uint32_t)(srcRect.Offset.y + srcRect.Size.y))
				elementsCopied												+= ::cho::bitBltRow(dst, src, dstOffset, srcRect, yDst, ySrc);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBltRow						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SCoord2<_tCoord>& srcOffset, uint32_t yDst, uint32_t ySrc)		{
		// Make sure everything is in range.
		const uint32_t													xDstOffset					= ::cho::clamp((int32_t)dstRect.Offset.x	, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= ::cho::clamp((int32_t)srcOffset.x			, 0, (int32_t)src.width());			// 
		const uint32_t													xCopyCells					= ::cho::max
			( 0U
			, ::cho::min
				( ::cho::min(src.width() - xSrcOffset, dst.width() - xDstOffset)
				, (uint32_t)dstRect.Size.x
				)
			);	// 
		if(xDstOffset < dst.width() && xSrcOffset < src.width() && xCopyCells)
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
		return xCopyCells;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SCoord2<_tCoord>& srcOffset)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstRect.Offset.y;
			const uint32_t													ySrc						= y + srcOffset.y;
			if(yDst >= dst.height() || yDst >= (uint32_t)(dstRect.Offset.y + dstRect.Size.y))
				continue;
			if(ySrc < src.height())
				elementsCopied												+= ::cho::bitBltRow(dst, src, dstRect, srcOffset, yDst, ySrc);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBltRow					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, uint32_t yDst, uint32_t ySrc)		{
		// Make sure everything is in range.
		const uint32_t													xDstOffset					= ::cho::clamp((int32_t)dstRect.Offset.x, 0, (int32_t)dst.width());			// 
		const uint32_t													xCopyCells					= ::cho::min
			( ::cho::min
				( src.width()
				, ::cho::max(0U, dst.width() - xDstOffset)
				)
			, (uint32_t)dstRect.Size.x);	// 
		if(xDstOffset < dst.width() && xCopyCells)
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][0], sizeof(_tCell) * xCopyCells);
		return xCopyCells;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstRect.Offset.y;
			if(yDst < dst.height() && yDst < (uint32_t)(dstRect.Offset.y + dstRect.Size.y))
				elementsCopied												+= ::cho::bitBltRow(dst, src, dstRect, yDst, y);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBltRow					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SCoord2<_tCoord>& srcOffset, uint32_t yDst, uint32_t ySrc)		{
		// Make sure everything is in range.
		const uint32_t													xDstOffset					= ::cho::clamp((int32_t)dstOffset.x, 0, (int32_t)dst.width());			// 
		const uint32_t													xSrcOffset					= ::cho::clamp((int32_t)srcOffset.x, 0, (int32_t)src.width());			// 
		const uint32_t													xCopyCells					= (uint32_t)::cho::max(0, (int32_t)::cho::min((int32_t)src.width() - xSrcOffset, (int32_t)dst.width() - xDstOffset));	// 
		if(xDstOffset < dst.width() && xSrcOffset < src.width() && xCopyCells)
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
		return xCopyCells;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SCoord2<_tCoord>& srcOffset)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstOffset.y;
			const uint32_t													ySrc						= y + srcOffset.y;
			if(yDst < dst.height() && ySrc < src.height())
				elementsCopied												+= ::cho::bitBltRow(dst, src, dstOffset, srcOffset, yDst, ySrc);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBltRow					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, uint32_t yDst, uint32_t ySrc)		{
		// Make sure everything is in range.
		const uint32_t													xDstOffset					= ::cho::clamp((int32_t)dstOffset.x, 0, (int32_t)dst.width());			// 
		const uint32_t													xCopyCells					= (uint32_t)::cho::max(0, (int32_t)::cho::min((int32_t)src.width(), (int32_t)dst.width() - (int32_t)xDstOffset));	// 
		if(xDstOffset < dst.width() && xCopyCells)
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][0], sizeof(_tCell) * xCopyCells);
		return xCopyCells;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstOffset.y;
			if(yDst < dst.height())
				elementsCopied												+= ::cho::bitBltRow(dst, src, dstOffset, yDst, y);
		}
		return elementsCopied;
	}

	template<typename _tCell>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src)		{
		const uint32_t													yCopyRows					= ::cho::min(dst.height(), src.height());
		const uint32_t													xCopyCells					= ::cho::min(dst.width (), src.width ());
		for(uint32_t y = 0; y < yCopyRows; ++y) 
			memcpy(&dst[y][0], &src[y][0], sizeof(_tCell) * xCopyCells);
		return xCopyCells * yCopyRows;
	}

//------ objBlt(): Same as bitBlt() but with assignment operator instead of memcpy().
	template<typename _tCell, typename _tCoord>
						::cho::error_t							objBltRow						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SRectangle2D<_tCoord>& srcRect, uint32_t yDst, uint32_t ySrc)	{
		for(uint32_t x = 0, xMax = src.width (); x < xMax; ++x) {
			const uint32_t													xDst						= x + dstRect.Offset.x;
			const uint32_t													xSrc						= x + srcRect.Offset.x;
			if(xDst >= dst.width() || xDst >= (uint32_t)(dstRect.Offset.x + dstRect.Size.x))
				continue;
			if(xSrc <  src.width() && xSrc <  (uint32_t)(srcRect.Offset.x + srcRect.Size.x))
				dst[yDst][xDst]												= src[ySrc][xSrc];
		}
		return 0;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							objBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SRectangle2D<_tCoord>& srcRect)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstRect.Offset.y;
			const uint32_t													ySrc						= y + srcRect.Offset.y;
			if(yDst >= dst.height() || yDst >= (uint32_t)(dstRect.Offset.y + dstRect.Size.y))
				continue;
			if(ySrc <  src.height() && ySrc <  (uint32_t)(srcRect.Offset.y + srcRect.Size.y))
				elementsCopied												+= ::cho::objBltRow(dst, src, dstRect, srcRect, yDst, ySrc);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							objBltRow					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SRectangle2D<_tCoord>& srcRect, uint32_t yDst, uint32_t ySrc)		{
		for(uint32_t x = 0, xMax = src.width (); x < xMax; ++x) {
			const uint32_t													xDst						= x + dstOffset.x;
			const uint32_t													xSrc						= x + srcRect.Offset.x;
			if(xDst >= dst.width())
				continue;
			if(xSrc < src.width() && xSrc < (uint32_t)(srcRect.Offset.x + srcRect.Size.x))
				dst[yDst][xDst]												= src[ySrc][xSrc];
		}
		return 0;
	}
	template<typename _tCell, typename _tCoord>
						::cho::error_t							objBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SRectangle2D<_tCoord>& srcRect)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstOffset.y;
			const uint32_t													ySrc						= y + srcRect.Offset.y;
			if(yDst >= dst.height())
				continue;
			if(ySrc < src.height() && ySrc < (uint32_t)(srcRect.Offset.y + srcRect.Size.y))
				elementsCopied												+= ::cho::objBltRow(dst, src, dstOffset, srcRect, yDst, ySrc);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							objBltRow					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SCoord2<_tCoord>& srcOffset, uint32_t yDst, uint32_t ySrc)		{
		for(uint32_t x = 0, xMax = src.width (); x < xMax; ++x) {
			const uint32_t													xDst						= x + dstRect.Offset.x;
			const uint32_t													xSrc						= x + srcOffset.x;
			if(xDst >= dst.width() || xDst >= (uint32_t)(dstRect.Offset.x + dstRect.Size.x))
				continue;
			if(xSrc < src.width())
				dst[yDst][xDst]												= src[ySrc][xSrc];
		}
		return 0;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							objBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SCoord2<_tCoord>& srcOffset)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstRect.Offset.y;
			const uint32_t													ySrc						= y + srcOffset.y;
			if(yDst >= dst.height() || yDst >= (uint32_t)(dstRect.Offset.y + dstRect.Size.y))
				continue;
			if(ySrc < src.height())
				elementsCopied												+= ::cho::objBltRow(dst, src, dstRect, srcOffset, yDst, ySrc);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							objBltRow					(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, uint32_t yDst, uint32_t ySrc)		{
		for(uint32_t x = 0, xMax = src.width (); x < xMax; ++x) {
			const uint32_t													xDst						= x + dstRect.Offset.x;
			if(xDst < dst.width() && xDst < (uint32_t)(dstRect.Offset.x + dstRect.Size.x))
				dst[yDst][xDst]												= src[ySrc][x];
		}
		return 0;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							objBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstRect.Offset.y;
			if(yDst < dst.height() && yDst < (uint32_t)(dstRect.Offset.y + dstRect.Size.y))
				elementsCopied												+= ::cho::objBltRow(dst, src, dstRect, yDst, y);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							objBltRow						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SCoord2<_tCoord>& srcOffset, uint32_t yDst, uint32_t ySrc)		{
		for(uint32_t x = 0, xMax = src.width (); x < xMax; ++x) {
			const uint32_t													xDst						= x + dstOffset.x;
			const uint32_t													xSrc						= x + srcOffset.x;
			if(xDst >= dst.width())
				continue;
			if(xSrc < src.width())
				dst[yDst][xDst]												= src[ySrc][xSrc];
		}
		return 0;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							objBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SCoord2<_tCoord>& srcOffset)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstOffset.y;
			const uint32_t													ySrc						= y + srcOffset.y;
			if(yDst >= dst.height())
				continue;
			if(ySrc < src.height())
				elementsCopied												+= ::cho::objBltRow(dst, src, dstOffset, srcOffset, yDst, ySrc);
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							objBltRow						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, uint32_t yDst, uint32_t ySrc)		{
		for(uint32_t x = 0, xMax = src.width (); x < xMax; ++x) {
			const uint32_t													xDst						= x + dstOffset.x;
			if(xDst < dst.width())
				dst[yDst][xDst]												= src[ySrc][x];
		}
		return 0;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							objBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstOffset.y;
			if(yDst < dst.height())
				elementsCopied												+= ::cho::objBltRow(dst, src, dstOffset, yDst, y);
		}
		return elementsCopied;
	}

	template<typename _tCell>
						::cho::error_t							objBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src)		{
		uint32_t														elementsCopied				= 0;
		for(uint32_t y = 0, yMax = ::cho::min(dst.height(), src.height()); y < yMax; ++y)
		for(uint32_t x = 0, xMax = ::cho::min(dst.width (), src.width ()); x < xMax; ++x) 
			dst[y][x]													= src[y][x];
		return elementsCopied;
	}


}

#endif // CHO_GRID_VIEW_H_3423423
