#include "cho_gui.h"
#include "cho_ascii_display.h"
#include "cho_frameinfo.h"
#include "cho_timer.h"
#include "cho_input.h"
#include "cho_display.h"
#include "cho_runtime.h"
#include "cho_grid.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SApplication {
						::cho::SDisplay								MainDisplay									= {};
						::cho::SRuntimeValues						RuntimeValues								= {};
						::cho::SInput								SystemInput									= {};
						::cho::STimer								Timer										= {};
						::cho::SFrameInfo							FrameInfo									= {};

						::cho::array_pod<::cho::SColorBGRA>			OffscreenBitmap								= {};
						::cho::array_pod<uint8_t>					OffscreenStencil							= {};
						::cho::array_pod<::cho::SColorBGRA>			TextureBackgroundDay						= {};
						::cho::array_pod<::cho::SColorBGRA>			TextureBackgroundNight						= {};
						::cho::array_pod<::cho::SColorBGRA>			TextureBackgroundScaledDay					= {};
						::cho::array_pod<::cho::SColorBGRA>			TextureBackgroundScaledNight				= {};

						::cho::grid_view<::cho::SColorBGRA>			ViewOffscreenBitmap							= {};
						::cho::grid_view<::cho::SColorBGRA>			ViewOffscreenStencil						= {};
						::cho::grid_view<::cho::SColorBGRA>			ViewTextureBackgroundDay					= {};
						::cho::grid_view<::cho::SColorBGRA>			ViewTextureBackgroundNight					= {};
						::cho::grid_view<::cho::SColorBGRA>			ViewTextureBackgroundScaledDay				= {};
						::cho::grid_view<::cho::SColorBGRA>			ViewTextureBackgroundScaledNight			= {};

																	SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: RuntimeValues(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
