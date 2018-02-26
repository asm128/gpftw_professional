#include "cho_framework.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SApplication {
						::cho::SFramework								Framework									;

						::cho::STexture<::cho::SColorBGRA>				TextureFont									= {};
						::cho::STextureMonochrome<uint32_t>				TextureFontMonochrome						= {};

																		SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
