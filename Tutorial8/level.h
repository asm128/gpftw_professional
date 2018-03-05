#include "cho_array.h"
#include "enemy.h"

#ifndef LEVEL_H_2908374982374
#define LEVEL_H_2908374982374

struct SSpawner {
	float							SpawnDelay;
	float							TimeLived;
	bool							Loop;
	bool							Enemy;
	WEAPON_TYPE						TypeWeapon;
	SHIP_CLASS						TypeShip;
};

typedef ::cho::array_pod<SSpawner>	SLevel;

//struct SLevel {
//	::cho::array_pod<SSpawner>		SpawnerSpwaners;
//};

#endif // LEVEL_H_2908374982374
