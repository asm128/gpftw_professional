#include "application.h"
#include "cho_grid_copy.h"
#include "cho_bitmap_target.h"

//
//static				::cho::error_t										raster_callback						(void* bitmapTarget, const ::cho::SCoord2<uint32_t>& bitmapMetrics, const ::cho::SCoord2<uint32_t>& cellPos, const void* value) {
//	::cho::grid_view<::cho::SColorBGRA>											target								= {(::cho::SColorBGRA*)bitmapTarget, bitmapMetrics};
//	::cho::drawPixelLight(target, cellPos.Cast<float>(), *(::cho::SColorBGRA*)value, .1f, 3.0f);
//	return 0;
//}
					::cho::error_t										drawShips									(::SApplication& applicationInstance)											{
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::STexture<::cho::SColorBGRA>											& offscreen									= framework.Offscreen;
	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TextureShip.Original.View, applicationInstance.CenterPositionShip.Cast<int32_t>() - applicationInstance.TextureCenterShip, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	return 0;
}

					::cho::error_t										drawShots									(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::STexture<::cho::SColorBGRA>											& offscreen									= framework.Offscreen;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= offscreen.View;
	for(uint32_t iRay = 0, rayCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iRay < rayCount; ++iRay) {
		//::cho::rasterLine(viewOffscreen, ::cho::SColorBGRA(::cho::RED), applicationInstance.ProjectilePaths[iRay], raster_callback);
		//::cho::drawLine(viewOffscreen, ::cho::SColorBGRA(::cho::RED), applicationInstance.ProjectilePaths[iRay]);
		applicationInstance.CacheLinePoints.clear();
		::cho::drawLine(viewOffscreen.metrics(), applicationInstance.StuffToDraw.ProjectilePaths[iRay], applicationInstance.CacheLinePoints);
		for(uint32_t iLinePoint = 0, pointCount = applicationInstance.CacheLinePoints.size(); iLinePoint < pointCount; ++iLinePoint) {
			const ::cho::SCoord2<float>								& pointToDraw									= applicationInstance.CacheLinePoints[iLinePoint].Cast<float>();
			::cho::drawPixelLight(viewOffscreen, pointToDraw, ::cho::SColorBGRA(::cho::RED), .15f, 3.0f);
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
	applicationInstance.StuffToDraw.Stars.clear();
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
	applicationInstance.StuffToDraw.Thrust.clear();
	return 0;
}

					::cho::error_t										drawPowerups								(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::STexture<::cho::SColorBGRA>											& offscreen									= framework.Offscreen;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= offscreen.View;
	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TexturePowerup	.Original.View, applicationInstance.CenterPositionPowerup	.Cast<int32_t>() - applicationInstance.TextureCenterPowerup	, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	{
		static double					beaconTimer			= 0;
		beaconTimer					+= framework.FrameInfo.Seconds.LastFrame * 4;
		::cho::SCoord2<int32_t>			centerPowerup	= applicationInstance.CenterPositionPowerup.Cast<int32_t>();
		int32_t							halfWidth		= 5; //(framework.FrameInfo.FrameNumber / 100) % 6;
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
			{ ::cho::RED
			, ::cho::LIGHTCYAN
			, ::cho::LIGHTGREEN
			, ::cho::LIGHTYELLOW
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
	if((int32_t)framework.FrameInfo.Seconds.Total % 2)
	{
		static double					beaconTimer			= 0;
		beaconTimer					+= framework.FrameInfo.Seconds.LastFrame * 8;
		::cho::SCoord2<int32_t>			centerPowerup	= applicationInstance.CenterPositionCrosshair.Cast<int32_t>();
		int32_t							halfWidth		= 7 - ((int32_t)beaconTimer % 8);
		::cho::SCoord2<int32_t>			lightCrosshair []		= 
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
	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TextureCrosshair[0].Original.View, applicationInstance.CenterPositionCrosshair.Cast<int32_t>() - applicationInstance.TextureCenterCrosshair, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	if(0 == ((int32_t)framework.FrameInfo.Seconds.Total % 2))
	{ // This works
		static double					beaconTimer			= 0;
		beaconTimer					+= framework.FrameInfo.Seconds.LastFrame * 8;
		::cho::SCoord2<int32_t>			centerPowerup	= applicationInstance.CenterPositionCrosshair.Cast<int32_t>();
		int32_t							halfWidth		= 7 - ((int32_t)beaconTimer % 8);
		::cho::SCoord2<int32_t>			lightCrosshair []		= 
			{ centerPowerup + ::cho::SCoord2<int32_t>{-1, -halfWidth - 1}
			, centerPowerup + ::cho::SCoord2<int32_t>{ 0, -halfWidth - 1}
			, centerPowerup + ::cho::SCoord2<int32_t>{halfWidth, -1}
			, centerPowerup + ::cho::SCoord2<int32_t>{halfWidth,  0}
			, centerPowerup + ::cho::SCoord2<int32_t>{ 0, halfWidth}
			, centerPowerup + ::cho::SCoord2<int32_t>{-1, halfWidth}
			, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth - 1,  0}
			, centerPowerup + ::cho::SCoord2<int32_t>{-halfWidth - 1, -1}
			};
	
		for(uint32_t iPoint = 0, pointCount = ::cho::size(lightCrosshair); iPoint < pointCount; ++iPoint) {
			::cho::SCoord2<int32_t>					& pointToTest				= lightCrosshair[iPoint];
			::cho::drawPixelLight(viewOffscreen, pointToTest.Cast<float>(), ::cho::SColorBGRA(::cho::RED), .2f, 3.0f);
		}
	}	return 0;
}
