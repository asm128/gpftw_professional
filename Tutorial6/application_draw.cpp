#include "application.h"
#include "cho_grid_copy.h"
#include "cho_bitmap_target.h"
//
					::cho::error_t										drawShips									(::SApplication& applicationInstance)											{
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::STexture<::cho::SColorBGRA>											& offscreen									= framework.Offscreen;

	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TextureEnemy.Processed.View, applicationInstance.CenterPositionEnemy.Cast<int32_t>() - applicationInstance.TextureCenterEnemy, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	{
		static double																beaconTimer				= 0;
		beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 8;
		::cho::SCoord2<float>														centerPowerup			= applicationInstance.CenterPositionEnemy;
		char																		positionsX[]			= {0, 1, 2, 3, 2, 1, 0, -1, -2, -3, -2, -1};
		int32_t																		selectedPos				= ((int32_t)beaconTimer % ::cho::size(positionsX));
		::cho::SCoord2<float>														lightCrosshair			= centerPowerup + ::cho::SCoord2<float>{(float)positionsX[selectedPos], 0.0f};
		::cho::drawPixelLight(offscreen.View, lightCrosshair.Cast<float>(), ::cho::SColorBGRA(::cho::GREEN), .2f, 3.0f);
	}
	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TextureShip.Processed.View, applicationInstance.CenterPositionShip.Cast<int32_t>() - applicationInstance.TextureCenterShip, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
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
		if(particleToDraw.Lit) {
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

					::cho::error_t										drawShots									(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= applicationInstance.Framework.Offscreen.View;
	for(uint32_t iRay = 0, rayCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iRay < rayCount; ++iRay) {
		const ::SLaserToDraw														& laserToDraw								= applicationInstance.StuffToDraw.ProjectilePaths[iRay];
		applicationInstance.CacheLinePoints.clear();
		::cho::drawLine(viewOffscreen.metrics(), laserToDraw.Segment, applicationInstance.CacheLinePoints);
		::cho::SColorBGRA															finalColor										
			= (0 == (laserToDraw.Id %  5)) ? ::cho::SColorBGRA(::cho::CYAN) 
			: (0 == (laserToDraw.Id %  7)) ? ::cho::SColorBGRA(::cho::YELLOW) 
			: (0 == (laserToDraw.Id % 11)) ? ::cho::SColorBGRA(::cho::RED) 
			: (0 == (laserToDraw.Id % 13)) ? ::cho::SColorBGRA(::cho::MAGENTA) 
			: ::cho::SColorBGRA(::cho::ORANGE)
			;
		for(uint32_t iLinePoint = 0, pointCount = applicationInstance.CacheLinePoints.size(); iLinePoint < pointCount; ++iLinePoint) {
			const ::cho::SCoord2<float>								& pointToDraw									= applicationInstance.CacheLinePoints[iLinePoint].Cast<float>();
			::cho::drawPixelLight(viewOffscreen, pointToDraw, finalColor, .15f, 3.0f);
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
		if(false == starToDraw.Lit)
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
		float																	maxFactor	= .5f;
		float																	range		= 3.f;
		maxFactor															= (rand() % 3 + 1) * 0.10f;
		range																= starToDraw.Id % 3 + 1.0f;
		::cho::drawPixelLight(viewOffscreen, particlePosition.Cast<float>(), viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x], maxFactor, range);
	}
	return 0;
}
					::cho::error_t										drawThrust								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::STexture<::cho::SColorBGRA>											& offscreen									= framework.Offscreen;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= offscreen.View;
	for(uint32_t iThrust = 0, particleCount = (uint32_t)applicationInstance.StuffToDraw.Thrust.size(); iThrust < particleCount; ++iThrust) {
		::SParticleToDraw															& thrustToDraw								= applicationInstance.StuffToDraw.Thrust[iThrust];
		if(false == thrustToDraw.Lit)
			continue;
		const int32_t																physicsId									= thrustToDraw.Id;
		const ::cho::SCoord2<float>													& particlePosition							= applicationInstance.ParticleSystem.Integrator.Particle[physicsId].Position;
		if(false == ::cho::in_range(particlePosition, {{}, offscreen.View.metrics().Cast<float>()}))
			continue;

		viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	
			= (thrustToDraw.TimeLived > .075)		? (applicationInstance.ShipState.Thrust ? ::cho::DARKGRAY	: ::cho::DARKGRAY	)
			: (thrustToDraw.TimeLived > .03 )		? (applicationInstance.ShipState.Thrust ? ::cho::GRAY		: ::cho::GRAY 		)
			: (physicsId % 3)						? (applicationInstance.ShipState.Thrust ? ::cho::CYAN		: ::cho::RED 		)
			: (physicsId % 2)						? (applicationInstance.ShipState.Thrust ? ::cho::WHITE		: ::cho::ORANGE		)
			: ::cho::YELLOW 
			;
		float																	maxFactor	= .5f;
		float																	range		= 3.f;
		maxFactor															*= (1.0f - ::cho::min(1.0f, thrustToDraw.TimeLived / 4));
		range																= physicsId % 2 + (1.0f - ::cho::min(1.0f, thrustToDraw.TimeLived / 4));
		::cho::drawPixelLight(viewOffscreen, particlePosition, viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x], maxFactor, range);
	}
	return 0;
}

					::cho::error_t										drawPowerups								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::STexture<::cho::SColorBGRA>											& offscreen									= framework.Offscreen;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= offscreen.View;
	static float																timer										= 0;
	timer																	+= (float)framework.FrameInfo.Seconds.LastFrame / 2;
	{
		::cho::SCoord2<int32_t>		offset		= {0, 0};
		::cho::SCoord2<int32_t>		position	= applicationInstance.CenterPositionPowerup.Cast<int32_t>() + offset;
		for(uint32_t iTex = 0, textureCount = applicationInstance.TexturesPowerup0.size(); iTex < textureCount; ++iTex)
			error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TexturesPowerup0[iTex], position - applicationInstance.TextureCenterPowerup0, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		static double					beaconTimer			= 0;
		beaconTimer					+= framework.FrameInfo.Seconds.LastFrame * 4;
		::cho::SCoord2<int32_t>			centerPowerup	= position;
		int32_t							halfWidth		= 6; //(framework.FrameInfo.FrameNumber / 100) % 6;
		::cho::SCoord2<int32_t>			lightPos []		= 
			{ centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth	, -halfWidth - 1}
			, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth - 1, -halfWidth}
			, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth - 1,  halfWidth - 1}
			, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth	,  halfWidth}
			, centerPowerup + ::cho::SCoord2<int32_t>{ halfWidth - 1,  halfWidth}
			, centerPowerup + ::cho::SCoord2<int32_t>{ halfWidth	,  halfWidth - 1}
			, centerPowerup + ::cho::SCoord2<int32_t>{ halfWidth	,  -halfWidth}
			, centerPowerup + ::cho::SCoord2<int32_t>{ halfWidth - 1,  -halfWidth - 1}
			};

		::cho::SCoord2<int32_t>		selectedLightPos0	= lightPos[((uint32_t)beaconTimer % (::cho::size(lightPos) / 2)) + 0]
			,						selectedLightPos2	= lightPos[((uint32_t)beaconTimer % (::cho::size(lightPos) / 2)) + 4]
			;

		::cho::SColorBGRA			colors []			= 
			{ ::cho::RED
			, ::cho::LIGHTCYAN
			, ::cho::LIGHTGREEN
			, ::cho::LIGHTYELLOW
			};
		int32_t						selectedColor		= ((int32_t)beaconTimer / 4) % ::cho::size(colors);
		::cho::drawPixelLight(viewOffscreen, selectedLightPos0.Cast<float>(), colors[selectedColor], .3f, 3.0f);
		::cho::drawPixelLight(viewOffscreen, selectedLightPos2.Cast<float>(), colors[selectedColor], .3f, 3.0f);
	}

	{
		::cho::SCoord2<int32_t>		offset		= {-150, -150};
		::cho::SCoord2<int32_t>		position	= applicationInstance.CenterPositionPowerup.Cast<int32_t>() + offset;
		for(uint32_t iTex = 0, textureCount = applicationInstance.TexturesPowerup1.size(); iTex < textureCount; ++iTex)
			error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TexturesPowerup1[iTex], position - applicationInstance.TextureCenterPowerup1, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
		static double					beaconTimer			= 0;
		beaconTimer					+= framework.FrameInfo.Seconds.LastFrame * 4;
		::cho::SCoord2<int32_t>			centerPowerup	= position;
		int32_t							halfWidth		= 6; //(framework.FrameInfo.FrameNumber / 100) % 6;
		::cho::SCoord2<int32_t>			lightPos []		= 
			{ centerPowerup + ::cho::SCoord2<int32_t>{-1, -halfWidth - 1}
			, centerPowerup + ::cho::SCoord2<int32_t>{ 0, -halfWidth - 1}
			, centerPowerup + ::cho::SCoord2<int32_t>{halfWidth, -1}
			, centerPowerup + ::cho::SCoord2<int32_t>{halfWidth,  0}
			, centerPowerup + ::cho::SCoord2<int32_t>{ 0, halfWidth}
			, centerPowerup + ::cho::SCoord2<int32_t>{-1, halfWidth}
			, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth - 1,  0}
			, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth - 1, -1}
			};

		::cho::SCoord2<int32_t>		selectedLightPos0	= lightPos[((uint32_t)beaconTimer % (::cho::size(lightPos) / 2)) + 0]
			,						selectedLightPos2	= lightPos[((uint32_t)beaconTimer % (::cho::size(lightPos) / 2)) + 4]
			;

		::cho::SColorBGRA			colors []			= 
			{ ::cho::LIGHTYELLOW
			, ::cho::RED
			, ::cho::LIGHTGREEN
			, ::cho::LIGHTCYAN
			};
		int32_t						selectedColor		= ((int32_t)beaconTimer / 4) % ::cho::size(colors);
		::cho::drawPixelLight(viewOffscreen, selectedLightPos0.Cast<float>(), colors[selectedColor], .3f, 3.0f);
		::cho::drawPixelLight(viewOffscreen, selectedLightPos2.Cast<float>(), colors[selectedColor], .3f, 3.0f);
	}
	return 0;
}

					::cho::error_t										drawCrosshair								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::STexture<::cho::SColorBGRA>											& offscreen									= framework.Offscreen;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= offscreen.View;
	static double																powTimer									= 0;
	powTimer																+= framework.FrameInfo.Seconds.LastFrame;
	if((int32_t)powTimer % 2) {
		static double																beaconTimer									= 0;
		beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 9;
		::cho::SCoord2<int32_t>														centerPowerup								= applicationInstance.CenterPositionCrosshair.Cast<int32_t>();
		int32_t																		halfWidth									= 8 - ((int32_t)beaconTimer % 9);
		::cho::SCoord2<int32_t>														lightCrosshair []							= 
			{ centerPowerup + ::cho::SCoord2<int32_t>{ halfWidth,  halfWidth }
			, centerPowerup + ::cho::SCoord2<int32_t>{ halfWidth, -halfWidth - 1 }
			, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth - 1, -halfWidth - 1 }
			, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth - 1,  halfWidth }
			};

		for(uint32_t iPoint = 0, pointCount = ::cho::size(lightCrosshair); iPoint < pointCount; ++iPoint) {
			::cho::SCoord2<int32_t>					& pointToTest				= lightCrosshair[iPoint];
			::cho::drawPixelLight(viewOffscreen, pointToTest.Cast<float>(), ::cho::SColorBGRA(::cho::RED), .2f, 3.0f);
		}
	}
	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TextureCrosshair.Processed.View, applicationInstance.CenterPositionCrosshair.Cast<int32_t>() - applicationInstance.TextureCenterCrosshair, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	if(0 == ((int32_t)powTimer % 2)) {
		static double																beaconTimer									= 0;
		beaconTimer																+= framework.FrameInfo.Seconds.LastFrame * 9;
		const ::cho::SCoord2<int32_t>												centerCrosshair								= applicationInstance.CenterPositionCrosshair.Cast<int32_t>();
		int32_t																		halfWidth									= 8 - ((int32_t)beaconTimer % 9);
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
			const ::cho::SCoord2<int32_t>					& pointToTest				= lightCrosshair[iPoint];
			::cho::drawPixelLight(viewOffscreen, pointToTest.Cast<float>()
				, (0 == (int32_t)framework.FrameInfo.Seconds.Total % 5) ? ::cho::SColorBGRA(::cho::RED			) 
				: (0 == (int32_t)framework.FrameInfo.Seconds.Total % 3) ? ::cho::SColorBGRA(::cho::LIGHTGREEN	)
				: (0 == (int32_t)framework.FrameInfo.Seconds.Total % 2) ? ::cho::SColorBGRA(::cho::LIGHTYELLOW	) 
				: ::cho::SColorBGRA(::cho::LIGHTCYAN)
				, .2f, 3.0f);
		}
	}	return 0;
}
