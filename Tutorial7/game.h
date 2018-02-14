#include "cho_coord.h"

#ifndef GAME_H_982374982374
#define GAME_H_982374982374

#pragma pack(push, 1)
struct SWeapon {
						float													Delay;
						float													Speed;
};

struct SShipState {
						bool													Thrust										: 1;
						bool													Firing										: 1;
						bool													Brakes										: 1;
};
#pragma pack(pop)

struct SGame {
						::cho::SCoord2<float>									PositionShip								= {};
						::cho::SCoord2<float>									PositionPowerup								= {};
						::cho::SCoord2<float>									PositionCrosshair							= {};
						::cho::SCoord2<float>									PositionEnemy								= {};
						::cho::SCoord2<float>									DirectionShip								= {};
						::SWeapon												Laser										= {.10f, 2000};
						double													GhostTimer									= 0;
						bool													LineOfFire									= false;
						::SShipState											ShipState									= {false, false};
						uint32_t												PathStep									= 0;
						::cho::SCoord2<float>									PathEnemy	[4]								= 
							{ { 10.f,  10.f}
							, {320.f, 180.f}
							, { 50.f, 200.f}
							, {480.f,  10.f}
							};
};

#endif // GAME_H_982374982374
