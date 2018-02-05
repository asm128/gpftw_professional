#include "cho_framework.h"
#include "cho_particle.h"

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

struct SApplication {
	typedef				::cho::SParticleSystem<::PARTICLE_TYPE, float>		TParticleSystem;

						::cho::SFramework									Framework;
						TParticleSystem										ParticleSystem								= {};

																			SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
