#include "cho_framework.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SApplication {
						::cho::SFramework							Framework									;

						::cho::STexture<::cho::SColorBGRA>			TextureFont									= {};
						::cho::STexture<::cho::SColorBGRA>			TextureBackgroundDay						= {};
						::cho::STexture<::cho::SColorBGRA>			TextureBackgroundNight						= {};

																	SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
