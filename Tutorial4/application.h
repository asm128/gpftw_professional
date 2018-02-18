#include "cho_particle.h"
#include "cho_framework.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

enum PARTICLE_TYPE : int8_t
	{	PARTICLE_TYPE_SHIP_THRUST	= 0
	,	PARTICLE_TYPE_STAR
	,	PARTICLE_TYPE_LASER
	,	PARTICLE_TYPE_COUNT
	,	PARTICLE_TYPE_INVALID		= -1
	};

struct SGameParticle {
						PARTICLE_TYPE											Type										= (PARTICLE_TYPE)-1;
						float													TimeLived									= 0;
						bool													Lit											= true;
};

struct SApplication {
	typedef				::cho::SParticleSystem<::SGameParticle, float>		TParticleSystem;

						::cho::SFramework									Framework									;

						TParticleSystem										ParticleSystem								= {};

						::cho::SColorBGRA									ColorBackground								= ::cho::SColorFloat(.15f, .15f, .15f, 1.0f);

						::cho::STextureProcessable<::cho::SColorBGRA>		TextureShip									= {};
						::cho::STextureProcessable<::cho::SColorBGRA>		TexturePowerup								= {};
						::cho::STextureProcessable<::cho::SColorBGRA>		TextureCrosshair[6]							= {};
						::cho::SCoord2<int32_t>								PSOffsetFromShipCenter						= {};

						::cho::SCoord2<int32_t>								TextureCenterShip							= {};
						::cho::SCoord2<int32_t>								TextureCenterPowerup						= {};
						::cho::SCoord2<int32_t>								TextureCenterCrosshair						= {};

						::cho::SCoord2<float>								DirectionShip								= {};

						::cho::SCoord2<float>								PositionShip								= {};
						::cho::SCoord2<float>								PositionPowerup								= {};
						::cho::SCoord2<float>								PositionCrosshair							= {};

						bool												TurboShip									= false;
						bool												Firing										= false;

																			SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
