// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "cho_particle.h"
#include "cho_framework.h"
#include "game.h"
#include "spawner.h"
#include "track.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

enum PARTICLE_TYPE : int8_t
	{	PARTICLE_TYPE_SNOW			= 0
	,	PARTICLE_TYPE_SHIP_THRUST
	,	PARTICLE_TYPE_STAR
	,	PARTICLE_TYPE_PROJECTILE
	,	PARTICLE_TYPE_BOMB
	,	PARTICLE_TYPE_DEBRIS
	,	PARTICLE_TYPE_COUNT
	,	PARTICLE_TYPE_INVALID		= -1
	};

#pragma pack(push, 1)
struct SGameParticle {
						PARTICLE_TYPE																				Type										= (PARTICLE_TYPE)-1;
						PLAYER_TYPE																					TypePlayer									= (PLAYER_TYPE	)-1;
						WEAPON_TYPE																					TypeWeapon									= (WEAPON_TYPE	)-1;
						int8_t																						IndexWeapon									= -1;
						int8_t																						Padding										= 0;
						uint32_t																					OwnerIndex									= (uint32_t		)-1;
						float																						TimeLived									= 0;
						bool																						Lit											= true;
};

struct SParticleToDraw {
						int32_t																						IndexParticlePhysics;
						int32_t																						IndexParticleInstance;
						float																						TimeLived;
						::cho::SCoord2<int32_t>																		Position;
};

struct SLaserToDraw {
						int32_t																						IndexParticlePhysics;
						int32_t																						IndexParticleInstance;
						::cho::SLine2D<float>																		Segment;
};
#pragma pack(pop)

struct SStuffToDraw {
						::cho::array_pod<::SLaserToDraw>															ProjectilePaths								= {};
						::cho::array_pod<::SParticleToDraw>															Stars										= {};
						::cho::array_pod<::SParticleToDraw>															Thrust										= {};
						::cho::array_pod<::SParticleToDraw>															Debris										= {};
						::cho::array_pod<::cho::SCoord2<float>>														CollisionPoints								= {};

						::cho::array_obj<::cho::grid_view<::cho::SColorBGRA>>										TexturesPowerup0							= {};
						::cho::array_obj<::cho::grid_view<::cho::SColorBGRA>>										TexturesPowerup1							= {};
};

enum GAME_TEXTURE : int8_t
	{ GAME_TEXTURE_FONT_ATLAS		= 0
	, GAME_TEXTURE_SHIP0			
	, GAME_TEXTURE_SHIP1		
	, GAME_TEXTURE_POWCORESQUARE		
	, GAME_TEXTURE_POWCOREDIAGONAL	
	, GAME_TEXTURE_CROSSHAIR	
	, GAME_TEXTURE_POWICON		
	, GAME_TEXTURE_ENEMY		
	, GAME_TEXTURE_COUNT	
	, GAME_TEXTURE_INVALID			= -1	
	};

struct SEffectsDelay {
						double																						Thrust;
						double																						Star;
};

static constexpr	const ::cho::SCoord2<uint32_t>																GAME_SCREEN_SIZE							= {1280, 720};
			  
struct SLevelState {
	int32_t							Number;
	float							Time;
};

struct SApplication {
	typedef				::cho::SParticleSystem<::SGameParticle, float>												TParticleSystem;
	typedef				TParticleSystem::TParticleInstance															TParticleInstance;
	typedef				TParticleSystem::TIntegrator																TIntegrator;
	typedef				TIntegrator::TParticle																		TParticle;

						::cho::SFramework																			Framework									;

						::SGame																						Game										= {};
						::SLevelState																				Level										= {0, 0};
						bool																						Playing										= true;
						bool																						Paused										= true;
						bool																						Debugging									= true;

						TParticleSystem																				ParticleSystemThrust						= {};
						TParticleSystem																				ParticleSystemProjectiles					= {};
						TParticleSystem																				ParticleSystemDebris						= {};
						TParticleSystem																				ParticleSystemStars							= {};

						::cho::SColorBGRA																			ColorBackground								= ::cho::SColorFloat(.15f, .15f, .15f, 1.0f);

						::cho::array_static<::cho::STextureProcessable<::cho::SColorBGRA>	, GAME_TEXTURE_COUNT>	Textures									= {};
						::cho::array_static<::cho::SCoord2<int32_t>							, GAME_TEXTURE_COUNT>	TextureCenters								= {};
						::cho::SCoord2<int32_t>																		PSOffsetFromShipCenter						= {};
						::cho::STextureMonochrome<uint32_t>															TextureFontMonochrome						= {};

						::SStuffToDraw																				StuffToDraw									= {};
						::cho::array_pod<::cho::SCoord2<int32_t>>													CacheLinePoints								= {};
						::SEffectsDelay																				EffectsDelay								= {};

																													SApplication								(::cho::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
