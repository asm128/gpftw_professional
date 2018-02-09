#include "cho_particle.h"
#include "cho_framework.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

enum PARTICLE_TYPE : int8_t
	{	PARTICLE_TYPE_SNOW		= 0
	,	PARTICLE_TYPE_SHIP_THRUST
	,	PARTICLE_TYPE_STAR
	,	PARTICLE_TYPE_RAIN
	,	PARTICLE_TYPE_FIRE
	,	PARTICLE_TYPE_LAVA
	,	PARTICLE_TYPE_COUNT
	,	PARTICLE_TYPE_INVALID	= -1
	};

struct SApplication {
	typedef				::cho::SParticleSystem<::PARTICLE_TYPE, float>		TParticleSystem;

						::cho::SFramework									Framework									;

						TParticleSystem										ParticleSystem								= {};
						::cho::STextureProcessable<::cho::SColorBGRA>		TextureShip									= {};
						::cho::STextureProcessable<::cho::SColorBGRA>		TexturePowerup								= {};
						::cho::STextureProcessable<::cho::SColorBGRA>		TexturePS									= {};
						::cho::SCoord2<int32_t>								TextureCenterPS								= {};
						::cho::SCoord2<int32_t>								PSPositionInTexture							= {};
						::cho::SCoord2<int32_t>								PSOffsetFromShipCenter						= {};
						::cho::SCoord2<int32_t>								TextureCenterShip							= {};
						::cho::SCoord2<int32_t>								TextureCenterPowerup						= {};
						::cho::SCoord2<float>								CenterPositionShip							= {};
						::cho::SCoord2<float>								CenterPositionPowerup						= {};

																			SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
