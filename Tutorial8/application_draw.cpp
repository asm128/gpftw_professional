// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"
#include "cho_grid_copy.h"
#include "cho_bitmap_target.h"

static				::cho::error_t										drawShipHealthBar								(::SApplication& applicationInstance, const ::cho::SCoord2<float> & centerEnemy, const ::cho::SCoord2<int32_t> & halfMetrics, uint32_t health, int32_t yOffset, const ::cho::SColorBGRA & finalColor)											{
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
		::cho::drawPixelLight(viewOffscreen, pointToDraw, finalColor, .15f, 2.0f);
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
	for(uint32_t iEnemy = 0, enemyCount = gameInstance.Enemies.Alive.size(); iEnemy < enemyCount; ++iEnemy) {
		if(0 == gameInstance.Enemies.Alive[iEnemy])
			continue;
		const ::cho::SCoord2<float>													& centerEnemy								= gameInstance.Enemies.Position[iEnemy];
		const ::SHealthPoints														& enemyHealth								= gameInstance.Enemies.Health[iEnemy];
		error_if(errored(::cho::grid_copy_alpha(viewOffscreen, enemyView, centerEnemy.Cast<int32_t>() - applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		{
			static double																beaconTimer									= 0;
			beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 8;
			int32_t																		selectedPos									= ((int32_t)beaconTimer % ::cho::size(indexPositionsX));
			::cho::SCoord2<float>														lightCrosshair								= centerEnemy + ::cho::SCoord2<float>{(float)indexPositionsX[selectedPos], 0.0f};
			::cho::drawPixelLight(viewOffscreen, lightCrosshair.Cast<float>(), ::cho::SColorBGRA(::cho::RED), .2f, 3.0f);
			::cho::SCoord2<int32_t>														halfMetrics									= (enemyView.metrics() / 2).Cast<int32_t>();
			::drawShipHealthBar(applicationInstance, centerEnemy, halfMetrics, enemyHealth.Health, (int32_t)(halfMetrics.y * 2		), ::cho::GREEN);
			::drawShipHealthBar(applicationInstance, centerEnemy, halfMetrics, enemyHealth.Shield, (int32_t)(halfMetrics.y * 1.75	), ::cho::CYAN );
		}
		// ---- Draw ghosts
		static constexpr const ::cho::SCoord2<float>								reference	= {1, 0};
		::cho::SCoord2<float>														vector;
		for(uint32_t iGhost = 0; iGhost < 5; ++iGhost) {
			vector																	= reference * (64 * sin(framework.FrameInfo.Seconds.Total));
			vector.Rotate(::cho::math_2pi / 5 * iGhost + gameInstance.Enemies.TimeLived[iEnemy]);
			error_if(errored(::cho::grid_copy_alpha(viewOffscreen, enemyView, (gameInstance.Enemies.Position[iEnemy] + vector).Cast<int32_t>() - applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
			{
				static double																beaconTimer								= 0;
				beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 8;
				::cho::SCoord2<float>														centerPowerup							= gameInstance.Enemies.Position[iEnemy] + vector;
				int32_t																		selectedPos								= ((int32_t)beaconTimer % ::cho::size(indexPositionsX));
				::cho::SCoord2<float>														lightCrosshair							= centerPowerup + ::cho::SCoord2<float>{(float)indexPositionsX[selectedPos], 0.0f};
				::cho::drawPixelLight(viewOffscreen, lightCrosshair.Cast<float>(), ::cho::SColorBGRA(::cho::YELLOW), .2f, 3.0f);
			}
		}
	}
	// ---- Draw player ships
	for(uint32_t iShip = 0, shipCount = gameInstance.ShipsPlaying; iShip < shipCount; ++iShip) {
		if(0 == gameInstance.Ships.Alive[iShip])
			continue;
		const ::cho::grid_view<::cho::SColorBGRA>									& shipView									= applicationInstance.Textures[GAME_TEXTURE_SHIP0 + iShip].Processed.View;
		const ::cho::SCoord2<float>													& centerEnemy								= gameInstance.Ships.Position	[iShip];
		const ::SHealthPoints														& enemyHealth								= gameInstance.Ships.Health	[iShip];
		::cho::SCoord2<int32_t>														halfMetrics									= (shipView.metrics() / 2).Cast<int32_t>();
		::drawShipHealthBar(applicationInstance, centerEnemy, halfMetrics, enemyHealth.Health, (int32_t)(halfMetrics.y * 2		), ::cho::GREEN);
		::drawShipHealthBar(applicationInstance, centerEnemy, halfMetrics, enemyHealth.Shield, (int32_t)(halfMetrics.y * 1.75	), ::cho::CYAN );
		error_if(errored(::cho::grid_copy_alpha(viewOffscreen, shipView, gameInstance.Ships.Position[iShip].Cast<int32_t>() - applicationInstance.TextureCenters[GAME_TEXTURE_SHIP0 + iShip], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
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
		if(applicationInstance.ParticleSystem.Instances[particleToDraw.IndexParticle].Type.Lit) {
			::cho::SColorBGRA															finalColor										
				= (0 == (particleToDraw.Id % 3)) ? ::cho::SColorBGRA(::cho::YELLOW) 
				: (0 == (particleToDraw.Id % 2)) ? ::cho::SColorBGRA(::cho::RED) 
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
		const ::cho::SParticleInstance<::SGameParticle>								& gameParticle								= applicationInstance.ParticleSystem.Instances[laserToDraw.IndexParticle];
		const ::cho::SColorBGRA														finalColor									= weaponTypeColorPalette[gameParticle.Type.TypeWeapon];
		for(uint32_t iLinePoint = 0, pointCount = applicationInstance.CacheLinePoints.size(); iLinePoint < pointCount; ++iLinePoint) {
			const ::cho::SCoord2<float>													& pointToDraw								= applicationInstance.CacheLinePoints[iLinePoint].Cast<float>();
			::cho::drawPixelLight(viewOffscreen, pointToDraw, finalColor, (gameParticle.Type.TypePlayer == PLAYER_TYPE_PLAYER) ? .15f : .95f, 3.0f);
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
		if(false == applicationInstance.ParticleSystem.Instances[starToDraw.IndexParticle].Type.Lit)
			continue;
		::cho::SCoord2<int32_t>														& particlePosition							= starToDraw.Position;
		viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	
			= (0 == (starToDraw.Id % 7))	? ::cho::DARKYELLOW	/ 2.0f
			: (0 == (starToDraw.Id % 6))	? ::cho::GRAY 
			: (0 == (starToDraw.Id % 5))	? ::cho::WHITE
			: (0 == (starToDraw.Id % 4))	? ::cho::DARKGRAY 
			: (0 == (starToDraw.Id % 3))	? ::cho::GRAY 
			: (0 == (starToDraw.Id % 2))	? ::cho::WHITE
			: ::cho::DARKGRAY 
			;
		float																		maxFactor	= .5f;
		float																		range		= 3.f;
		maxFactor																= (rand() % 3 + 1) * 0.10f;
		range																	= starToDraw.Id % 3 + 1.0f;
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
		if(false == applicationInstance.ParticleSystem.Instances[thrustToDraw.IndexParticle].Type.Lit)
			continue;
		const int32_t																physicsId									= thrustToDraw.Id;
		const ::cho::SCoord2<float>													& particlePosition							= applicationInstance.ParticleSystem.Integrator.Particle[physicsId].Position;
		if(false == ::cho::in_range(particlePosition, {{}, offscreen.View.metrics().Cast<float>()}))
			continue;
		viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	
			= (thrustToDraw.TimeLived > .075)		? (gameInstance.Ships.States[0].Thrust ? ::cho::DARKGRAY	: ::cho::DARKGRAY	)
			: (thrustToDraw.TimeLived > .03 )		? (gameInstance.Ships.States[0].Thrust ? ::cho::GRAY		: ::cho::GRAY 		)
			: (physicsId % 3)						? (gameInstance.Ships.States[0].Thrust ? ::cho::CYAN		: ::cho::RED 		)
			: (physicsId % 2)						? (gameInstance.Ships.States[0].Thrust ? ::cho::WHITE		: ::cho::ORANGE		)
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

static				::cho::error_t										drawSquarePowerup							(::SApplication& applicationInstance, POWERUP_FAMILY powFamily, const ::cho::SCoord2<float>& powPosition, double time)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= framework.Offscreen.View;
	::cho::SCoord2<int32_t>														offset										= {0, 0};
	::cho::SCoord2<int32_t>														position									= powPosition.Cast<int32_t>() + offset;
	for(uint32_t iTex = 0, textureCount = applicationInstance.StuffToDraw.TexturesPowerup0.size(); iTex < textureCount; ++iTex)
		error_if(errored(::cho::grid_copy_alpha(viewOffscreen, applicationInstance.StuffToDraw.TexturesPowerup0[iTex], position - applicationInstance.TextureCenters[GAME_TEXTURE_POWERUP0], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	::cho::SCoord2<int32_t>														centerPowerup								= position;
	int32_t																		halfWidth									= 6; //(framework.FrameInfo.FrameNumber / 100) % 6;
	::cho::SCoord2<int32_t>														lightPos []									= 
		{ centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth	, -halfWidth - 1}
		, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth - 1, -halfWidth}
		, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth - 1,  halfWidth - 1}
		, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth	,  halfWidth}
		, centerPowerup + ::cho::SCoord2<int32_t>{ halfWidth - 1,  halfWidth}
		, centerPowerup + ::cho::SCoord2<int32_t>{ halfWidth	,  halfWidth - 1}
		, centerPowerup + ::cho::SCoord2<int32_t>{ halfWidth	,  -halfWidth}
		, centerPowerup + ::cho::SCoord2<int32_t>{ halfWidth - 1,  -halfWidth - 1}
		};
	::cho::SCoord2<int32_t>														selectedLightPos0							= lightPos[((uint32_t)time % (::cho::size(lightPos) / 2)) + 0]
		,																		selectedLightPos2							= lightPos[((uint32_t)time % (::cho::size(lightPos) / 2)) + 4]
		;
	int32_t																		selectedColor								= powFamily;
	::cho::drawPixelLight(viewOffscreen, selectedLightPos0.Cast<float>(), powerupFamilyColorPalette[selectedColor], .3f, 3.0f);
	::cho::drawPixelLight(viewOffscreen, selectedLightPos2.Cast<float>(), powerupFamilyColorPalette[selectedColor], .3f, 3.0f);
	return 0;
}

static				::cho::error_t										drawDiagonalPowerup						(::SApplication& applicationInstance, POWERUP_FAMILY powFamily, const ::cho::SCoord2<float>& powPosition, double time)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= framework.Offscreen.View;
	::cho::SCoord2<int32_t>														position									= powPosition.Cast<int32_t>();
	for(uint32_t iTex = 0, textureCount = applicationInstance.StuffToDraw.TexturesPowerup1.size(); iTex < textureCount; ++iTex)
		error_if(errored(::cho::grid_copy_alpha(viewOffscreen, applicationInstance.StuffToDraw.TexturesPowerup1[iTex], position - applicationInstance.TextureCenters[GAME_TEXTURE_POWERUP1], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	::cho::SCoord2<int32_t>															centerPowerup								= position;
	int32_t																			halfWidth									= 6; //(framework.FrameInfo.FrameNumber / 100) % 6;
	::cho::SCoord2<int32_t>															lightPos []									= 
		{ centerPowerup + ::cho::SCoord2<int32_t>{-1, -halfWidth - 1}
		, centerPowerup + ::cho::SCoord2<int32_t>{ 0, -halfWidth - 1}
		, centerPowerup + ::cho::SCoord2<int32_t>{halfWidth, -1}
		, centerPowerup + ::cho::SCoord2<int32_t>{halfWidth,  0}
		, centerPowerup + ::cho::SCoord2<int32_t>{ 0, halfWidth}
		, centerPowerup + ::cho::SCoord2<int32_t>{-1, halfWidth}
		, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth - 1,  0}
		, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth - 1, -1}
		};
	::cho::SCoord2<int32_t>															selectedLightPos0							= lightPos[((uint32_t)time % (::cho::size(lightPos) / 2)) + 0]
		,																			selectedLightPos2							= lightPos[((uint32_t)time % (::cho::size(lightPos) / 2)) + 4]
		;
	int32_t																			selectedColor								= powFamily;//((int32_t)time / 4) % ::cho::size(colors);
	::cho::drawPixelLight(viewOffscreen, selectedLightPos0.Cast<float>(), powerupFamilyColorPalette[selectedColor], .3f, 3.0f);
	::cho::drawPixelLight(viewOffscreen, selectedLightPos2.Cast<float>(), powerupFamilyColorPalette[selectedColor], .3f, 3.0f);
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
		if(gameInstance.Powerups.Family[iPow] == POWERUP_FAMILY_WEAPON)
			drawSquarePowerup(applicationInstance, powFamily, gameInstance.Powerups.Position[iPow], timer);
		else
			drawDiagonalPowerup(applicationInstance, powFamily, gameInstance.Powerups.Position[iPow], timer);
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
		if(false == gameInstance.Ships.LineOfFire[iShip]) 
			::drawCrosshairDiagonal(applicationInstance, beaconTimer, gameInstance.PositionCrosshair[iShip].Cast<int32_t>());
		error_if(errored(::cho::grid_copy_alpha(framework.Offscreen.View, applicationInstance.Textures[GAME_TEXTURE_CROSSHAIR].Processed.View, gameInstance.PositionCrosshair[iShip].Cast<int32_t>() - applicationInstance.TextureCenters[GAME_TEXTURE_CROSSHAIR], {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		if(gameInstance.Ships.LineOfFire[iShip]) 
			::drawCrosshairAligned(applicationInstance, beaconTimer, gameInstance.PositionCrosshair[iShip].Cast<int32_t>());
	}
	return 0;
}
