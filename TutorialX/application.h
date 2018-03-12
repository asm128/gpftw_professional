#include "cho_framework.h"
#include "cho_gui.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SApplication {
						::cho::SFramework								Framework									;

						::cho::STexture<::cho::SColorBGRA>				TextureFont									= {};
						::cho::STextureMonochrome<uint32_t>				TextureFontMonochrome						= {};
						::cho::SGUI										GUI											= {};
						::cho::STriangle3D	<float>						CubePositions[12]							= {};

																		SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
