#include "cho_particle.h"
#include "cho_framework.h"
#include "game.h"

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
						::cho::SColorBGRA										Color;
};

struct SStuffToDraw {
						::cho::array_pod<::SLaserToDraw>						ProjectilePaths								= {};
						::cho::array_pod<::SParticleToDraw>						Stars										= {};
						::cho::array_pod<::SParticleToDraw>						Thrust										= {};
						::cho::array_pod<::SParticleToDraw>						Debris										= {};
						::cho::array_pod<::cho::SCoord2<float>>					CollisionPoints								= {};

						::cho::array_obj<::cho::grid_view<::cho::SColorBGRA>>	TexturesPowerup0							= {};
						::cho::array_obj<::cho::grid_view<::cho::SColorBGRA>>	TexturesPowerup1							= {};
};

enum GAME_TEXTURE : int8_t
	{ GAME_TEXTURE_SHIP			
	, GAME_TEXTURE_POWERUP0		
	, GAME_TEXTURE_POWERUP1		
	, GAME_TEXTURE_CROSSHAIR	
	, GAME_TEXTURE_POWICON		
	, GAME_TEXTURE_ENEMY		
	, GAME_TEXTURE_COUNT	
	, GAME_TEXTURE_INVALID		= -1	
	};

struct SApplication {
	typedef				::cho::SParticleSystem<::PARTICLE_TYPE, float>			TParticleSystem;

						::cho::SFramework										Framework									;

						::SGame													Game;

						TParticleSystem											ParticleSystem								= {};

						::cho::SColorBGRA										ColorBackground								= ::cho::SColorFloat(.15f, .15f, .15f, 1.0f);

						::cho::STextureProcessable<::cho::SColorBGRA>			Textures		[GAME_TEXTURE_COUNT]		= {};
						::cho::SCoord2<int32_t>									TextureCenters	[GAME_TEXTURE_COUNT]		= {};

						::cho::STextureProcessable<::cho::SColorBGRA>			TextureShip									= {};
						::cho::SCoord2<int32_t>									PSOffsetFromShipCenter						= {};

						::SStuffToDraw											StuffToDraw									= {};
						::cho::array_pod<::cho::SCoord2<int32_t>>				CacheLinePoints								= {};


																				SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
