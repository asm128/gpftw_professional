#include "cho_array_view.h"

#ifndef WEAPON_H_298364239
#define WEAPON_H_298364239

enum WEAPON_TYPE : int8_t
	{ WEAPON_TYPE_LASER					= 0
	, WEAPON_TYPE_PLASMA
	, WEAPON_TYPE_SPARK
	, WEAPON_TYPE_BULLET
	, WEAPON_TYPE_ARROW
	, WEAPON_TYPE_POISON
	, WEAPON_TYPE_BUBBLE
	, WEAPON_TYPE_FIRE
	, WEAPON_TYPE_ROCK
	, WEAPON_TYPE_COUNT
	, WEAPON_TYPE_INVALID				= -1
	};

enum SHOT_TYPE 
	{	SHOT_TYPE_SINGLE
	,	SHOT_TYPE_SPLASH
	,	SHOT_TYPE_OMNI
	,	SHOT_TYPE_HOMING
	,	SHOT_TYPE_GRAVITY
	,	SHOT_TYPE_BUBBLE
	,	SHOT_TYPE_COUNT
	};

struct SWeaponProperties {
						::cho::view_const_string				Name;		
						WEAPON_TYPE								TypeWeapon;
						SHOT_TYPE								TypeShot;
						float									Speed;
						float									Delay;
						int32_t									Damage;
};

static constexpr	const SWeaponProperties					weaponProperties []					=   
	{	{"Bow"				, WEAPON_TYPE_ARROW		, SHOT_TYPE_SINGLE, 1000.f, 0.4f, 1500}
	,	{"Gun"				, WEAPON_TYPE_BULLET	, SHOT_TYPE_SINGLE, 1000.f, 0.2f, 1000}
	,	{"Laser Gun"		, WEAPON_TYPE_LASER		, SHOT_TYPE_SINGLE, 1000.f, 0.2f, 1000}
	,	{"Plasma Gun"		, WEAPON_TYPE_PLASMA	, SHOT_TYPE_SINGLE, 1000.f, 0.3f, 1500}
	,	{"Bubble Gun"		, WEAPON_TYPE_BUBBLE	, SHOT_TYPE_SINGLE, 1000.f, 0.2f, 1000}
	,	{"Electricity Gun"	, WEAPON_TYPE_SPARK		, SHOT_TYPE_SINGLE, 1000.f, 0.2f, 1000}
	,	{"Radiation Gun"	, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.2f, 1000}
	,	{"Fire Gun"			, WEAPON_TYPE_POISON	, SHOT_TYPE_SINGLE, 1000.f, 0.1f,  750}
	};

#endif // WEAPON_H_298364239