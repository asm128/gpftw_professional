#include "cho_coord.h"
#include "cho_array.h"

#ifndef GAME_H_982374982374
#define GAME_H_982374982374

#pragma pack(push, 1)

enum WEAPON_TYPE : int8_t
	{ WEAPON_TYPE_0
	, WEAPON_TYPE_1
	, WEAPON_TYPE_2
	, WEAPON_TYPE_3
	, WEAPON_TYPE_4
	, WEAPON_TYPE_5
	, WEAPON_TYPE_COUNT
	, WEAPON_TYPE_INVALID		= -1
	};

struct SWeapon {
						float													Delay;
						float													Speed;
						WEAPON_TYPE												Type;
};

struct SShipState {
						bool													Thrust										: 1;
						bool													Firing										: 1;
						bool													Brakes										: 1;
};

struct SProjectile {
						float													Delay;
						float													Speed;
						double													TimeLived;
						WEAPON_TYPE												Type;
};

static constexpr	const int32_t											MAX_PLAYERS									= 2;
static constexpr	const int32_t											MAX_POWERUP									= 32;
static constexpr	const int32_t											MAX_ENEMIES									= 64;
#pragma pack(pop)

static constexpr	const ::cho::SCoord2<float>								g_pathEnemy0		[4]						= 
							{ { 10.f,  10.f}
							, {320.f, 180.f}
							, { 50.f, 200.f}
							, {480.f,  10.f}
							};

static constexpr	const ::cho::SCoord2<float>								g_pathEnemy1		[4]						= 
							{ { 10.f,  10.f}
							, {180.f, 320.f}
							, {200.f,  50.f}
							, {10.f,  480.f}
							};


struct SGame {
						::SShipState											ShipStates			[MAX_PLAYERS]			= {};
						::SWeapon												ShipWeapon			[MAX_PLAYERS]			= {};
						::cho::SCoord2<float>									ShipPosition		[MAX_PLAYERS]			= {};
						::cho::SCoord2<float>									ShipDirection		[MAX_PLAYERS]			= {};
						::cho::SCoord2<float>									CrosshairPosition	[MAX_PLAYERS]			= {};
						bool													ShipLineOfFire		[MAX_PLAYERS]			= {};
						double													ShipWeaponDelay		[MAX_PLAYERS]			= {};

						float													HalfWidthShip								= 5;
						float													HalfWidthCrosshair							= 5;
						float													HalfWidthEnemy								= 5;
						float													HalfWidthPowerup							= 5;

						::cho::SCoord2<float>									EnemyPosition		[MAX_ENEMIES]			= {};
						::cho::SCoord2<float>									EnemyDirection		[MAX_ENEMIES]			= {};
						double													EnemyTimeLived		[MAX_ENEMIES]			= {};
						double													EnemySkillTimer		[MAX_ENEMIES]			= {};
						int8_t													EnemyTarget			[MAX_ENEMIES]			= {};
						::SWeapon												EnemyWeapon			[MAX_ENEMIES]			= {};
						uint32_t												EnemyPathStep		[MAX_ENEMIES]			= {};
						uint32_t												CountEnemies								= 0;

						::cho::SCoord2<float>									PositionPowerup								= {};

						double													GhostTimer									= 0;
						::SWeapon												Laser										= {.10f, 2000};
						uint32_t												PathStep									= 0;
						::cho::SCoord2<float>									PathEnemy			[4]						= 
							{ { 10.f,  10.f}
							, {320.f, 180.f}
							, { 50.f, 200.f}
							, {480.f,  10.f}
							};
};

#endif // GAME_H_982374982374
