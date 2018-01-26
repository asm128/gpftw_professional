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
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SRectangle2D<_tCoord>& srcRect)		{
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstRect.Offset.y;
			const uint32_t													ySrc						= y + srcRect.Offset.y;
			if(yDst >= dst.height() || yDst >= (uint32_t)(dstRect.Offset.y + dstRect.Size.y))
				continue;
			if(ySrc >= src.height() || ySrc >= (uint32_t)(srcRect.Offset.y + srcRect.Size.y))
				continue;
			for(uint32_t x = 0, xMax = src.width (); x < xMax; ++x) {
				const uint32_t													xDst						= x + dstRect.Offset.x;
				const uint32_t													xSrc						= x + srcRect.Offset.x;
				if(xDst >= dst.width() || xDst >= (uint32_t)(dstRect.Offset.x + dstRect.Size.x))
					continue;
				if(xSrc >= src.width() || xSrc >= (uint32_t)(srcRect.Offset.x + srcRect.Size.x))
					continue;
				dst[yDst][xDst]												= src[ySrc][xSrc];
			}
		}
		return 0;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SRectangle2D<_tCoord>& srcRect)		{
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstOffset.y;
			const uint32_t													ySrc						= y + srcRect.Offset.y;
			if(yDst >= dst.height())
				continue;
			if(ySrc >= src.height() || ySrc >= (uint32_t)(srcRect.Offset.y + srcRect.Size.y))
				continue;
			for(uint32_t x = 0, xMax = src.width (); x < xMax; ++x) {
				const uint32_t													xDst						= x + dstOffset.x;
				const uint32_t													xSrc						= x + srcRect.Offset.x;
				if(xDst >= dst.width())
					continue;
				if(xSrc >= src.width() || xSrc >= (uint32_t)(srcRect.Offset.x + srcRect.Size.x))
					continue;
				dst[yDst][xDst]												= src[ySrc][xSrc];
			}
		}
		return 0;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect, const ::cho::SCoord2<_tCoord>& srcOffset)		{
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstRect.Offset.y;
			const uint32_t													ySrc						= y + srcOffset.y;
			if(yDst >= dst.height() || yDst >= (uint32_t)(dstRect.Offset.y + dstRect.Size.y))
				continue;
			if(ySrc >= src.height())
				continue;
			for(uint32_t x = 0, xMax = src.width (); x < xMax; ++x) {
				const uint32_t													xDst						= x + dstRect.Offset.x;
				const uint32_t													xSrc						= x + srcOffset.x;
				if(xDst >= dst.width() || xDst >= (uint32_t)(dstRect.Offset.x + dstRect.Size.x))
					continue;
				if(xSrc >= src.width())
					continue;
				dst[yDst][xDst]												= src[ySrc][xSrc];
			}
		}
		return 0;
	}

						template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SRectangle2D<_tCoord>& dstRect)		{
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstRect.Offset.y;
			if(yDst >= dst.height() || yDst >= (uint32_t)(dstRect.Offset.y + dstRect.Size.y))
				continue;
			for(uint32_t x = 0, xMax = src.width (); x < xMax; ++x) {
				const uint32_t													xDst						= x + dstRect.Offset.x;
				if(xDst >= dst.width() || xDst >= (uint32_t)(dstRect.Offset.x + dstRect.Size.x))
					continue;
				dst[yDst][xDst]												= src[y][x];
			}
		}
		return 0;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset, const ::cho::SCoord2<_tCoord>& srcOffset)		{
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstOffset.y;
			const uint32_t													ySrc						= y + srcOffset.y;
			if(yDst >= dst.height())
				continue;
			if(ySrc >= src.height())
				continue;
			for(uint32_t x = 0, xMax = src.width (); x < xMax; ++x) {
				const uint32_t													xDst						= x + dstOffset.x;
				const uint32_t													xSrc						= x + srcOffset.x;
				if(xDst >= dst.width())
					continue;
				if(xSrc >= src.width())
					continue;
				dst[yDst][xDst]												= src[ySrc][xSrc];
			}
		}
		return 0;
	}

	template<typename _tCell, typename _tCoord>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src, const ::cho::SCoord2<_tCoord>& dstOffset)		{
		for(uint32_t y = 0, yMax = src.height(); y < yMax; ++y) {
			const uint32_t													yDst						= y + dstOffset.y;
			if(yDst >= dst.height())
				continue;
			for(uint32_t x = 0, xMax = src.width (); x < xMax; ++x) {
				const uint32_t													xDst						= x + dstOffset.x;
				if(xDst >= dst.width())
					continue;
				dst[yDst][xDst]												= src[y][x];
			}
		}
		return 0;
	}

	template<typename _tCell>
						::cho::error_t							bitBlt						(::cho::grid_view<_tCell>& dst, const ::cho::grid_view<_tCell>& src)		{
		for(uint32_t y = 0, yMax = ::cho::min(dst.height(), src.height()); y < yMax; ++y)
		for(uint32_t x = 0, xMax = ::cho::min(dst.width (), src.width ()); x < xMax; ++x) 
			dst[y][x]													= src[y][x];
		return 0;
	}

}

#endif // CHO_GRID_VIEW_H_3423423