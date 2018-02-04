#include "cho_ascii_display.h"
#include "cho_frameinfo.h"
#include "cho_timer.h"
#include "cho_input.h"
#include "cho_display.h"
#include "cho_runtime.h"
#include "cho_grid.h"
#include "cho_texture.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SApplication {
						::cho::SDisplay								MainDisplay									= {};
						::cho::SRuntimeValues						RuntimeValues								= {};
						::cho::SInput								SystemInput									= {};
						::cho::STimer								Timer										= {};
						::cho::SFrameInfo							FrameInfo									= {};

						::cho::STexture<::cho::SColorBGRA>			BitmapOffscreen								= {};
						::cho::STexture<::cho::SColorBGRA>			TextureFont									= {};
						::cho::STexture<::cho::SColorBGRA>			TextureBackgroundDay						= {};
						::cho::STexture<::cho::SColorBGRA>			TextureBackgroundNight						= {};

																	SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: RuntimeValues(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
