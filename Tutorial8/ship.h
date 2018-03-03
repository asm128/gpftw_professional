#include "cho_array_view.h"

#ifndef SHIP_H_2098379238
#define SHIP_H_2098379238

enum SHIP_CLASS : int8_t
	{ SHIP_CLASS_ASSAULT
	, SHIP_CLASS_INTERCEPTOR
	, SHIP_CLASS_SAUCER
	, SHIP_CLASS_SPAWNER
	, SHIP_CLASS_MOTHER
	, SHIP_CLASS_MONSTER
	, SHIP_CLASS_COUNT
	};

struct SShipProperties {
						::cho::view_const_string				Name;		
						SHIP_CLASS								Class;
						float									Speed;
						int32_t									Health;
						int32_t									Shield;
};

struct SShipState {
						bool									Thrust										: 1;
						bool									Firing										: 1;
						bool									Brakes										: 1;
};

static constexpr	const int								MAX_PLAYER_SHIPS							= 50;
static constexpr	const SShipProperties					shipProperties		[]						= 
	{ {"Assault"			, SHIP_CLASS_ASSAULT		, 10,    80,   100}
	, {"Interceptor"		, SHIP_CLASS_INTERCEPTOR	, 10,   100,    80}
	, {"Alien Saucer"		, SHIP_CLASS_SAUCER			, 10,   100,   100}
	, {"Mothership"			, SHIP_CLASS_MOTHER			, 10,  1000,  1000}
	, {"Monster Ship"		, SHIP_CLASS_MONSTER		, 10, 10000, 10000}
	, {"Factory Pod"		, SHIP_CLASS_SPAWNER		, 10,    50,   100}
	, {"Wormhole Endpoint"	, SHIP_CLASS_SPAWNER		, 10, 10000,     0}
	};

#endif // SHIP_H_2098379238
