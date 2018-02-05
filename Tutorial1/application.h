#include "cho_framework.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SApplication {
						::cho::SFramework								Framework									;

						::cho::STexture<::cho::SColorBGRA>				TextureFont									= {};
						::cho::STextureProcessable<::cho::SColorBGRA>	TextureBackgroundDay						= {};
						::cho::STextureProcessable<::cho::SColorBGRA>	TextureBackgroundNight						= {};
						::cho::SRectangle2D<uint32_t>					Rectangle									= {};

																		SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
