// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "cho_coord.h"
#include "cho_bit_array_view.h"
#include "cho_array_static.h"

#ifndef GAME_H_982374982374
#define GAME_H_982374982374

#pragma pack(push, 1)

enum PLAYER_TYPE : int8_t 
	{ PLAYER_TYPE_PLAYER				= 0
	, PLAYER_TYPE_ENEMY
	, PLAYER_TYPE_COUNT
	, PLAYER_TYPE_INVALID				= -1
	};

enum WEAPON_TYPE : int8_t
	{ WEAPON_TYPE_LASER					= 0
	, WEAPON_TYPE_PLASMA
	, WEAPON_TYPE_SPARK
	, WEAPON_TYPE_BULLET
	, WEAPON_TYPE_ARROW
	, WEAPON_TYPE_POISON
	, WEAPON_TYPE_ROCK
	, WEAPON_TYPE_COUNT
	, WEAPON_TYPE_INVALID				= -1
	};

enum HEALTH_TYPE : int8_t
	{ HEALTH_TYPE_HEALTH				= 0
	, HEALTH_TYPE_SHIELD
	, HEALTH_TYPE_LIFE
	, HEALTH_TYPE_WEAPON_LEVEL
	, HEALTH_TYPE_SHIP_LEVEL
	, HEALTH_TYPE_COUNT
	, HEALTH_TYPE_INVALID				= -1
	};

enum BUFF_TYPE : int8_t
	{ BUFF_TYPE_POINTS					= 0
	, BUFF_TYPE_FORCE_FIELD					
	, BUFF_TYPE_FIRE_RATIO
	, BUFF_TYPE_COUNT
	, BUFF_TYPE_INVALID					= -1
	};

enum POWERUP_FAMILY : int8_t
	{ POWERUP_FAMILY_WEAPON				= 0
	, POWERUP_FAMILY_HEALTH
	, POWERUP_FAMILY_BUFF
	, POWERUP_FAMILY_BOMB
	, POWERUP_FAMILY_COUNT
	, POWERUP_FAMILY_INVALID			= -1
	};

struct SPowerup {
						BUFF_TYPE																TypeBuff																= BUFF_TYPE_INVALID;
						HEALTH_TYPE																TypeHealth																= HEALTH_TYPE_INVALID;
						WEAPON_TYPE																TypeWeapon																= WEAPON_TYPE_INVALID;
};

struct SWeapon {
						float																	Delay;
						float																	Speed;
						WEAPON_TYPE																Type;
};

struct SShipState {
						bool																	Thrust																	: 1;
						bool																	Firing																	: 1;
						bool																	Brakes																	: 1;
};

struct SHealthPoints {
						int32_t																	Health;
						int32_t																	Shield;
};

struct SProjectile {
						float																	Speed;
						double																	TimeLived;
						WEAPON_TYPE																TypeWeapon;
						PLAYER_TYPE																TypePlayer;
						int32_t																	ShipIndex;
};

static constexpr	const int32_t															MAX_PLAYERS																= 4;
static constexpr	const int32_t															MAX_POWERUP																= 64;
static constexpr	const int32_t															MAX_ENEMIES																= 128;
static constexpr	const int32_t															MAX_PROJECTILES															= 512;

template<size_t _sizeArray>
struct SArrayElementState : public ::cho::bit_array_view<uint64_t> { 
public:
	typedef				uint64_t																TStorage;
						TStorage																Data				[(_sizeArray / (8 * sizeof(TStorage))) + 1]			= {};	

																								SArrayElementState														()						: bit_array_view(Data, _sizeArray) {}
};
#pragma pack(pop)

template<size_t _sizeArray>
					::cho::error_t															firstUnused																(const ::SArrayElementState<_sizeArray>& container)			{
	for(uint32_t iObject = 0; iObject < _sizeArray; ++iObject)
		if(0 == container[iObject])
			return iObject;
	return -1;						
}

static constexpr	const uint32_t															MAGIC_NUMBER															= 1397704771;

template<uint32_t _sizeArray>	
struct SPropertiesPowerup		{ 
						::SArrayElementState						< _sizeArray>				Alive																	= {};
						::cho::array_static<::cho::SCoord2<float>	, _sizeArray>				Position																= {};
						::cho::array_static<POWERUP_FAMILY			, _sizeArray>				Family																	= {};
						::cho::array_static<::SPowerup				, _sizeArray>				Type																	= {};
};

template<uint32_t _sizeArray>	
struct SPropertiesProjectile	{ 
						::SArrayElementState						< _sizeArray>				Alive																	= {};
						::cho::array_static<::SProjectile			, MAX_PROJECTILES>			Projectiles																= {};
						::cho::array_static<::SHealthPoints			, _sizeArray>				Health																	= {};
};

template<uint32_t _sizeArray>	
struct SPropertiesEnemy			{ 
						::SArrayElementState						< _sizeArray>				Alive																	= {};
						::cho::array_static<::SShipState			, _sizeArray>				States																	= {};
						::cho::array_static<::cho::SCoord2<float>	, _sizeArray>				Position																= {};
						::cho::array_static<::cho::SCoord2<float>	, _sizeArray>				Direction																= {};
						::cho::array_static<::SHealthPoints			, _sizeArray>				Health																	= {};
						::cho::array_static<double					, _sizeArray>				TimeLived																= {};
						::cho::array_static<double					, _sizeArray>				SkillTimer																= {};
						::cho::array_static<int8_t					, _sizeArray>				Target																	= {};
						::cho::array_static<::SWeapon				, _sizeArray>				Weapon																	= {};
						::cho::array_static<double					, _sizeArray>				WeaponDelay																= {};
						::cho::array_static<uint32_t				, _sizeArray>				PathStep																= {};
						::cho::array_static<float					, _sizeArray>				TimerPath																= {};
};

template<uint32_t _sizeArray>	
struct SPropertiesShip			{ 		
						::SArrayElementState						< _sizeArray>				Alive																	= {};			
						::cho::array_static<::SShipState			, _sizeArray>				States																	= {};
						::cho::array_static<::SHealthPoints			, _sizeArray>				Health																	= {};
						::cho::array_static<::SWeapon				, _sizeArray>				Weapon																	= {};
						::cho::array_static<::cho::SCoord2<float>	, _sizeArray>				Position																= {};
						::cho::array_static<::cho::SCoord2<float>	, _sizeArray>				Direction																= {};
						::cho::array_static<bool					, _sizeArray>				LineOfFire																= {};
						::cho::array_static<double					, _sizeArray>				WeaponDelay																= {};
};

struct SGame {
						uint32_t																ShipsPlaying															= 2;
						::SPropertiesShip		<MAX_PLAYERS>									Ships																	= {};
						::SPropertiesEnemy		<MAX_ENEMIES>									Enemies																	= {};
						::SPropertiesProjectile	<MAX_PROJECTILES>								Projectiles																= {};
						::SPropertiesPowerup	<MAX_POWERUP>									Powerups																= {};
						::cho::array_static<::cho::SCoord2<float>, MAX_PLAYERS>					PositionCrosshair														= {};
						uint32_t																CountEnemies															= 0;
						uint32_t																CountProjectiles														= 0;
						uint32_t																CountPowerups															= 0;

						float																	HalfWidthShip															= 5;
						float																	HalfWidthCrosshair														= 5;
						float																	HalfWidthEnemy															= 5;
						float																	HalfWidthPowerup														= 5;

						double																	GhostTimer																= 0;
						uint32_t																PathStep																= 0;
						::cho::SCoord2<float>													PathEnemy					[10]										= 
							{ { 10.f,  10.f}
							, {320.f, 180.f}
							, { 50.f, 200.f}
							, {480.f,  10.f}
							, {150.f,  20.f}
							, {180.f, 320.f}
							, {200.f,  50.f}
							, { 10.f, 480.f}
							, { 50.f,  50.f}
							, { 20.f, 120.f}
							};
};

#endif // GAME_H_982374982374
