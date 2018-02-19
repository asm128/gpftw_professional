// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"
#include "cho_collision.h"

					::cho::error_t										updateInput									(::SApplication& applicationInstance)											{ 
	::cho::SInput																& inputSystem								= applicationInstance.Framework.SystemInput;
	::SGame																		& gameInstance								= applicationInstance.Game;
	::SShipState																& ship0State								= gameInstance.Ships.States[0];
	ship0State.Firing														= inputSystem.KeyboardCurrent.KeyState['T'] != 0;
	ship0State.Thrust														= inputSystem.KeyboardCurrent.KeyState['Y'] != 0;
	ship0State.Brakes														= inputSystem.KeyboardCurrent.KeyState['U'] != 0;

	::SShipState																& ship1State								= gameInstance.Ships.States[1];
	ship1State.Firing														= inputSystem.KeyboardCurrent.KeyState[VK_NUMPAD1] != 0;
	ship1State.Thrust														= inputSystem.KeyboardCurrent.KeyState[VK_NUMPAD2] != 0;
	ship1State.Brakes														= inputSystem.KeyboardCurrent.KeyState[VK_NUMPAD3] != 0;

	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip)
		gameInstance.Ships.Direction[iShip]										= {};

	::cho::SCoord2<float>														& ship0Direction							= gameInstance.Ships.Direction[0];
	::cho::SCoord2<float>														& ship1Direction							= gameInstance.Ships.Direction[1];
	if(inputSystem.KeyboardCurrent.KeyState['W'			]) ship0Direction.y	+= 1;
	if(inputSystem.KeyboardCurrent.KeyState['S'			]) ship0Direction.y	-= 1;
	if(inputSystem.KeyboardCurrent.KeyState['D'			]) ship0Direction.x	+= 1;
	if(inputSystem.KeyboardCurrent.KeyState['A'			]) ship0Direction.x	-= 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_UP		]) ship1Direction.y	+= 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_DOWN		]) ship1Direction.y	-= 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_RIGHT	]) ship1Direction.x	+= 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_LEFT		]) ship1Direction.x	-= 1;
	for(uint32_t iWeaponSelect = 0; iWeaponSelect < WEAPON_TYPE_COUNT; ++iWeaponSelect) {
		if(inputSystem.KeyboardCurrent.KeyState['1' + iWeaponSelect]) {
			gameInstance.Ships.Weapon[0].Type										= (WEAPON_TYPE)iWeaponSelect;
			break;
		}
	}

	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip)
		gameInstance.Ships.Direction[iShip].Normalize();
	return 0;
}

struct SAABBCache {	
						::cho::SLine2D<float>								RectangleSegments	[4]						= {};
						::cho::SCoord2<float>								CollisionPoints		[4]						= {};
						bool												Collision			[4]						= {};
};

template<typename _tCoord>
static				::cho::error_t										checkLaserCollision
	( const ::cho::SLine2D<_tCoord>				& projectilePath
	, ::SAABBCache								& aabbCache
	, const cho::SCoord2<_tCoord>				& posXHair	
	, float										halfSizeBox	
	, ::cho::array_pod<cho::SCoord2<_tCoord>>	& collisionPoints
	)
{ // Check powerup
	::cho::buildAABBSegments(posXHair, halfSizeBox
		, aabbCache.RectangleSegments[0]
		, aabbCache.RectangleSegments[1]
		, aabbCache.RectangleSegments[2]
		, aabbCache.RectangleSegments[3]
		);
	::cho::error_t																result										= 0;
	for(uint32_t iSeg = 0; iSeg < 4; ++iSeg) {
		::cho::SCoord2<_tCoord>														& collision									= aabbCache.CollisionPoints[iSeg];
		if(1 == ::cho::segment_segment_intersect(projectilePath, aabbCache.RectangleSegments[iSeg], collision)) {
			bool																		bFound										= false;
			for(uint32_t iS2 = 0; iS2 < iSeg; ++iS2) {
				if(collision == aabbCache.CollisionPoints[iS2]) {
					bFound																	= true;
					info_printf("Discarded collision point.");
					break;
				}
			}
			if(false == bFound) {
				result																	= 1;
				aabbCache.Collision	[iSeg]												= true;
 				collisionPoints.push_back(collision);
			}
		}
	}
	return result;
}

template <size_t _sizeAlive>
					::cho::error_t										checkLineOfFire								(::SApplication & applicationInstance, ::SAABBCache& aabbCache, uint32_t iShip, float halfSizeBox, const ::cho::SLine2D<float>& projectilePath, const ::cho::array_static<cho::SCoord2<float>, _sizeAlive>& positions, const ::SArrayElementState<_sizeAlive>& alive)	{
	::SGame																		& gameInstance			 					= applicationInstance.Game;
	for(uint32_t iEnemy = 0; iEnemy < alive.size(); ++iEnemy) {
		if(0 == alive[iEnemy])
			continue;
		const cho::SCoord2<float>													& posEnemy									= positions[iEnemy];
		if(1 == ::checkLaserCollision(projectilePath, aabbCache, posEnemy, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints))
			gameInstance.Ships.LineOfFire[iShip]										= true;
	}
	return 0;
}
					::cho::error_t										updateShips									(::SApplication & applicationInstance)			{
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::SGame																		& gameInstance			 					= applicationInstance.Game;
	const ::cho::SCoord2<uint32_t>												& offscreenMetrics							= framework.Offscreen.View.metrics();
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) { // Update ship positions 
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		::cho::SCoord2<float>														& shipPosition								= gameInstance.Ships.Position[iShip];
		shipPosition															+= gameInstance.Ships.Direction[iShip] * (float)(framework.FrameInfo.Seconds.LastFrame * 100) * 
			(gameInstance.Ships.States[iShip].Brakes ? .25f : (gameInstance.Ships.States[iShip].Thrust ? 2 : 1));
		shipPosition.x															= ::cho::clamp(shipPosition.x, .1f, (float)offscreenMetrics.x - 1);
		shipPosition.y															= ::cho::clamp(shipPosition.y, .1f, (float)offscreenMetrics.y - 1);
		gameInstance.PositionCrosshair[iShip]									= shipPosition + ::cho::SCoord2<float>{96,};
		gameInstance.PositionCrosshair[iShip].x									= ::cho::min(gameInstance.PositionCrosshair[iShip].x, (float)offscreenMetrics.x);
	}

	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {	// --- Calculate line of fire and set state accordingly. This causes Draw() to draw the crosshair in the right mode.
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		::cho::SCoord2<float>														& shipPosition								= gameInstance.Ships.Position[iShip];
		::SAABBCache																aabbCache;
		::cho::SLine2D<float>														projectilePath								= {shipPosition, shipPosition + ::cho::SCoord2<float>{10000, }};
		projectilePath.A.y														-= 1;
		projectilePath.B.y														-= 1;
		gameInstance.Ships.LineOfFire[iShip]										= false;
		::checkLineOfFire(applicationInstance, aabbCache, iShip, gameInstance.HalfWidthEnemy	, projectilePath, gameInstance.Enemies	.Position, gameInstance.Enemies	.Alive);
		::checkLineOfFire(applicationInstance, aabbCache, iShip, gameInstance.HalfWidthPowerup	, projectilePath, gameInstance.Powerups	.Position, gameInstance.Powerups.Alive);
	}
	return 0;
}

					::cho::error_t										updateParticles								(::SApplication& applicationInstance)											{ 
	typedef	::SApplication::TParticleInstance									TParticleInstance;
	::SApplication::TIntegrator													& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	::cho::SFramework															& framework									= applicationInstance.Framework;
	const float																	lastFrameSeconds							= (float)framework.FrameInfo.Seconds.LastFrame;
	ree_if(errored(particleIntegrator.Integrate(lastFrameSeconds, framework.FrameInfo.Seconds.LastFrameHalfSquared)), "Not sure why would this fail.");

	const float																	windDirection								= (float)sin(framework.FrameInfo.Seconds.Total/3.0f) * .025f;
	::cho::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystem.Instances;
	::cho::clear
		( applicationInstance.StuffToDraw.ProjectilePaths
		, applicationInstance.StuffToDraw.CollisionPoints	
		, applicationInstance.StuffToDraw.Debris			
		, applicationInstance.StuffToDraw.Thrust			
		, applicationInstance.StuffToDraw.Stars				
		);

	for(uint32_t iParticle = 0; iParticle < particleInstances.size(); ++iParticle) {
		typedef	::SApplication::TParticle											TParticle;
		TParticleInstance															& particleInstance							= particleInstances[iParticle];
		TParticle																	& particleNext								= particleIntegrator.ParticleNext	[particleInstance.ParticleIndex];
		TParticle																	& particleCurrent							= particleIntegrator.Particle		[particleInstance.ParticleIndex];
		const bool																	nextPosOutOfRange								 
			= (	((uint32_t)particleNext.Position.x) >= framework.Offscreen.View.width	()
			||	((uint32_t)particleNext.Position.y) >= framework.Offscreen.View.height	()
			);
		const bool																	currentPosOutOfRange								 
			= (	((uint32_t)particleCurrent.Position.x) >= framework.Offscreen.View.width	()
			||	((uint32_t)particleCurrent.Position.y) >= framework.Offscreen.View.height	()
			);
		const bool																	instanceTimeout									= (particleInstance.Type.TimeLived >= .125 && particleInstance.Type.Type == PARTICLE_TYPE_SHIP_THRUST);
		if((currentPosOutOfRange && nextPosOutOfRange) || instanceTimeout) { // Remove the particle instance and related information.
			particleIntegrator.ParticleState[particleInstance.ParticleIndex].Unused	= true;
			ree_if(errored(particleInstances.remove(iParticle)), "Not sure why would this fail.");
			--iParticle;
		}
	}

	for(uint32_t iParticle = 0; iParticle < particleInstances.size(); ++iParticle) {
		TParticleInstance															& particleInstance							= particleInstances[iParticle];
		int32_t																		physicsId									= particleInstance.ParticleIndex;
		typedef	::SApplication::TParticle											TParticle;
		TParticle																	& particleNext								= particleIntegrator.ParticleNext[physicsId];
		TParticle																	& particleCurrent							= particleIntegrator.Particle[physicsId];
		if(particleInstance.Type.Type == PARTICLE_TYPE_PROJECTILE) {
			const ::SLaserToDraw														laserToDraw									= {physicsId, (int32_t)iParticle, ::cho::SLine2D<float>{particleCurrent.Position, particleNext.Position}, ::cho::LIGHTCYAN};
			applicationInstance.StuffToDraw.ProjectilePaths.push_back(laserToDraw);
		}
		::SParticleToDraw															particleToDraw								= {physicsId, (int32_t)iParticle, particleInstance.Type.TimeLived, particleCurrent.Position.Cast<int32_t>()};
			 if(particleInstance.Type.Type == PARTICLE_TYPE_STAR		)	applicationInstance.StuffToDraw.Stars	.push_back(particleToDraw);
		else if(particleInstance.Type.Type == PARTICLE_TYPE_SHIP_THRUST	)	applicationInstance.StuffToDraw.Thrust	.push_back(particleToDraw);
		else if(particleInstance.Type.Type == PARTICLE_TYPE_DEBRIS		)	applicationInstance.StuffToDraw.Debris	.push_back(particleToDraw);
		particleInstance.Type.TimeLived											+= lastFrameSeconds;
		particleCurrent															= particleNext;
	}
	return 0;
}

template<typename _tParticleType>
static				::cho::error_t										addParticle														
	(	::SGameParticle												particleType
	,	::cho::array_pod<::cho::SParticleInstance<_tParticleType>>	& particleInstances
	,	::SApplication::TIntegrator									& particleIntegrator
	,	const ::cho::SCoord2<float>									& particlePosition
	,	const ::cho::SCoord2<float>									& particleDirection
	,	float														speed
	,	const ::cho::array_view<::SApplication::TParticle>			& particleDefinitions
	)														
{
	::cho::SParticleInstance<_tParticleType>									& newInstance								= particleInstances[::cho::addParticle(particleType, particleInstances, particleIntegrator, particleDefinitions[particleType.Type])]; 
	::SApplication::TParticle													& newParticle								= particleIntegrator.Particle[newInstance.ParticleIndex];
	newParticle.Position													= particlePosition; 
	::cho::SCoord2<float>														newDirection								= particleDirection;
	const float																	value										= .5;
	switch(particleType.Type) {
	default							: break;
	case ::PARTICLE_TYPE_SHIP_THRUST: 
		newParticle.Position.y													+= rand() % 3 - 1;
		newDirection.Rotate(((rand() % 32767) / 32766.0f) * value - value / 2);
		break;
	}
	newParticle.Forces.Velocity												= newDirection * speed;	//{ -, (float)((rand() % 31 * 4) - 15 * 4)};
	return 0;
}

static				::cho::error_t										addProjectile								(::SGame & gameInstance, int32_t iShip, PLAYER_TYPE playerType, WEAPON_TYPE weaponType, float projectileSpeed)					{
	::cho::bit_array_view<uint64_t>												& projectilesAlive							= gameInstance.Projectiles.Alive;
	for(uint32_t iProjectile = 0, projectileCount = projectilesAlive.size(); iProjectile < projectileCount; ++iProjectile) {
		if(0 == projectilesAlive[iProjectile]) {
			projectilesAlive[iProjectile]											= 1;
			::SProjectile																& projectile							= gameInstance.Projectiles.Projectiles[iProjectile];
			projectile.TypeWeapon													= weaponType;
			projectile.TypePlayer													= playerType;
			projectile.ShipIndex													= iShip;
			projectile.TimeLived													= 0;
			projectile.Speed														= projectileSpeed;
			return iProjectile;
		}
	}
	return -1;	// Projectile storage is full. Cannot add projectile.
}

template <size_t _sizeAlive>
static				::cho::error_t										updateSpawnShots							
	( ::SApplication												& applicationInstance
	, const ::cho::array_view<::SApplication::TParticle>			& particleDefinitions
	, uint32_t														maxShips
	, const ::cho::array_static<cho::SCoord2<float>, _sizeAlive>	& positions
	, ::cho::array_static<double, _sizeAlive>						& weaponDelay
	, const ::cho::array_static<::SWeapon, _sizeAlive>				& weapons
	, const ::cho::array_static<::SShipState, _sizeAlive>			& shipState
	, const ::SArrayElementState<_sizeAlive>						& alive
	, PLAYER_TYPE													playerType
	) 
{
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::SGame																		& gameInstance								= applicationInstance.Game;
	::cho::array_pod<::SApplication::TParticleInstance>							& particleInstances							= applicationInstance.ParticleSystem.Instances;
	::SApplication::TIntegrator													& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	for(uint32_t iShip = 0, shipCount = maxShips; iShip < shipCount; ++iShip) {
		if(0 == alive[iShip])
			continue;
		uint32_t																	textureIndex								= (playerType == PLAYER_TYPE_PLAYER) ? GAME_TEXTURE_SHIP0 + iShip : GAME_TEXTURE_ENEMY;
		weaponDelay[iShip]														+= framework.FrameInfo.Seconds.LastFrame;
		if(shipState[iShip].Firing) { // Add lasers / bullets.
			const ::SWeapon																weapon										= weapons[iShip];
			if( weaponDelay[iShip] >= weapon.Delay ) {
				weaponDelay[iShip]														= 0;
				::SGameParticle																gameParticle;
				gameParticle.OwnerIndex													= iShip;
				gameParticle.TimeLived													= 0;
				gameParticle.Type														= PARTICLE_TYPE_PROJECTILE;
				gameParticle.Lit														= true;
				gameParticle.TypePlayer													= playerType;
				gameParticle.TypeWeapon													= weapons[iShip].Type;
				const ::cho::SCoord2<float>													textureShipMetrics							= applicationInstance.Textures[textureIndex].Processed.View.metrics().Cast<float>();
				const ::cho::SCoord2<float>													weaponParticleOffset						= {textureShipMetrics.x - (textureShipMetrics.x - applicationInstance.TextureCenters[textureIndex].x), -1};
				const ::cho::SCoord2<float>													shotDirection								= (playerType == PLAYER_TYPE_PLAYER) ? ::cho::SCoord2<float>{1.0f, 0.0f} : 
					(gameInstance.Ships.Position[rand() % gameInstance.ShipsPlaying] - gameInstance.Enemies.Position[iShip]).Normalize();
				::addParticle(gameParticle, particleInstances, particleIntegrator, positions[iShip] + weaponParticleOffset, shotDirection, weapon.Speed, particleDefinitions);
				e_if(errored(::addProjectile(gameInstance, iShip, gameParticle.TypePlayer, gameParticle.TypeWeapon, weapon.Speed)), "Projectile storage is full. Cannot add projectile.");
			}
		}
	}
	return 0;
}

					::cho::error_t										updateSpawn									
	( ::SApplication										& applicationInstance
	, const ::cho::array_view<::SApplication::TParticle>	& particleDefinitions
	)
{
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::SFramework::TOffscreen												& offscreen									= framework.Offscreen;
	// Add some effect particles
	::cho::array_pod<::SApplication::TParticleInstance>							& particleInstances							= applicationInstance.ParticleSystem.Instances;
	::SApplication::TIntegrator													& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	applicationInstance.EffectsDelay.Thrust									+= framework.FrameInfo.Seconds.LastFrame;
	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		if(applicationInstance.EffectsDelay.Thrust > .01) { // Add thrust particles.
			for(int32_t i = 0, particleCountToSpawn = 1 + rand() % 4; i < particleCountToSpawn; ++i) {
				::SGameParticle																gameParticle;
				gameParticle.OwnerIndex													= iShip;
				gameParticle.TimeLived													= 0;
				gameParticle.Type														= PARTICLE_TYPE_SHIP_THRUST;
				gameParticle.TypePlayer													= PLAYER_TYPE_PLAYER;
				gameParticle.Lit														= 0 == (rand() % 2);
				::addParticle(gameParticle, particleInstances, particleIntegrator, gameInstance.Ships.Position[iShip] + applicationInstance.PSOffsetFromShipCenter.Cast<float>(), gameInstance.Ships.Direction[iShip] * -1.0, (float)(rand() % 400) + (gameInstance.Ships.States[iShip].Thrust ? 400 : 0), particleDefinitions);
			}
		}
	}
	::updateSpawnShots(applicationInstance, particleDefinitions, gameInstance.ShipsPlaying			, gameInstance.Ships	.Position, gameInstance.Ships	.WeaponDelay, gameInstance.Ships	.Weapon, gameInstance.Ships		.States, gameInstance.Ships		.Alive, PLAYER_TYPE_PLAYER	);
	::updateSpawnShots(applicationInstance, particleDefinitions, gameInstance.Enemies.Alive.size()	, gameInstance.Enemies	.Position, gameInstance.Enemies	.WeaponDelay, gameInstance.Enemies	.Weapon, gameInstance.Enemies	.States, gameInstance.Enemies	.Alive, PLAYER_TYPE_ENEMY	);
	applicationInstance.EffectsDelay.Star									+= framework.FrameInfo.Seconds.LastFrame;
	if(applicationInstance.EffectsDelay.Star > .1) {
		applicationInstance.EffectsDelay.Star									= 0;
		bool																		bFastStarFromThrust							= false;
		for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {
			if(0 == gameInstance.Ships.Alive[iShip])
				continue;
			if (gameInstance.Ships.States[iShip].Thrust) {
				bFastStarFromThrust														= true;
				break;
			}
		}
		::SGameParticle																gameParticle;
		gameParticle.TimeLived													= 0;
		gameParticle.Type														= PARTICLE_TYPE_STAR;
		gameParticle.Lit														= 0 == rand() % 3;
		::addParticle(gameParticle, particleInstances, particleIntegrator, {offscreen.View.metrics().x - 1.0f, (float)(rand() % offscreen.View.metrics().y)}, {-1, 0}, (float)(rand() % (bFastStarFromThrust ? 400 : 75)) + 25, particleDefinitions);
	}
	return 0;
}

					::cho::error_t										updateShots
	( ::SApplication										& applicationInstance
	, const ::cho::array_view<::SApplication::TParticle>	& particleDefinitions
	)
{ 
	::SGame																		& gameInstance								= applicationInstance.Game;
	::cho::bit_array_view<uint64_t>												& projectilesAlive							= gameInstance.Projectiles.Alive;
	for(uint32_t iProjectile = 0, projectileCount = projectilesAlive.size(); iProjectile < projectileCount; ++iProjectile) {
		if(0 == projectilesAlive[iProjectile]) 
			continue;
		::SProjectile																& projectile								= gameInstance.Projectiles.Projectiles[iProjectile];
		projectile.TimeLived													+= applicationInstance.Framework.FrameInfo.Seconds.LastFrame;
		if(projectile.TimeLived > 0.01) 
			projectilesAlive[iProjectile]											= 0;
	}
	applicationInstance.StuffToDraw.CollisionPoints.clear();
	::SAABBCache																aabbCache;
	::cho::array_pod<::SApplication::TParticleInstance>							& particleInstances							= applicationInstance.ParticleSystem.Instances;
	for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
		const ::SLaserToDraw														& laserToDraw								= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
		const ::cho::SLine2D<float>													& projectilePath							= laserToDraw.Segment;
		for(uint32_t iPow = 0; iPow < gameInstance.Powerups.Alive.size(); ++iPow) { // Check powerup
			if(0 == gameInstance.Powerups.Alive[iPow])
				continue;
			const cho::SCoord2<float>													& posPowerup								= gameInstance.Powerups.Position[iPow];
			float																		halfSizeBox									= (float)applicationInstance.TextureCenters[GAME_TEXTURE_POWERUP0].x;
			if(1 == ::checkLaserCollision(projectilePath, aabbCache, posPowerup, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints)) {
			
			}
		}
		const ::SApplication::TParticleInstance										& particleInstance							= particleInstances[laserToDraw.IndexParticle];
		::SWeapon																	& weapon									= (particleInstance.Type.TypePlayer == PLAYER_TYPE_PLAYER)
			? gameInstance.Ships	.Weapon[particleInstance.Type.OwnerIndex]
			: gameInstance.Enemies	.Weapon[particleInstance.Type.OwnerIndex]
			;
		if(particleInstance.Type.TypePlayer == PLAYER_TYPE_PLAYER) {
			for(uint32_t iEnemy = 0; iEnemy < gameInstance.Enemies.Alive.size(); ++iEnemy) { // Check enemy
				if(0 == gameInstance.Enemies.Alive[iEnemy])
					continue;
				const cho::SCoord2<float>													& posEnemy									= gameInstance.Enemies.Position[iEnemy];
				float																		halfSizeBox									= (float)applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY].x;
				::SHealthPoints																& enemyHealth								= gameInstance.Enemies.Health[iEnemy];
				if(1 == ::checkLaserCollision(projectilePath, aabbCache, posEnemy, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints)) {
					float																		damegePorportion							= ::cho::max(.5f, (rand() % 5001) / 5000.0f);
					enemyHealth.Health														-= (int32_t)(weapon.Speed * (1.0f - damegePorportion));
					enemyHealth.Shield														-= (int32_t)(weapon.Speed * damegePorportion);
					if(enemyHealth.Health < 0) enemyHealth.Health = 0;
					if(enemyHealth.Shield < 0) enemyHealth.Shield = 0;
				}
				if(0 >= enemyHealth.Health) {
					gameInstance.Enemies.Alive[iEnemy] = 0;
					continue;
				}
				static constexpr const ::cho::SCoord2<float>								reference									= {1, 0};
				::cho::SCoord2<float>														vector;
				for(uint32_t iGhost = 0; iGhost < 5; ++iGhost) {
					vector																	= reference * (64 * sin(applicationInstance.Framework.FrameInfo.Seconds.Total));
					vector.Rotate(::cho::math_2pi / 5 * iGhost + gameInstance.GhostTimer);
					::checkLaserCollision(projectilePath, aabbCache, posEnemy + vector, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints);
				}
			}
		} 
		else {
			for(uint32_t iShip = 0; iShip < gameInstance.Ships.Alive.size(); ++iShip) { // Check enemy
				if(0 == gameInstance.Ships.Alive[iShip])
					continue;
				const cho::SCoord2<float>													& posEnemy									= gameInstance.Ships.Position[iShip];
				float																		halfSizeBox									= (float)applicationInstance.TextureCenters[GAME_TEXTURE_SHIP0 + iShip].x;
				::SHealthPoints																& enemyHealth								= gameInstance.Ships.Health[iShip];
				if(1 == ::checkLaserCollision(projectilePath, aabbCache, posEnemy, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints)) {
					float																		damegePorportion							= ::cho::max(.5f, (rand() % 5001) / 5000.0f);
					enemyHealth.Health														-= (int32_t)(weapon.Speed * (1.0f - damegePorportion));
					enemyHealth.Shield														-= (int32_t)(weapon.Speed * damegePorportion);
					if(enemyHealth.Health < 0) enemyHealth.Health = 0;
					if(enemyHealth.Shield < 0) enemyHealth.Shield = 0;
				}
				if(0 >= enemyHealth.Health) {
					gameInstance.Ships.Alive[iShip] = 0;
					continue;
				}
			}
		}
	}
	::SApplication::TIntegrator													& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	for(uint32_t iCollision = 0, collisionCount = applicationInstance.StuffToDraw.CollisionPoints.size(); iCollision < collisionCount; ++iCollision)
		for(uint32_t i=0; i < 10; ++i) {
			::cho::SCoord2<float>	angle	= {(float)-(rand() % 20) - 10, (float)(rand() % 20 - 1 - 10)};
			angle.Normalize();
			::SGameParticle																gameParticle;
			gameParticle.TimeLived													= 0;
			gameParticle.Type														= PARTICLE_TYPE_DEBRIS;
			gameParticle.Lit														= 0 == (rand()% 2);
			gameParticle.TypePlayer													= PLAYER_TYPE_PLAYER;
			::addParticle(gameParticle, particleInstances, particleIntegrator, applicationInstance.StuffToDraw.CollisionPoints[iCollision], angle, (float)(rand() % 400) + 100, particleDefinitions);
		}
	return 0;
}

					::cho::error_t										updateGUI									(::SApplication& applicationInstance)					{ 
	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {	//  ------ update crosshair collision points with lasers 
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		const cho::SCoord2<float>													& posXHair									= gameInstance.PositionCrosshair[iShip];
		for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
			const ::SLaserToDraw														& laserToDraw								= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
			const ::SApplication::TParticleInstance										& particleInstance							= applicationInstance.ParticleSystem.Instances[laserToDraw.IndexParticle];
			if(particleInstance.Type.OwnerIndex != iShip || particleInstance.Type.TypePlayer != PLAYER_TYPE_PLAYER)
				continue;
			float																		halfSizeBox									= gameInstance.HalfWidthCrosshair;
			const ::cho::SLine2D<float>													verticalSegments[]							= 
				{ {posXHair + ::cho::SCoord2<float>{ halfSizeBox - 1, halfSizeBox - 1}, posXHair + ::cho::SCoord2<float>{ halfSizeBox - 1	,-halfSizeBox}}
				, {posXHair + ::cho::SCoord2<float>{-halfSizeBox	, halfSizeBox - 1}, posXHair + ::cho::SCoord2<float>{-halfSizeBox		,-halfSizeBox}}
				};
			const ::cho::SLine2D<float>													& projectilePath							= laserToDraw.Segment;
			::cho::SCoord2<float>														collisions	[::cho::size(verticalSegments)]= {};
			for(uint32_t iSeg = 0; iSeg < ::cho::size(verticalSegments); ++iSeg) {
				::cho::SCoord2<float>														& collision									= collisions		[iSeg];
				const ::cho::SLine2D<float>													& segSelected								= verticalSegments	[iSeg]; 
				if(::cho::line_line_intersect(projectilePath, segSelected, collision)) {
					bool																		bFound										= false;
					for(uint32_t iS2 = 0; iS2 < iSeg; ++iS2) {
						if(collision == collisions[iS2]) {
							bFound																	= true;
							info_printf("Discarded collision point.");
							break;
						}
					}
					if(false == bFound)
 						applicationInstance.StuffToDraw.CollisionPoints.push_back(collision);
				}
			}
		}
	}
	return 0;
}

					::cho::error_t										updateEnemies								(::SApplication & applicationInstance)			{
	::cho::SFramework															& framework									= applicationInstance.Framework;
	const ::cho::SCoord2<uint32_t>												& offscreenMetrics							= framework.Offscreen.View.metrics();
	::SGame																		& gameInstance								= applicationInstance.Game;
	gameInstance.GhostTimer													+= framework.FrameInfo.Seconds.LastFrame;
	static float																timerSpawn									= 0;
	timerSpawn																+= (float)framework.FrameInfo.Seconds.LastFrame;
	if(timerSpawn > 10) {
		timerSpawn																= 0;
		int32_t																		indexToSpawnEnemy								= firstUnused(gameInstance.Enemies.Alive);
		if(indexToSpawnEnemy != -1) {
			gameInstance.Enemies.Alive			[indexToSpawnEnemy]					= 1;
			gameInstance.Enemies.Position		[indexToSpawnEnemy]					= {offscreenMetrics.x - 200.0f, (float)offscreenMetrics.y};//{(float)(rand() % offscreenMetrics.x), (float)(rand() % offscreenMetrics.y)};
			gameInstance.Enemies.Health			[indexToSpawnEnemy]					= {5000, 5000};
			gameInstance.Enemies.Weapon			[indexToSpawnEnemy]					= {2, 100, WEAPON_TYPE(rand()% WEAPON_TYPE_COUNT)};
			gameInstance.Enemies.WeaponDelay	[indexToSpawnEnemy]					= 0;
			gameInstance.Enemies.States			[indexToSpawnEnemy].Firing			= true;
		}
		else
			warning_printf("Not enough space in enemy container to spawn more enemies!");	
		int32_t																		indexToSpawnPow								= firstUnused(gameInstance.Powerups.Alive);
		if(indexToSpawnPow != -1 && (0 == (rand() % 5))) {
			gameInstance.Powerups.Alive			[indexToSpawnPow]					= 1;
			gameInstance.Powerups.Position		[indexToSpawnPow]					= gameInstance.Enemies.Position[0];
			gameInstance.Powerups.Family		[indexToSpawnPow]					= (POWERUP_FAMILY)(rand() % POWERUP_FAMILY_COUNT);
		}
		else
			warning_printf("Not enough space in enemy container to spawn more enemies!");	
	}
	for(uint32_t iEnemy = 0; iEnemy < gameInstance.Enemies.Alive.size(); ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		float																		& timerPath									= gameInstance.Enemies.TimerPath[iEnemy];
		gameInstance.Enemies.SkillTimer[iEnemy]									+= framework.FrameInfo.Seconds.LastFrame;
		timerPath																+= (float)framework.Timer.LastTimeSeconds;
		uint32_t																	& enemyPathStep								= gameInstance.Enemies.PathStep[iEnemy];
		if(timerPath > 10.0f) {
			timerPath																= 0;
			++enemyPathStep;
			if( enemyPathStep >= ::cho::size(gameInstance.PathEnemy) )
				enemyPathStep										= 0;
		}
		{
			::cho::SCoord2<float>														& enemyPosition								= gameInstance.Enemies.Position[iEnemy];
			const ::cho::SCoord2<float>													& pathTarget								= gameInstance.PathEnemy[enemyPathStep];
			::cho::SCoord2<float>														directionEnemy								= (pathTarget - enemyPosition);
			if(directionEnemy.LengthSquared() < 0.5) {
				timerPath																= 0;
				++enemyPathStep;
				if( enemyPathStep >= ::cho::size(gameInstance.PathEnemy) )
					enemyPathStep															= 0;
			}
			else {
				directionEnemy.Normalize();
				::cho::SFramework::TOffscreen												& offscreen									= framework.Offscreen;
				gameInstance.Enemies.Position[iEnemy]									+= directionEnemy * (float)(framework.FrameInfo.Seconds.LastFrame * 100);// * (applicationInstance.ShipState.Brakes ? .25f : (applicationInstance.ShipState.Thrust ? 2 : 1));
				gameInstance.Enemies.Position[iEnemy]									= 
					{ ::cho::clamp(enemyPosition.x, .1f, (float)offscreen.View.metrics().x - 1)
					, ::cho::clamp(enemyPosition.y, .1f, (float)offscreen.View.metrics().y - 1)
					};
			}
		}
	}	return 0;
}