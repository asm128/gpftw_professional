#include "cho_particle.h"
#include "cho_framework.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

enum PARTICLE_TYPE : int8_t
	{	PARTICLE_TYPE_SNOW			= 0
	,	PARTICLE_TYPE_SHIP_THRUST
	,	PARTICLE_TYPE_STAR
	,	PARTICLE_TYPE_LASER
	,	PARTICLE_TYPE_ROCK
	,	PARTICLE_TYPE_ARROW
	,	PARTICLE_TYPE_POISON
	,	PARTICLE_TYPE_FIREBALL
	,	PARTICLE_TYPE_BULLET
	,	PARTICLE_TYPE_PLASMA
	,	PARTICLE_TYPE_BOMB
	,	PARTICLE_TYPE_DEBRIS
	,	PARTICLE_TYPE_COUNT
	,	PARTICLE_TYPE_INVALID		= -1
	};

struct SGameParticle {
						PARTICLE_TYPE											Type										= (PARTICLE_TYPE)-1;
						float													TimeLived									= 0;
						bool													Lit											= true;
};

struct SParticleToDraw {
						int32_t													Id;
						int32_t													IndexParticle;
						float													TimeLived;
						::cho::SCoord2<int32_t>									Position;
						bool													Lit;
};

struct SLaserToDraw {
						int32_t													Id;
						int32_t													IndexParticle;
						::cho::SLine2D<float>									Segment;
};

struct SStuffToDraw {
						::cho::array_pod<::SLaserToDraw>						ProjectilePaths								= {};
						::cho::array_pod<::SParticleToDraw>						Stars										= {};
						::cho::array_pod<::SParticleToDraw>						Thrust										= {};
						::cho::array_pod<::SParticleToDraw>						Debris										= {};
						::cho::array_pod<::cho::SCoord2<float>>					CollisionPoints								= {};
};

struct SWeapon {
						float													Delay;
						float													Speed;
};

struct SShipState {
						bool													Thrust										: 1;
						bool													Firing										: 1;
						bool													Brakes										: 1;
};

struct SApplication {
	typedef				::cho::SParticleSystem<::SGameParticle, float>			TParticleSystem;

						::cho::SFramework										Framework									;

						TParticleSystem											ParticleSystem								= {};

						::cho::SColorBGRA										ColorBackground								= ::cho::SColorFloat(.15f, .15f, .15f, 1.0f);

						::cho::STextureProcessable<::cho::SColorBGRA>			TextureShip									= {};
						::cho::STextureProcessable<::cho::SColorBGRA>			TexturePowerup								= {};
						::cho::STextureProcessable<::cho::SColorBGRA>			TextureCrosshair							= {};

						::cho::SCoord2<int32_t>									TextureCenterShip							= {};
						::cho::SCoord2<int32_t>									TextureCenterPowerup						= {};
						::cho::SCoord2<int32_t>									TextureCenterCrosshair						= {};
						::cho::SCoord2<int32_t>									PSOffsetFromShipCenter						= {};

						::cho::SCoord2<float>									DirectionShip								= {};

						::cho::SCoord2<float>									PositionShip							= {};
						::cho::SCoord2<float>									PositionPowerup						= {};
						::cho::SCoord2<float>									PositionCrosshair						= {};

						::SStuffToDraw											StuffToDraw									= {};
						::cho::array_pod<::cho::SCoord2<int32_t>>				CacheLinePoints								= {};

						::SShipState											ShipState									= {false, false};
						::SWeapon												Laser										= {.15f, 2000};

																				SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
