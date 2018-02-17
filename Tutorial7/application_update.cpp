#include "application.h"
#include "cho_collision.h"

					::cho::error_t										updateInput									(::SApplication& applicationInstance)											{ 
	::cho::SInput																& inputSystem								= applicationInstance.Framework.SystemInput;
	::SGame																		& gameInstance								= applicationInstance.Game;
	gameInstance.ShipStates[0].Firing										= inputSystem.KeyboardCurrent.KeyState['T'] != 0;
	gameInstance.ShipStates[0].Thrust										= inputSystem.KeyboardCurrent.KeyState['Y'] != 0;
	gameInstance.ShipStates[0].Brakes										= inputSystem.KeyboardCurrent.KeyState['U'] != 0;

	gameInstance.ShipStates[1].Firing										= inputSystem.KeyboardCurrent.KeyState[VK_NUMPAD1] != 0;
	gameInstance.ShipStates[1].Thrust										= inputSystem.KeyboardCurrent.KeyState[VK_NUMPAD2] != 0;
	gameInstance.ShipStates[1].Brakes										= inputSystem.KeyboardCurrent.KeyState[VK_NUMPAD3] != 0;

	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip)
		gameInstance.ShipDirection[iShip]										= {};

	if(inputSystem.KeyboardCurrent.KeyState['W'			]) gameInstance.ShipDirection[0].y += 1;
	if(inputSystem.KeyboardCurrent.KeyState['S'			]) gameInstance.ShipDirection[0].y -= 1;
	if(inputSystem.KeyboardCurrent.KeyState['D'			]) gameInstance.ShipDirection[0].x += 1;
	if(inputSystem.KeyboardCurrent.KeyState['A'			]) gameInstance.ShipDirection[0].x -= 1;

	if(inputSystem.KeyboardCurrent.KeyState[VK_UP		]) gameInstance.ShipDirection[1].y += 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_DOWN		]) gameInstance.ShipDirection[1].y -= 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_RIGHT	]) gameInstance.ShipDirection[1].x += 1;
	if(inputSystem.KeyboardCurrent.KeyState[VK_LEFT		]) gameInstance.ShipDirection[1].x -= 1;

	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip)
		gameInstance.ShipDirection[iShip].Normalize();
	return 0;
}

					::cho::error_t										updateParticles								(::SApplication& applicationInstance)											{ 
	typedef	::SApplication::TParticleInstance									TParticleInstance;
	typedef	::SApplication::TParticle											TParticle;
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
		TParticle																	& particleNext								= particleIntegrator.ParticleNext[physicsId];
		TParticle																	& particleCurrent							= particleIntegrator.Particle[physicsId];
		if(particleInstance.Type == PARTICLE_TYPE_LASER) {
			::SLaserToDraw																laserToDraw									= {physicsId, (int32_t)iParticle, ::cho::SLine2D<float>{particleCurrent.Position, particleNext.Position}, ::cho::LIGHTCYAN};
			applicationInstance.StuffToDraw.ProjectilePaths.push_back(laserToDraw);
		}
		if( ((uint32_t)particleNext.Position.x) >= framework.Offscreen.View.width	()
		 || ((uint32_t)particleNext.Position.y) >= framework.Offscreen.View.height	()
		 || (particleInstance.TimeLived >= .125 && particleInstance.Type == PARTICLE_TYPE_SHIP_THRUST)
		 ) { // Remove the particle instance and related information.
			particleIntegrator.ParticleState[physicsId].Unused						= true;
			ree_if(errored(particleInstances.remove(iParticle)), "Not sure why would this fail.");
			--iParticle;
		}
		else {
			::SParticleToDraw															particleToDraw								= {physicsId, (int32_t)iParticle, particleInstance.TimeLived, particleCurrent.Position.Cast<int32_t>(), particleInstance.Lit};
				 if(particleInstance.Type == PARTICLE_TYPE_STAR			)	applicationInstance.StuffToDraw.Stars	.push_back(particleToDraw);
			else if(particleInstance.Type == PARTICLE_TYPE_SHIP_THRUST	)	applicationInstance.StuffToDraw.Thrust	.push_back(particleToDraw);
			else if(particleInstance.Type == PARTICLE_TYPE_DEBRIS		)	applicationInstance.StuffToDraw.Debris	.push_back(particleToDraw);
			particleInstance.TimeLived												+= lastFrameSeconds;
			particleCurrent															= particleNext;
		}
	}
	return 0;
}

					::cho::error_t										updateShips									(::SApplication & applicationInstance)			{
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::SFramework::TOffscreen												& offscreen									= framework.Offscreen;
	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {
		::cho::SCoord2<float>														& shipPosition								= gameInstance.ShipPosition[iShip];
		// update ship
		shipPosition															+= gameInstance.ShipDirection[iShip] * (float)(framework.FrameInfo.Seconds.LastFrame * 100) * 
			(gameInstance.ShipStates[iShip].Brakes ? .25f : (gameInstance.ShipStates[iShip].Thrust ? 2 : 1));
		shipPosition.x															= ::cho::clamp(shipPosition.x, .1f, (float)offscreen.View.metrics().x - 1);
		shipPosition.y															= ::cho::clamp(shipPosition.y, .1f, (float)offscreen.View.metrics().y - 1);
		{ // update crosshair 
			gameInstance.CrosshairPosition[iShip]									= shipPosition + ::cho::SCoord2<float>{96,};
			gameInstance.CrosshairPosition[iShip].x									= ::cho::min(gameInstance.CrosshairPosition[iShip].x, (float)offscreen.View.metrics().x);
		}
	}
	return 0;
}

template<typename _tParticleType>
static				::cho::error_t										addParticle														
	(	::PARTICLE_TYPE												particleType
	,	::cho::array_pod<::cho::SParticleInstance<_tParticleType>>	& particleInstances
	,	::SApplication::TIntegrator									& particleIntegrator
	,	const ::cho::SCoord2<float>									& particlePosition
	,	const ::cho::SCoord2<float>									& particleDirection
	,	float														speed
	,	const ::cho::array_view<::SApplication::TParticle>			& particleDefinitions
	)														
{
	::cho::SParticleInstance<_tParticleType>									& newInstance								= particleInstances[::cho::addParticle(particleType, particleInstances, particleIntegrator, particleDefinitions[particleType])]; 
	::SApplication::TParticle													& newParticle								= particleIntegrator.Particle[newInstance.ParticleIndex];
	newParticle.Position													= particlePosition; 
	::cho::SCoord2<float>														newDirection								= particleDirection;
	const float																	value										= .5;
	switch(particleType) {
	default							: break;
	case ::PARTICLE_TYPE_DEBRIS		:	
	case ::PARTICLE_TYPE_STAR		: newInstance.Lit							= 0 == (rand() % 3); break;
	case ::PARTICLE_TYPE_SHIP_THRUST: newInstance.Lit							= 0 == (rand() % 2);
		newParticle.Position.y													+= rand() % 3 - 1;
		newDirection.Rotate(((rand() % 32767) / 32766.0f) * value - value / 2);
		break;
	}
	newParticle.Forces.Velocity												= newDirection * speed;	//{ -, (float)((rand() % 31 * 4) - 15 * 4)};
	return 0;
}

static				::cho::error_t										addProjectile								(::SGame & gameInstance, int32_t iShip, PLAYER_TYPE playerType, WEAPON_TYPE weaponType)					{
	::cho::bit_array_view<uint32_t>												& projectilesAlive							= gameInstance.ProjectilesAliveView;
	const float																	projectileSpeed								= gameInstance.Laser.Speed;
	for(uint32_t iProjectile = 0, projectileCount = projectilesAlive.size(); iProjectile < projectileCount; ++iProjectile)
		if(0 == projectilesAlive[iProjectile]) {
			projectilesAlive[iProjectile]											= 1;
			gameInstance.Projectiles[iProjectile].TypeWeapon						= weaponType;
			gameInstance.Projectiles[iProjectile].TypePlayer						= playerType;
			gameInstance.Projectiles[iProjectile].ShipIndex							= iShip;
			gameInstance.Projectiles[iProjectile].TimeLived							= 0;
			gameInstance.Projectiles[iProjectile].Speed								= projectileSpeed;
			return iProjectile;
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
	applicationInstance.EffectsDelay.Thrust																+= framework.FrameInfo.Seconds.LastFrame;
	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {
		if(applicationInstance.EffectsDelay.Thrust > .01) {
			for(int32_t i = 0, particleCountToSpawn = 1 + rand() % 4; i < particleCountToSpawn; ++i) 
				::addParticle(PARTICLE_TYPE_SHIP_THRUST, particleInstances, particleIntegrator, gameInstance.ShipPosition[iShip] + applicationInstance.PSOffsetFromShipCenter.Cast<float>(), gameInstance.ShipDirection[iShip] * -1.0, (float)(rand() % 400) + (gameInstance.ShipStates[iShip].Thrust ? 400 : 0), particleDefinitions);
		}
		gameInstance.ShipWeaponDelay[iShip]										+= framework.FrameInfo.Seconds.LastFrame;
		if(gameInstance.ShipStates[iShip].Firing) {
			if(gameInstance.ShipWeaponDelay[iShip] >= gameInstance.Laser.Delay) {
				gameInstance.ShipWeaponDelay[iShip]										= 0;
				const ::cho::SCoord2<float>													textureShipMetrics					= applicationInstance.TextureShip.Processed.View.metrics().Cast<float>();
				const ::cho::SCoord2<float>													weaponParticleOffset				= {textureShipMetrics.x - (textureShipMetrics.x - applicationInstance.TextureCenters[GAME_TEXTURE_SHIP0 + iShip].x), -1};
				::addParticle(PARTICLE_TYPE_LASER, particleInstances, particleIntegrator, gameInstance.ShipPosition[iShip] + weaponParticleOffset, {1, 0}, gameInstance.Laser.Speed, particleDefinitions);
				e_if(errored(::addProjectile(gameInstance, iShip, PLAYER_TYPE_PLAYER, WEAPON_TYPE_LASER)), "Projectile storage is full. Cannot add projectile.");
			}
		}
	}

	applicationInstance.EffectsDelay.Star									+= framework.FrameInfo.Seconds.LastFrame;
	if(applicationInstance.EffectsDelay.Star > .1) {
		applicationInstance.EffectsDelay.Star									= 0;
		bool																		bFastStarFromThrust							= false;
		for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) 
			if (gameInstance.ShipStates[iShip].Thrust) {
				bFastStarFromThrust														= true;
				break;
			}
		::addParticle(PARTICLE_TYPE_STAR, particleInstances, particleIntegrator, {offscreen.View.metrics().x - 1.0f, (float)(rand() % offscreen.View.metrics().y)}, {-1, 0}, (float)(rand() % (bFastStarFromThrust ? 400 : 75)) + 25, particleDefinitions);
	}
	return 0;
}

struct SAABBCache {	
						::cho::SLine2D<float>								RectangleSegments	[4]						= {};
						::cho::SCoord2<float>								CollisionPoints		[4]						= {};
						bool												Collision			[4]						= {};
};

template<typename _tCoord>
static				::cho::error_t										updateLaserCollision
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

					::cho::error_t										updateShots
	( ::SApplication										& applicationInstance
	, const ::cho::array_view<::SApplication::TParticle>	& particleDefinitions
	)
{ 
	::SGame																		& gameInstance								= applicationInstance.Game;
	::cho::bit_array_view<uint32_t>												& projectilesAlive							= gameInstance.ProjectilesAliveView;
	for(uint32_t iProjectile = 0, projectileCount = projectilesAlive.size(); iProjectile < projectileCount; ++iProjectile)
		if(0 != projectilesAlive[iProjectile] && gameInstance.Projectiles[iProjectile].TimeLived > 0.01) {
			projectilesAlive[iProjectile]											= 0;
		}
	applicationInstance.StuffToDraw.CollisionPoints.clear();
	::SAABBCache																aabbCache;
	for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
		const ::SLaserToDraw														& laserToDraw								= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
		const ::cho::SLine2D<float>													& projectilePath							= laserToDraw.Segment;
		{ // Check powerup
			const cho::SCoord2<float>													& posPowerup								= gameInstance.PositionPowerup;
			float																		halfSizeBox									= (float)applicationInstance.TextureCenters[GAME_TEXTURE_POWERUP0].x;
			::updateLaserCollision(projectilePath, aabbCache, posPowerup, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints);
		}
		for(uint32_t iEnemy = 0; iEnemy < gameInstance.CountEnemies; ++iEnemy) { // Check enemy
			const cho::SCoord2<float>													& posEnemy									= gameInstance.EnemyPosition[iEnemy];
			float																		halfSizeBox									= (float)applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY].x;
			::updateLaserCollision(projectilePath, aabbCache, posEnemy, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints);
			static constexpr const ::cho::SCoord2<float>								reference									= {1, 0};
			::cho::SCoord2<float>														vector;
			for(uint32_t iGhost = 0; iGhost < 5; ++iGhost) {
				vector																	= reference * (64 * sin(applicationInstance.Framework.FrameInfo.Seconds.Total));
				vector.Rotate(::cho::math_2pi / 5 * iGhost + gameInstance.GhostTimer);
				::updateLaserCollision(projectilePath, aabbCache, posEnemy + vector, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints);
			}
		}
	}

	typedef	::SApplication::TParticleInstance									TParticleInstance;
	::cho::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystem.Instances;
	::SApplication::TIntegrator													& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	for(uint32_t iCollision = 0, collisionCount = applicationInstance.StuffToDraw.CollisionPoints.size(); iCollision < collisionCount; ++iCollision)
		for(uint32_t i=0; i < 10; ++i) {
			::cho::SCoord2<float>	angle	= {(float)-(rand() % 20) - 10, (float)(rand() % 20 - 1 - 10)};
			angle.Normalize();
			::addParticle(PARTICLE_TYPE_DEBRIS, particleInstances, particleIntegrator, applicationInstance.StuffToDraw.CollisionPoints[iCollision], angle, (float)(rand() % 400) + 100, particleDefinitions);
		}
	return 0;
} 
					::cho::error_t										updateGUI									(::SApplication& applicationInstance)					{ 
	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {
		{ // Calculate line of sight 
			::SAABBCache																aabbCache;
			::cho::SLine2D<float>														projectilePath								= {gameInstance.ShipPosition[iShip], gameInstance.ShipPosition[iShip] + ::cho::SCoord2<float>{10000, }};
			projectilePath.A.y														-= 1;
			projectilePath.B.y														-= 1;
			gameInstance.ShipLineOfFire[iShip]										= false;
			for(uint32_t iEnemy = 0; iEnemy < ::cho::size(gameInstance.EnemyPosition); ++iEnemy) {
				const cho::SCoord2<float>													& posXHair									= gameInstance.EnemyPosition[iEnemy];
				float																		halfSizeBox									= gameInstance.HalfWidthEnemy; //(float)applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY].x;
				if(1 == ::updateLaserCollision(projectilePath, aabbCache, posXHair, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints))
					gameInstance.ShipLineOfFire[iShip]										= true;
			}
			{
				const cho::SCoord2<float>													& posXHair									= gameInstance.PositionPowerup;
				float																		halfSizeBox									= gameInstance.HalfWidthPowerup;//(float)applicationInstance.TextureCenters[GAME_TEXTURE_POWERUP0].x;
				if(1 == ::updateLaserCollision(projectilePath, aabbCache, posXHair, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints))
					gameInstance.ShipLineOfFire[iShip]										= true;
			}
		}
		// 
		const cho::SCoord2<float>												& posXHair									= gameInstance.CrosshairPosition[iShip];
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
