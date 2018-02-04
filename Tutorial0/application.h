#include "cho_ascii_display.h"
#include "cho_frameinfo.h"
#include "cho_timer.h"
#include "cho_array.h"
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

						::cho::array_pod<::cho::SColorBGRA>			BitmapOffscreen								= {};
						::cho::array_pod<::cho::SColorBGRA>			TextureFont									= {};
						::cho::array_pod<::cho::SColorBGRA>			TextureBackgroundDay						= {};
						::cho::array_pod<::cho::SColorBGRA>			TextureBackgroundNight						= {};

						::cho::grid_view<::cho::SColorBGRA>			ViewBitmapOffscreen							= {};
						::cho::grid_view<::cho::SColorBGRA>			ViewTextureFont								= {};
						::cho::grid_view<::cho::SColorBGRA>			ViewTextureBackgroundDay					= {};
						::cho::grid_view<::cho::SColorBGRA>			ViewTextureBackgroundNight					= {};

																	SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: RuntimeValues(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
