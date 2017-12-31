#include "cho_grid_view.h"		// for grid_view<>
#include "cho_ascii_target.h"	// for SASCIITarget

#ifndef CHO_ASCII_DISPLAY_H_34234234
#define CHO_ASCII_DISPLAY_H_34234234

namespace cho
{
					::cho::error_t									asciiDisplayCreate					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::cho::error_t									asciiDisplayDestroy					();
					::cho::error_t									asciiDisplayPresent					(const ::cho::view_const_uint8& characters, const ::cho::view_const_uint16& colorRefs);
					::cho::error_t									asciiDisplayClear					(uint8_t character = ' ', uint16_t colorRef = ::cho::ASCII_COLOR_WHITE);
					::cho::error_t									asciiDisplaySize					(::cho::SCoord2<uint32_t>& size);

					::cho::error_t									asciiDisplayResize					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::cho::error_t									asciiDisplayPaletteSet				(const ::cho::array_view<::cho::SColorRGBA>& palette);
					::cho::error_t									asciiDisplayPaletteReset			();
					::cho::error_t									asciiDisplayTitleSet				(const ::cho::view_const_string& title);

	template <typename _tUnit>
	static inline	::cho::error_t									asciiDisplayCreate					(const ::cho::SCoord2<_tUnit>& displaySize)				{ return ::cho::asciiDisplayCreate(displaySize.x, displaySize.y); }
	static inline	::cho::error_t									asciiDisplayPresent					(const ::cho::SASCIITarget& renderTarget)				{
		return ::cho::asciiDisplayPresent
			( ::cho::view_const_uint8 {renderTarget.Characters	.begin(), renderTarget.Characters	.size()}
			, ::cho::view_const_uint16{renderTarget.Colors		.begin(), renderTarget.Colors		.size()}
			);
	}
}

#endif // CHO_ASCII_DISPLAY_H_34234234
