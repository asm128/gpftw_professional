#include "cho_grid_view.h"		// for grid_view<>
#include "cho_ascii_target.h"	// for SASCIITarget

#ifndef CHO_ASCII_DISPLAY_H_34234234
#define CHO_ASCII_DISPLAY_H_34234234

namespace cho
{
					::cho::error_t									asciiDisplayCreate					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::cho::error_t									asciiDisplayDestroy					();
					::cho::error_t									asciiDisplayPresent					(const ::cho::array_view<const uint8_t>& characters, const ::cho::array_view<const uint16_t>& colorRefs);
					::cho::error_t									asciiDisplayClear					(uint8_t character = ' ', uint16_t colorRef = ::cho::ASCII_COLOR_WHITE);
					::cho::error_t									asciiDisplaySize					(::cho::SCoord2<uint32_t>& size);

					::cho::error_t									asciiDisplayResize					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::cho::error_t									asciiDisplayPaletteSet				(const ::cho::array_view<::cho::SColorRGBA>& palette);
					::cho::error_t									asciiDisplayPaletteReset			();
					::cho::error_t									asciiDisplayTitleSet				(const ::cho::array_view<const char>& title);

	template <typename _tUnit>
	static inline	::cho::error_t									asciiDisplayCreate					(const ::cho::SCoord2<_tUnit>& displaySize)				{ return ::chol::asciiDisplayCreate(displaySize.x, displaySize.y); }
	static inline	::cho::error_t									asciiDisplayPresent					(const ::cho::SASCIITarget& renderTarget)				{
		return ::cho::asciiDisplayPresent
			( ::cho::array_view<const uint8_t >{renderTarget.Characters	.begin(), renderTarget.Characters	.size()}
			, ::cho::array_view<const uint16_t>{renderTarget.Colors		.begin(), renderTarget.Colors		.size()}
			);
	}
}

#endif // CHO_ASCII_DISPLAY_H_34234234
