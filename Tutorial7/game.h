#include "cho_coord.h"
#include "cho_bit_array_view.h"
#include "cho_array.h"

#ifndef GAME_H_982374982374
#define GAME_H_982374982374

#pragma pack(push, 1)

enum PLAYER_TYPE : int8_t 
	{ PLAYER_TYPE_PLAYER		= 0
	, PLAYER_TYPE_ENEMY
	, PLAYER_TYPE_COUNT
	, PLAYER_TYPE_INVALID		= -1
	};

enum WEAPON_TYPE : int8_t
	{ WEAPON_TYPE_LASER			= 0
	, WEAPON_TYPE_ROCK
	, WEAPON_TYPE_ARROW
	, WEAPON_TYPE_BULLET
	, WEAPON_TYPE_POISON
	, WEAPON_TYPE_COUNT
	, WEAPON_TYPE_INVALID		= -1
	};

struct SWeapon {
						float													Delay;
						float													Speed;
						WEAPON_TYPE												Type;
};

struct SShipState {
						bool													Thrust												: 1;
						bool													Firing												: 1;
						bool													Brakes												: 1;
};

struct SProjectile {
						float													Speed;
						double													TimeLived;
						WEAPON_TYPE												TypeWeapon;
						PLAYER_TYPE												TypePlayer;
						int32_t													ShipIndex;
};

static constexpr	const int32_t											MAX_PLAYERS											= 2;
static constexpr	const int32_t											MAX_POWERUP											= 32;
static constexpr	const int32_t											MAX_ENEMIES											= 64;
static constexpr	const int32_t											MAX_PROJECTILES										= 512;

//static constexpr	const ::cho::SCoord2<float>								g_pathEnemy0		[4]								= 
//						{ { 10.f,  10.f}
//						, {320.f, 180.f}
//						, { 50.f, 200.f}
//						, {480.f,  10.f}
//						};
//
//static constexpr	const ::cho::SCoord2<float>								g_pathEnemy1		[4]								= 
//						{ { 10.f,  10.f}
//						, {180.f, 320.f}
//						, {200.f,  50.f}
//						, {10.f,  480.f}
//						};

template<size_t _sizeArray>
struct SArrayElementState : public ::cho::bit_array_view<uint32_t> { 
public:
						uint32_t												Data			[(_sizeArray / 32) + 1]				= {};	

																				SArrayElementState									() : bit_array_view(Data) {}
};

template<size_t _sizeArray>
					::cho::error_t											firstUnused												(const ::SArrayElementState<_sizeArray>& container)			{
	for(uint32_t iObject = 0; iObject < _sizeArray; ++iObject)
		if(0 == container[iObject])
			return iObject;
	return -1;						
}

struct SGame {
						::SArrayElementState<MAX_PLAYERS>						ShipsAlive											= {};			// These have to be written/read to/from disk separately as they contain pointers.
						::SArrayElementState<MAX_ENEMIES>						EnemiesAlive										= {};			// These have to be written/read to/from disk separately as they contain pointers.
						::SArrayElementState<MAX_PROJECTILES>					ProjectilesAlive									= {};			// These have to be written/read to/from disk separately as they contain pointers.

						uint32_t												MagicNumber											= 1397704771;	// From here, the whole thing can be saved with a signle fwrite().
						uint32_t												ShipsPlaying										= 2;
						::SShipState											ShipStates					[MAX_PLAYERS]			= {};
						::SWeapon												ShipWeapon					[MAX_PLAYERS]			= {};
						::cho::SCoord2<float>									ShipPosition				[MAX_PLAYERS]			= {};
						::cho::SCoord2<float>									ShipDirection				[MAX_PLAYERS]			= {};
						::cho::SCoord2<float>									CrosshairPosition			[MAX_PLAYERS]			= {};
						bool													ShipLineOfFire				[MAX_PLAYERS]			= {};
						double													ShipWeaponDelay				[MAX_PLAYERS]			= {};


						float													HalfWidthShip										= 5;
						float													HalfWidthCrosshair									= 5;
						float													HalfWidthEnemy										= 5;
						float													HalfWidthPowerup									= 5;

						::cho::SCoord2<float>									EnemyPosition				[MAX_ENEMIES]			= {};
						::cho::SCoord2<float>									EnemyDirection				[MAX_ENEMIES]			= {};
						double													EnemyTimeLived				[MAX_ENEMIES]			= {};
						double													EnemySkillTimer				[MAX_ENEMIES]			= {};
						int8_t													EnemyTarget					[MAX_ENEMIES]			= {};
						::SWeapon												EnemyWeapon					[MAX_ENEMIES]			= {};
						uint32_t												EnemyPathStep				[MAX_ENEMIES]			= {};

						::SProjectile											Projectiles					[MAX_PROJECTILES]		= {};

						::cho::SCoord2<float>									PositionPowerup										= {};

						double													GhostTimer											= 0;
						::SWeapon												Laser												= {.10f, 2000};
						uint32_t												PathStep											= 0;
						::cho::SCoord2<float>									PathEnemy					[5]						= 
							{ { 10.f,  10.f}
							, {320.f, 180.f}
							, { 50.f, 200.f}
							, {480.f,  10.f}
							, { 0.0f,  0.0f}
							};
};

#pragma pack(pop)

#endif // GAME_H_982374982374
