#include "cho_particle.h"
#include "cho_gui.h"
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

enum PARTICLE_TYPE : int8_t
	{	PARTICLE_TYPE_SNOW		= 0
	,	PARTICLE_TYPE_FIRE
	,	PARTICLE_TYPE_LAVA
	,	PARTICLE_TYPE_RAIN
	,	PARTICLE_TYPE_COUNT
	,	PARTICLE_TYPE_INVALID	= -1
	};

typedef				::cho::SParticleSystem<::PARTICLE_TYPE, float>		TParticleSystem;

struct SApplication {
						::cho::SDisplay										MainDisplay									= {};
						::cho::SRuntimeValues								RuntimeValues								= {};
						::cho::SInput										SystemInput									= {};
						::cho::STimer										Timer										= {};
						::cho::SFrameInfo									FrameInfo									= {};

						::TParticleSystem									ParticleSystem								= {};

						::cho::STexture<::cho::SColorBGRA>					BitmapOffscreen								= {};

																			SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: RuntimeValues(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
