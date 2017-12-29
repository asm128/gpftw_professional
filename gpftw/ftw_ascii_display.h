#include "ftw_grid_view.h"		// for grid_view<>
#include "ftw_ascii_target.h"	// for SASCIITarget

#ifndef FTW_ASCII_DISPLAY_H_34234234
#define FTW_ASCII_DISPLAY_H_34234234

namespace ftw
{
					::ftw::error_t									asciiDisplayCreate					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::ftw::error_t									asciiDisplayDestroy					();
					::ftw::error_t									asciiDisplayPresent					(const ::ftw::array_view<const uint8_t>& characters, const ::ftw::array_view<const uint16_t>& colorRefs);
					::ftw::error_t									asciiDisplayClear					(uint8_t character = ' ', uint16_t colorRef = ::ftw::ASCII_COLOR_WHITE);
					::ftw::error_t									asciiDisplaySize					(::ftw::SCoord2<uint32_t>& size);

					::ftw::error_t									asciiDisplayResize					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::ftw::error_t									asciiDisplayPaletteSet				(const ::ftw::array_view<::ftw::SColorRGBA>& palette);
					::ftw::error_t									asciiDisplayPaletteReset			();
					::ftw::error_t									asciiDisplayTitleSet				(const ::ftw::array_view<const char>& title);

	template <typename _tUnit>
	static inline	::ftw::error_t									asciiDisplayCreate					(const ::ftw::SCoord2<_tUnit>& displaySize)				{ return ::ftwl::asciiDisplayCreate(displaySize.x, displaySize.y); }
	static inline	::ftw::error_t									asciiDisplayPresent					(const ::ftw::SASCIITarget& renderTarget)				{
		return ::ftw::asciiDisplayPresent
			( ::ftw::array_view<const uint8_t >{renderTarget.Characters	.begin(), renderTarget.Characters	.size()}
			, ::ftw::array_view<const uint16_t>{renderTarget.Colors		.begin(), renderTarget.Colors		.size()}
			);
	}
}

#endif // FTW_ASCII_DISPLAY_H_34234234
