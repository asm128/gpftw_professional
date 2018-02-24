// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"
#include "cho_grid_copy.h"
#include "cho_bitmap_target.h"

static				::cho::error_t										drawShipHealthBar							(::SApplication& applicationInstance, const ::cho::SCoord2<float> & centerEnemy, const ::cho::SCoord2<int32_t> & halfMetrics, uint32_t health, int32_t yOffset, const ::cho::SColorBGRA & finalColor)											{
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= framework.Offscreen.View;
	::cho::SLine2D<int32_t>														healthBar									= {};
	healthBar.A 															= {(int32_t)(centerEnemy.x  + .5f - halfMetrics.x), (int32_t)(centerEnemy.y + yOffset)};
	healthBar.B																= {(int32_t)(centerEnemy.x  + .5f + halfMetrics.x), (int32_t)(centerEnemy.y + yOffset)};
	double																		enemyHealthProportion						= health / 5000.0;
	healthBar.B.x															= ::cho::interpolate_linear(healthBar.A.x, healthBar.B.x, enemyHealthProportion);
	applicationInstance.CacheLinePoints.clear();
	::cho::drawLine(viewOffscreen.metrics(), healthBar, applicationInstance.CacheLinePoints);
	for(uint32_t iLinePoint = 0, pointCount = applicationInstance.CacheLinePoints.size(); iLinePoint < pointCount; ++iLinePoint) {
		const ::cho::SCoord2<float>													& pointToDraw								= applicationInstance.CacheLinePoints[iLinePoint].Cast<float>();
		::cho::drawPixelLight(viewOffscreen, pointToDraw, finalColor, .2f, 1.5f);
	}
	return 0;
}
//
					::cho::error_t										drawShips									(::SApplication& applicationInstance)											{
	::SGame																		& gameInstance								= applicationInstance.Game;
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= framework.Offscreen.View;
	// ---- Draw enemy ships
	const ::cho::grid_view<::cho::SColorBGRA>									& enemyView									= applicationInstance.Textures[GAME_TEXTURE_ENEMY].Processed.View;
	char																		indexPositionsX[]							= {0, 1, 2, 3, 2, 1, 0, -1, -2, -3, -2, -1};
	::cho::SCoord2<int32_t>														halfMetricsEnemy							= (enemyView.metrics() / 2).Cast<int32_t>();
	int32_t																		halfMetricsEnemy2y							= halfMetricsEnemy.y;
	// ---- Draw ghosts.
	for(uint32_t iEnemy = 0, enemyCount = gameInstance.Enemies.Alive.size(); iEnemy < enemyCount; ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		static constexpr const ::cho::SCoord2<float>								reference	= {1, 0};
		::cho::SCoord2<float>														vector;
		static double																beaconTimer								= 0;
		beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 8;
		int32_t																		selectedPos								= ((int32_t)beaconTimer % ::cho::size(indexPositionsX));
		for(uint32_t iGhost = 0; iGhost < 5; ++iGhost) {
			vector																	= reference * (64 * sin(framework.FrameInfo.Seconds.Total));
			vector.Rotate(::cho::math_2pi / 5 * iGhost + gameInstance.Enemies.TimeLived[iEnemy]);
			error_if(errored(::cho::grid_copy_alpha(viewOffscreen, enemyView, (gameInstance.Enemies.Position[iEnemy] + vector).Cast<int32_t>() - applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
			{ // Draw ghost light
				::cho::SCoord2<float>														centerPowerup							= gameInstance.Enemies.Position[iEnemy] + vector;
				::cho::SCoord2<float>														lightCrosshair							= centerPowerup + ::cho::SCoord2<float>{(float)indexPositionsX[selectedPos], 0.0f};
				::cho::drawPixelLight(viewOffscreen, lightCrosshair.Cast<float>(), ::cho::SColorBGRA(::cho::YELLOW), .2f, 3.0f);
			}
		}
	}
	// --- Draw enemy ships.
	for(uint32_t iEnemy = 0, enemyCount = gameInstance.Enemies.Alive.size(); iEnemy < enemyCount; ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		const ::cho::SCoord2<float>													& centerEnemy								= gameInstance.Enemies.Position[iEnemy];
		const ::SHealthPoints														& enemyHealth								= gameInstance.Enemies.Health[iEnemy];
		error_if(errored(::cho::grid_copy_alpha(viewOffscreen, enemyView, centerEnemy.Cast<int32_t>() - applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		::drawShipHealthBar(applicationInstance, centerEnemy, halfMetricsEnemy, enemyHealth.Health, (int32_t)(halfMetricsEnemy2y	), ::cho::GREEN);
		::drawShipHealthBar(applicationInstance, centerEnemy, halfMetricsEnemy, enemyHealth.Shield, (int32_t)(halfMetricsEnemy2y - 2), ::cho::CYAN );
	}
	// --- Draw player ships.
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		const ::cho::grid_view<::cho::SColorBGRA>									& shipView									= applicationInstance.Textures[GAME_TEXTURE_SHIP0 + iShip].Processed.View;
		const ::cho::SCoord2<float>													& centerShip								= gameInstance.Ships.Position	[iShip];
		const ::SHealthPoints														& enemyHealth								= gameInstance.Ships.Health	[iShip];
		::cho::SCoord2<int32_t>														halfMetricsShip								= (shipView.metrics() / 2).Cast<int32_t>();
		int32_t																		halfMetricsShip2y							= halfMetricsShip.y;
		error_if(errored(::cho::grid_copy_alpha(viewOffscreen, shipView, gameInstance.Ships.Position[iShip].Cast<int32_t>() - applicationInstance.TextureCenters[GAME_TEXTURE_SHIP0 + iShip], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		::drawShipHealthBar(applicationInstance, centerShip, halfMetricsShip, enemyHealth.Health, (int32_t)(halfMetricsShip2y		), ::cho::GREEN);
		::drawShipHealthBar(applicationInstance, centerShip, halfMetricsShip, enemyHealth.Shield, (int32_t)(halfMetricsShip2y - 2	), ::cho::CYAN );
	}
	// --- Draw enemy ship lights.
	for(uint32_t iEnemy = 0, enemyCount = gameInstance.Enemies.Alive.size(); iEnemy < enemyCount; ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		const ::cho::SCoord2<float>													& centerEnemy								= gameInstance.Enemies.Position[iEnemy];
		static double																beaconTimer									= 0;
		beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 8;
		int32_t																		selectedPos									= ((int32_t)beaconTimer % ::cho::size(indexPositionsX));
		::cho::SCoord2<float>														lightCrosshair								= centerEnemy + ::cho::SCoord2<float>{(float)indexPositionsX[selectedPos], 0.0f};
		::cho::drawPixelLight(viewOffscreen, lightCrosshair.Cast<float>(), ::cho::SColorBGRA(::cho::RED), .2f, 3.0f);
	}
	return 0;
}

					::cho::error_t										drawCollisions								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= framework.Offscreen.View;
	for(uint32_t iRay = 0, rayCount = applicationInstance.StuffToDraw.CollisionPoints.size(); iRay < rayCount; ++iRay) {
		const ::cho::SCoord2<float>													& pointToDraw									= applicationInstance.StuffToDraw.CollisionPoints[iRay];
		::cho::drawPixelLight(viewOffscreen, pointToDraw, ::cho::SColorBGRA(::cho::ORANGE), .15f, 3.0f);
	}
	for(uint32_t iRay = 0, rayCount = applicationInstance.StuffToDraw.Debris.size(); iRay < rayCount; ++iRay) {
		const ::SParticleToDraw														& particleToDraw								= applicationInstance.StuffToDraw.Debris[iRay];
		const ::cho::SCoord2<float>													& pointToDraw									= particleToDraw.Position.Cast<float>();
		if(applicationInstance.ParticleSystemDebris.Instances[particleToDraw.IndexParticleInstance].Binding.Lit) {
			::cho::SColorBGRA															finalColor										
				= (0 == (particleToDraw.IndexParticlePhysics % 3)) ? ::cho::SColorBGRA(::cho::YELLOW) 
				: (0 == (particleToDraw.IndexParticlePhysics % 2)) ? ::cho::SColorBGRA(::cho::RED) 
				: ::cho::SColorBGRA(::cho::ORANGE)
				;
			finalColor																*= 1.0f - (particleToDraw.TimeLived);
			::cho::drawPixelLight(viewOffscreen, pointToDraw, finalColor, .15f, ::cho::max(0.0f, 1.0f - (particleToDraw.TimeLived)) * 4.0f);
		}
	}
	return 0;
}

static				const ::cho::array_static<::cho::SColorBGRA, WEAPON_TYPE_COUNT>	weaponTypeColorPalette			= 
	{ ::cho::SColorBGRA{::cho::RED		  	}
	, ::cho::SColorBGRA{::cho::LIGHTCYAN  	}
	, ::cho::SColorBGRA{::cho::LIGHTGREEN 	}
	, ::cho::SColorBGRA{::cho::LIGHTGRAY	}
	, ::cho::SColorBGRA{::cho::LIGHTYELLOW	}
	, ::cho::SColorBGRA{::cho::DARKGRAY		}
	, ::cho::SColorBGRA{::cho::WHITE		}
	};

					::cho::error_t										drawShots									(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= applicationInstance.Framework.Offscreen.View;
	for(uint32_t iRay = 0, rayCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iRay < rayCount; ++iRay) {
		const ::SLaserToDraw														& laserToDraw								= applicationInstance.StuffToDraw.ProjectilePaths[iRay];
		applicationInstance.CacheLinePoints.clear();
		::cho::drawLine(viewOffscreen.metrics(), laserToDraw.Segment, applicationInstance.CacheLinePoints);
		const ::cho::SParticleBinding<::SGameParticle>								& gameParticle								= applicationInstance.ParticleSystemProjectiles.Instances[laserToDraw.IndexParticleInstance];
		const float																	lightRange									= (gameParticle.Binding.TypePlayer == PLAYER_TYPE_PLAYER) ? 3.0f : 2.5f;
		const float																	lightValue									= (gameParticle.Binding.TypePlayer == PLAYER_TYPE_PLAYER) ? .15f : 1.0f;
		const ::cho::SColorBGRA														finalColor									= weaponTypeColorPalette[gameParticle.Binding.TypeWeapon];
		for(uint32_t iLinePoint = 0, pointCount = applicationInstance.CacheLinePoints.size(); iLinePoint < pointCount; ++iLinePoint) {
			const ::cho::SCoord2<float>													& pointToDraw								= applicationInstance.CacheLinePoints[iLinePoint].Cast<float>();
			::cho::drawPixelLight(viewOffscreen, pointToDraw, finalColor, lightValue, lightRange);
		}
	}
	return 0;
}
					::cho::error_t										drawBackground								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::STexture<::cho::SColorBGRA>											& offscreen									= framework.Offscreen;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= offscreen.View;
	::cho::drawRectangle(offscreen.View, applicationInstance.ColorBackground, ::cho::SRectangle2D<uint32_t>{{}, offscreen.View.metrics()});
	for(uint32_t iRay = 0, rayCount = applicationInstance.StuffToDraw.Stars.size(); iRay < rayCount; ++iRay) {
		::SParticleToDraw															& starToDraw								= applicationInstance.StuffToDraw.Stars[iRay];
		if(false == applicationInstance.ParticleSystemStars.Instances[starToDraw.IndexParticleInstance].Binding.Lit)
			continue;
		::cho::SCoord2<int32_t>														& particlePosition							= starToDraw.Position;
		viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	
			= (0 == (starToDraw.IndexParticlePhysics % 7))	? ::cho::DARKYELLOW	/ 2.0f
			: (0 == (starToDraw.IndexParticlePhysics % 6))	? ::cho::GRAY 
			: (0 == (starToDraw.IndexParticlePhysics % 5))	? ::cho::WHITE
			: (0 == (starToDraw.IndexParticlePhysics % 4))	? ::cho::DARKGRAY 
			: (0 == (starToDraw.IndexParticlePhysics % 3))	? ::cho::GRAY 
			: (0 == (starToDraw.IndexParticlePhysics % 2))	? ::cho::WHITE
			: ::cho::DARKGRAY 
			;
		float																		maxFactor	= .5f;
		float																		range		= 3.f;
		maxFactor																= (rand() % 3 + 1) * 0.10f;
		range																	= starToDraw.IndexParticlePhysics % 3 + 1.0f;
		::cho::drawPixelLight(viewOffscreen, particlePosition.Cast<float>(), viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x], maxFactor, range);
	}
	return 0;
}
					::cho::error_t										drawThrust								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::STexture<::cho::SColorBGRA>											& offscreen									= framework.Offscreen;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= offscreen.View;
	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iThrust = 0, particleCount = (uint32_t)applicationInstance.StuffToDraw.Thrust.size(); iThrust < particleCount; ++iThrust) {
		::SParticleToDraw															& thrustToDraw								= applicationInstance.StuffToDraw.Thrust[iThrust];
		::SApplication::TParticleInstance											& particleInstance							= applicationInstance.ParticleSystemThrust.Instances[thrustToDraw.IndexParticleInstance];
		if(false == particleInstance.Binding.Lit)
			continue;
		::SShipState																& shipState									= (particleInstance.Binding.TypePlayer == PLAYER_TYPE_PLAYER) 
			? gameInstance.Ships	.States[particleInstance.Binding.OwnerIndex] 
			: gameInstance.Enemies	.States[particleInstance.Binding.OwnerIndex]
			;
		const int32_t																physicsId									= thrustToDraw.IndexParticlePhysics;
		const ::cho::SCoord2<float>													& particlePosition							= applicationInstance.ParticleSystemThrust.Integrator.Particle[physicsId].Position;
		if(false == ::cho::in_range(particlePosition, {{}, offscreen.View.metrics().Cast<float>()}))
			continue;
		viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	
			= (thrustToDraw.TimeLived > .075)		? (shipState.Thrust ? ::cho::DARKGRAY	: ::cho::DARKGRAY	)
			: (thrustToDraw.TimeLived > .03 )		? (shipState.Thrust ? ::cho::GRAY		: ::cho::GRAY 		)
			: (physicsId % 3)						? (shipState.Thrust ? ::cho::CYAN		: ::cho::RED 		)
			: (physicsId % 2)						? (shipState.Thrust ? ::cho::WHITE		: ::cho::ORANGE		)
			: ::cho::YELLOW 
			;
		float																		maxFactor	= .5f;
		float																		range		= 3.f;
		maxFactor																*= (1.0f - ::cho::min(1.0f, thrustToDraw.TimeLived / 4));
		range																	= physicsId % 2 + (1.0f - ::cho::min(1.0f, thrustToDraw.TimeLived / 4));
		::cho::drawPixelLight(viewOffscreen, particlePosition, viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x], maxFactor, range);
	}
	return 0;
}

static constexpr	const ::cho::SColorBGRA								powerupFamilyColorPalette []				= 
	{ ::cho::LIGHTYELLOW
	, ::cho::LIGHTGREEN
	, ::cho::LIGHTCYAN
	, ::cho::RED
	};

static				::cho::error_t										drawPowerup						(::SApplication& applicationInstance, POWERUP_FAMILY powFamily, const ::cho::array_view<::cho::grid_view<::cho::SColorBGRA>>& texturePowerup, const ::cho::SCoord2<int32_t>& textureCenterPowerup, const ::cho::SCoord2<float>& powPosition, const ::cho::array_view<const ::cho::SCoord2<int32_t>>& lightPos, double time)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= framework.Offscreen.View;
	::cho::SCoord2<int32_t>														position									= powPosition.Cast<int32_t>();
	for(uint32_t iTex = 0, textureCount = texturePowerup.size(); iTex < textureCount; ++iTex)
		error_if(errored(::cho::grid_copy_alpha(viewOffscreen, texturePowerup[iTex], position - textureCenterPowerup, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	::cho::SCoord2<int32_t>															centerPowerup								= position;
	uint32_t																	lightIndex									= (uint32_t)time % (lightPos.size() / 2); 
	const ::cho::SCoord2<int32_t>												& selectedLightPos0							= lightPos[lightIndex + 0]
		,																		& selectedLightPos2							= lightPos[lightIndex + 4]
		;
	const ::cho::SColorBGRA														& selectedColor								= powerupFamilyColorPalette[powFamily];
	::cho::drawPixelLight(viewOffscreen, selectedLightPos0.Cast<float>(), selectedColor, .3f, 3.0f);
	::cho::drawPixelLight(viewOffscreen, selectedLightPos2.Cast<float>(), selectedColor, .3f, 3.0f);
	return 0;
}
					::cho::error_t										drawPowerups								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	static double																timer										= 0;
	timer																	+= framework.FrameInfo.Seconds.LastFrame * 2;
	::SGame																		& gameInstance								= applicationInstance.Game;

	for(uint32_t iPow = 0, powCount = gameInstance.Powerups.Alive.size(); iPow < powCount; ++iPow) {
		if(0 == gameInstance.Powerups.Alive[iPow])
			continue;
		POWERUP_FAMILY																powFamily									= gameInstance.Powerups.Family[iPow];
		::cho::SCoord2<int32_t>														position									= gameInstance.Powerups.Position[iPow].Cast<int32_t>();
		int32_t																		halfWidth									= 6;
		::cho::SCoord2<int32_t>														lightPos [8]								;
		::cho::array_view<::cho::grid_view<::cho::SColorBGRA>>						texturePowerup								;
		::cho::SCoord2<int32_t>														textureCenterPowerup						;
		if(powFamily == POWERUP_FAMILY_HEALTH || powFamily == POWERUP_FAMILY_BUFF) { // draw square powerup box
			lightPos[0]																= position + ::cho::SCoord2<int32_t>{-halfWidth	, -halfWidth - 1}		;
			lightPos[1]																= position + ::cho::SCoord2<int32_t>{-halfWidth - 1, -halfWidth}		;
			lightPos[2]																= position + ::cho::SCoord2<int32_t>{-halfWidth - 1,  halfWidth - 1}	;
			lightPos[3]																= position + ::cho::SCoord2<int32_t>{-halfWidth	,  halfWidth}			;
			lightPos[4]																= position + ::cho::SCoord2<int32_t>{ halfWidth - 1,  halfWidth}		;
			lightPos[5]																= position + ::cho::SCoord2<int32_t>{ halfWidth	,  halfWidth - 1}		;
			lightPos[6]																= position + ::cho::SCoord2<int32_t>{ halfWidth	,  -halfWidth}			;
			lightPos[7]																= position + ::cho::SCoord2<int32_t>{ halfWidth - 1,  -halfWidth - 1}	;
			texturePowerup															= applicationInstance.StuffToDraw.TexturesPowerup0;
			textureCenterPowerup													= applicationInstance.TextureCenters[GAME_TEXTURE_POWERUP0];
		}
		else { // draw diagonal powerup box
			lightPos[0]																= position + ::cho::SCoord2<int32_t>{-1, -halfWidth - 1};
			lightPos[1]																= position + ::cho::SCoord2<int32_t>{ 0, -halfWidth - 1};
			lightPos[2]																= position + ::cho::SCoord2<int32_t>{halfWidth, -1}		;
			lightPos[3]																= position + ::cho::SCoord2<int32_t>{halfWidth,  0}		;
			lightPos[4]																= position + ::cho::SCoord2<int32_t>{ 0, halfWidth}		;
			lightPos[5]																= position + ::cho::SCoord2<int32_t>{-1, halfWidth}		;
			lightPos[6]																= position + ::cho::SCoord2<int32_t>{-halfWidth - 1,  0};
			lightPos[7]																= position + ::cho::SCoord2<int32_t>{-halfWidth - 1, -1};
			texturePowerup															= applicationInstance.StuffToDraw.TexturesPowerup1;
			textureCenterPowerup													= applicationInstance.TextureCenters[GAME_TEXTURE_POWERUP1];
		}
		::drawPowerup(applicationInstance, powFamily, texturePowerup, textureCenterPowerup, position.Cast<float>(), lightPos, timer);
	}
	return 0;
}


static				::cho::error_t										drawCrosshairDiagonal						(::SApplication& applicationInstance, double beaconTimer, const ::cho::SCoord2<int32_t>	& centerCrosshair)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= framework.Offscreen.View;
	int32_t																		halfWidth									= 10 - ((int32_t)beaconTimer % 11);
	::cho::SCoord2<int32_t>														lightCrosshair []							= 
		{ centerCrosshair + ::cho::SCoord2<int32_t>{ halfWidth,  halfWidth }
		, centerCrosshair + ::cho::SCoord2<int32_t>{ halfWidth, -halfWidth - 1 }
		, centerCrosshair + ::cho::SCoord2<int32_t>{-halfWidth - 1, -halfWidth - 1 }
		, centerCrosshair + ::cho::SCoord2<int32_t>{-halfWidth - 1,  halfWidth }
		};
	for(uint32_t iPoint = 0, pointCount = ::cho::size(lightCrosshair); iPoint < pointCount; ++iPoint) {
		::cho::SCoord2<int32_t>														& pointToTest								= lightCrosshair[iPoint];
		::cho::drawPixelLight(viewOffscreen, pointToTest.Cast<float>(), ::cho::SColorBGRA(::cho::RED), .2f, 3.0f);
	}
	return 0;
}

static				::cho::error_t										drawCrosshairAligned						(::SApplication& applicationInstance, double beaconTimer, const ::cho::SCoord2<int32_t>	& centerCrosshair)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= framework.Offscreen.View;
	int32_t																		halfWidth									= 10 - ((int32_t)beaconTimer % 11);
	const ::cho::SCoord2<int32_t>												lightCrosshair []							= 
		{ centerCrosshair + ::cho::SCoord2<int32_t>{-1, -halfWidth - 1}
		, centerCrosshair + ::cho::SCoord2<int32_t>{ 0, -halfWidth - 1}
		, centerCrosshair + ::cho::SCoord2<int32_t>{halfWidth, -1}
		, centerCrosshair + ::cho::SCoord2<int32_t>{halfWidth,  0}
		, centerCrosshair + ::cho::SCoord2<int32_t>{ 0, halfWidth}
		, centerCrosshair + ::cho::SCoord2<int32_t>{-1, halfWidth}
		, centerCrosshair + ::cho::SCoord2<int32_t>{-halfWidth - 1,  0}
		, centerCrosshair + ::cho::SCoord2<int32_t>{-halfWidth - 1, -1}
		};
	
	for(uint32_t iPoint = 0, pointCount = ::cho::size(lightCrosshair); iPoint < pointCount; ++iPoint) {
		const ::cho::SCoord2<int32_t>												& pointToTest								= lightCrosshair[iPoint];
		::cho::drawPixelLight(viewOffscreen, pointToTest.Cast<float>()
			, (0 == (int32_t)beaconTimer % 5) ? ::cho::SColorBGRA(::cho::RED			) 
			: (0 == (int32_t)beaconTimer % 3) ? ::cho::SColorBGRA(::cho::LIGHTGREEN		)
			: (0 == (int32_t)beaconTimer % 2) ? ::cho::SColorBGRA(::cho::LIGHTYELLOW	) 
			: ::cho::SColorBGRA(::cho::LIGHTCYAN)
			, .2f, 3.0f
			);
	}
	return 0;
}
					::cho::error_t										drawCrosshair								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	static double																beaconTimer									= 0;
	beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 10;
	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iShip = 0, shipCount = gameInstance.Ships.Alive.size(); iShip < shipCount; ++iShip) {
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		::cho::SCoord2<int32_t>														posXHair									= gameInstance.PositionCrosshair[iShip].Cast<int32_t>();
		if(false == gameInstance.Ships.LineOfFire[iShip]) 
			::drawCrosshairDiagonal(applicationInstance, beaconTimer, posXHair);
		error_if(errored(::cho::grid_copy_alpha(framework.Offscreen.View, applicationInstance.Textures[GAME_TEXTURE_CROSSHAIR].Processed.View, posXHair - applicationInstance.TextureCenters[GAME_TEXTURE_CROSSHAIR], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		if(gameInstance.Ships.LineOfFire[iShip]) 
			::drawCrosshairAligned(applicationInstance, beaconTimer, posXHair);
	}
	return 0;
}
