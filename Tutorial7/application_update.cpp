// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"
#include "cho_collision.h"

					::cho::error_t										updateInput									(::SApplication& applicationInstance)											{ 
	::cho::SInput																& inputSystem								= applicationInstance.Framework.SystemInput;
	::SGame																		& gameInstance								= applicationInstance.Game;
	gameInstance.Ships.States[0].Firing										= inputSystem.KeyboardCurrent.KeyState['T'] != 0;
	gameInstance.Ships.States[0].Thrust										= inputSystem.KeyboardCurrent.KeyState['Y'] != 0;
	gameInstance.Ships.States[0].Brakes										= inputSystem.KeyboardCurrent.KeyState['U'] != 0;

	gameInstance.Ships.States[1].Firing										= inputSystem.KeyboardCurrent.KeyState[VK_NUMPAD1] != 0;
	gameInstance.Ships.States[1].Thrust										= inputSystem.KeyboardCurrent.KeyState[VK_NUMPAD2] != 0;
	gameInstance.Ships.States[1].Brakes										= inputSystem.KeyboardCurrent.KeyState[VK_NUMPAD3] != 0;

	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip)
		gameInstance.Ships.Direction[iShip]										= {};

	if(inputSystem.KeyboardCurrent.KeyState['W'			]) gameInstance.Ships.Direction[0].y += 1;
	if(inputSystem.KeyboardCurrent.KeyState['S'			]) gameInstance.Ships.Direction[0].y -= 1;
	if(inputSystem.KeyboardCurrent.KeyState['D'			]) gameInstance.Ships.Direction[0].x += 1;
	if(inputSystem.KeyboardCurrent.KeyState['A'			]) gameInstance.Ships.Direction[0].x -= 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_UP		]) gameInstance.Ships.Direction[1].y += 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_DOWN		]) gameInstance.Ships.Direction[1].y -= 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_RIGHT	]) gameInstance.Ships.Direction[1].x += 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_LEFT		]) gameInstance.Ships.Direction[1].x -= 1;

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
		::SAABBCache																aabbCache;
		::cho::SLine2D<float>														projectilePath								= {gameInstance.Ships.Position[iShip], gameInstance.Ships.Position[iShip] + ::cho::SCoord2<float>{10000, }};
		projectilePath.A.y														-= 1;
		projectilePath.B.y														-= 1;
		gameInstance.Ships.LineOfFire[iShip]										= false;
		for(uint32_t iEnemy = 0; iEnemy < gameInstance.Enemies.Alive.size(); ++iEnemy) {
			if(0 == gameInstance.Enemies.Alive[iEnemy])
				continue;
			const cho::SCoord2<float>													& posEnemy									= gameInstance.Enemies.Position[iEnemy];
			float																		halfSizeBox									= gameInstance.HalfWidthEnemy; //(float)applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY].x;
			if(1 == ::checkLaserCollision(projectilePath, aabbCache, posEnemy, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints))
				gameInstance.Ships.LineOfFire[iShip]										= true;
		}
		for(uint32_t iPow = 0; iPow < gameInstance.Powerups.Alive.size(); ++iPow) {
			if(0 == gameInstance.Powerups.Alive[iPow])
				continue;
			const cho::SCoord2<float>													& posPow									= gameInstance.Powerups.Position[iPow];
			float																		halfSizeBox									= gameInstance.HalfWidthPowerup;//(float)applicationInstance.TextureCenters[GAME_TEXTURE_POWERUP0].x;
			if(1 == ::checkLaserCollision(projectilePath, aabbCache, posPow, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints))
				gameInstance.Ships.LineOfFire[iShip]										= true;
		}
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
		TParticleInstance															& particleInstance							= particleInstances[iParticle];
		int32_t																		physicsId									= particleInstance.ParticleIndex;
		typedef	::SApplication::TParticle											TParticle;
		TParticle																	& particleNext								= particleIntegrator.ParticleNext[physicsId];
		TParticle																	& particleCurrent							= particleIntegrator.Particle[physicsId];
		if(particleInstance.Type.Type == PARTICLE_TYPE_PROJECTILE) {
			::SLaserToDraw																laserToDraw									= {physicsId, (int32_t)iParticle, ::cho::SLine2D<float>{particleCurrent.Position, particleNext.Position}, ::cho::LIGHTCYAN};
			applicationInstance.StuffToDraw.ProjectilePaths.push_back(laserToDraw);
		}
		if( ((uint32_t)particleNext.Position.x) >= framework.Offscreen.View.width	()
			|| ((uint32_t)particleNext.Position.y) >= framework.Offscreen.View.height	()
			|| (particleInstance.Type.TimeLived >= .125 && particleInstance.Type.Type == PARTICLE_TYPE_SHIP_THRUST)
			) { // Remove the particle instance and related information.
			particleIntegrator.ParticleState[physicsId].Unused						= true;
			ree_if(errored(particleInstances.remove(iParticle)), "Not sure why would this fail.");
			--iParticle;
		}
		else {
			::SParticleToDraw															particleToDraw								= {physicsId, (int32_t)iParticle, particleInstance.Type.TimeLived, particleCurrent.Position.Cast<int32_t>()};
				 if(particleInstance.Type.Type == PARTICLE_TYPE_STAR		)	applicationInstance.StuffToDraw.Stars	.push_back(particleToDraw);
			else if(particleInstance.Type.Type == PARTICLE_TYPE_SHIP_THRUST	)	applicationInstance.StuffToDraw.Thrust	.push_back(particleToDraw);
			else if(particleInstance.Type.Type == PARTICLE_TYPE_DEBRIS		)	applicationInstance.StuffToDraw.Debris	.push_back(particleToDraw);
			particleInstance.Type.TimeLived											+= lastFrameSeconds;
			particleCurrent															= particleNext;
		}
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
			gameInstance.Projectiles.Projectiles[iProjectile].TypeWeapon			= weaponType;
			gameInstance.Projectiles.Projectiles[iProjectile].TypePlayer			= playerType;
			gameInstance.Projectiles.Projectiles[iProjectile].ShipIndex				= iShip;
			gameInstance.Projectiles.Projectiles[iProjectile].TimeLived				= 0;
			gameInstance.Projectiles.Projectiles[iProjectile].Speed					= projectileSpeed;
			return iProjectile;
		}
	}
	return -1;	// Projectile storage is full. Cannot add projectile.
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
				gameParticle.Lit														= 0 == (rand() % 2);
				::addParticle(gameParticle, particleInstances, particleIntegrator, gameInstance.Ships.Position[iShip] + applicationInstance.PSOffsetFromShipCenter.Cast<float>(), gameInstance.Ships.Direction[iShip] * -1.0, (float)(rand() % 400) + (gameInstance.Ships.States[iShip].Thrust ? 400 : 0), particleDefinitions);
			}
		}
		gameInstance.Ships.WeaponDelay[iShip]									+= framework.FrameInfo.Seconds.LastFrame;
		if(gameInstance.Ships.States[iShip].Firing) { // Add lasers / bullets.
			const ::SWeapon																weapon										= gameInstance.Ships.Weapon[iShip];
			if( gameInstance.Ships.WeaponDelay[iShip] >= weapon.Delay ) {
				gameInstance.Ships.WeaponDelay[iShip]									= 0;
				::SGameParticle																gameParticle;
				gameParticle.OwnerIndex													= iShip;
				gameParticle.TimeLived													= 0;
				gameParticle.Type														= PARTICLE_TYPE_PROJECTILE;
				gameParticle.Lit														= true;
				gameParticle.TypePlayer													= PLAYER_TYPE_PLAYER;
				gameParticle.TypeWeapon													= gameInstance.Ships.Weapon[iShip].Type;
				const ::cho::SCoord2<float>													textureShipMetrics							= applicationInstance.TextureShip.Processed.View.metrics().Cast<float>();
				const ::cho::SCoord2<float>													weaponParticleOffset						= {textureShipMetrics.x - (textureShipMetrics.x - applicationInstance.TextureCenters[GAME_TEXTURE_SHIP0 + iShip].x), -1};
				::addParticle(gameParticle, particleInstances, particleIntegrator, gameInstance.Ships.Position[iShip] + weaponParticleOffset, {1, 0}, weapon.Speed, particleDefinitions);
				e_if(errored(::addProjectile(gameInstance, iShip, gameParticle.TypePlayer, gameParticle.TypeWeapon, weapon.Speed)), "Projectile storage is full. Cannot add projectile.");
			}
		}
	}
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
		gameInstance.Projectiles.Projectiles[iProjectile].TimeLived				+= applicationInstance.Framework.FrameInfo.Seconds.LastFrame;
		if(gameInstance.Projectiles.Projectiles[iProjectile].TimeLived > 0.01) 
			projectilesAlive[iProjectile]											= 0;
	}
	applicationInstance.StuffToDraw.CollisionPoints.clear();
	::SAABBCache																aabbCache;
	for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
		const ::SLaserToDraw														& laserToDraw								= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
		const ::cho::SLine2D<float>													& projectilePath							= laserToDraw.Segment;
		for(uint32_t iPow = 0; iPow < gameInstance.Powerups.Alive.size(); ++iPow) { // Check powerup
			if(0 == gameInstance.Powerups.Alive[iPow])
				continue;
			const cho::SCoord2<float>													& posPowerup								= gameInstance.Powerups.Position[iPow];
			float																		halfSizeBox									= (float)applicationInstance.TextureCenters[GAME_TEXTURE_POWERUP0].x;
			::checkLaserCollision(projectilePath, aabbCache, posPowerup, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints);
		}
		for(uint32_t iEnemy = 0; iEnemy < gameInstance.Enemies.Alive.size(); ++iEnemy) { // Check enemy
			if(0 == gameInstance.Enemies.Alive[iEnemy])
				continue;
			const cho::SCoord2<float>													& posEnemy									= gameInstance.Enemies.Position[iEnemy];
			float																		halfSizeBox									= (float)applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY].x;
			::checkLaserCollision(projectilePath, aabbCache, posEnemy, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints);
			static constexpr const ::cho::SCoord2<float>								reference									= {1, 0};
			::cho::SCoord2<float>														vector;
			for(uint32_t iGhost = 0; iGhost < 5; ++iGhost) {
				vector																	= reference * (64 * sin(applicationInstance.Framework.FrameInfo.Seconds.Total));
				vector.Rotate(::cho::math_2pi / 5 * iGhost + gameInstance.GhostTimer);
				::checkLaserCollision(projectilePath, aabbCache, posEnemy + vector, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints);
			}
		}
	}
	::cho::array_pod<::SApplication::TParticleInstance>							& particleInstances							= applicationInstance.ParticleSystem.Instances;
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
		const cho::SCoord2<float>												& posXHair									= gameInstance.PositionCrosshair[iShip];
		for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
			float																	halfSizeBox									= gameInstance.HalfWidthCrosshair;
			const ::cho::SLine2D<float>												verticalSegments[]							= 
				{ {posXHair + ::cho::SCoord2<float>{ halfSizeBox - 1, halfSizeBox - 1}, posXHair + ::cho::SCoord2<float>{ halfSizeBox - 1	,-halfSizeBox}}
				, {posXHair + ::cho::SCoord2<float>{-halfSizeBox	, halfSizeBox - 1}, posXHair + ::cho::SCoord2<float>{-halfSizeBox		,-halfSizeBox}}
				};
			const ::SLaserToDraw													& laserToDraw								= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
			const ::cho::SLine2D<float>												& projectilePath							= laserToDraw.Segment;
			::cho::SCoord2<float>													collisions	[::cho::size(verticalSegments)]= {};
			for(uint32_t iSeg = 0; iSeg < ::cho::size(verticalSegments); ++iSeg) {
				::cho::SCoord2<float>													& collision									= collisions		[iSeg];
				const ::cho::SLine2D<float>												& segSelected								= verticalSegments	[iSeg]; 
				if(::cho::line_line_intersect(projectilePath, segSelected, collision)) {
					bool																	bFound										= false;
					for(uint32_t iS2 = 0; iS2 < iSeg; ++iS2) {
						if(collision == collisions[iS2]) {
							bFound																= true;
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
	::SGame																		& gameInstance								= applicationInstance.Game;
	gameInstance.GhostTimer													+= framework.FrameInfo.Seconds.LastFrame;
	static float																timerSpawn									= 0;
	timerSpawn																+= (float)framework.FrameInfo.Seconds.LastFrame;
	if(timerSpawn > 5) {
		timerSpawn																= 0;
		int32_t																		indexToSpawn								= firstUnused(gameInstance.Enemies.Alive);
		if(indexToSpawn != -1) {
			gameInstance.Enemies.Alive[indexToSpawn]									= 1;
		}
		else
			warning_printf("Not enough space in enemy container to spawn more enemies!");	
		int32_t																		indexToSpawnPow								= firstUnused(gameInstance.Powerups.Alive);
		if(indexToSpawn != -1 && gameInstance.GhostTimer > 7) {
			gameInstance.Powerups.Alive		[indexToSpawnPow]						= 1;
			gameInstance.Powerups.Position	[indexToSpawnPow]						= gameInstance.Enemies.Position[0];
			gameInstance.Powerups.Family	[indexToSpawnPow]						= (POWERUP_FAMILY)(rand() % POWERUP_FAMILY_COUNT);
		}
		else
			warning_printf("Not enough space in enemy container to spawn more enemies!");	
	}
	for(uint32_t iEnemy = 0; iEnemy < gameInstance.Enemies.Alive.size(); ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		gameInstance.Enemies.SkillTimer[iEnemy]									+= framework.FrameInfo.Seconds.LastFrame;
		{
			static float																timerPath									= 0;
			timerPath																+= (float)framework.Timer.LastTimeSeconds;
			if(timerPath > 10.0f) {
				timerPath																= 0;
				++gameInstance.Enemies.PathStep[iEnemy];
				if( gameInstance.Enemies.PathStep[iEnemy] >= ::cho::size(gameInstance.PathEnemy) )
					gameInstance.Enemies.PathStep[iEnemy]										= 0;
			}
			const ::cho::SCoord2<float>													& pathTarget								= gameInstance.PathEnemy[gameInstance.Enemies.PathStep[iEnemy]];
			::cho::SCoord2<float>														directionEnemy								= (pathTarget - gameInstance.Enemies.Position[iEnemy]);
			if(directionEnemy.LengthSquared() < 0.5) {
				timerPath																= 0;
				++gameInstance.Enemies.PathStep[iEnemy];
				if( gameInstance.Enemies.PathStep[iEnemy] >= ::cho::size(gameInstance.PathEnemy) )
					gameInstance.Enemies.PathStep[iEnemy]										= 0;
			}
			else {
				directionEnemy.Normalize();
				// update enemy
				::cho::SFramework::TOffscreen												& offscreen									= framework.Offscreen;
				gameInstance.Enemies.Position[iEnemy]										+= directionEnemy * (float)(framework.FrameInfo.Seconds.LastFrame * 100);// * (applicationInstance.ShipState.Brakes ? .25f : (applicationInstance.ShipState.Thrust ? 2 : 1));
				gameInstance.Enemies.Position[iEnemy]										= 
					{ ::cho::clamp(gameInstance.Enemies.Position[iEnemy].x, .1f, (float)offscreen.View.metrics().x - 1)
					, ::cho::clamp(gameInstance.Enemies.Position[iEnemy].y, .1f, (float)offscreen.View.metrics().y - 1)
					};
			}
		}
	}	return 0;
}