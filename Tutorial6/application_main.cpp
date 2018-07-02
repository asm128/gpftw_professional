// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "cho_bitmap_file.h"
#include "cho_bitmap_target.h"
#include "cho_grid_scale.h"
#include "cho_bit_view.h"

#include "cho_app_impl.h"

CHO_DEFINE_APPLICATION_ENTRY_POINT(::SApplication);	

static ::SApplication::TParticleSystem::TIntegrator::TParticle			particleDefinitions	[::PARTICLE_TYPE_COUNT]	= {};

static				void												setupParticles								()																				{
	particleDefinitions	[::PARTICLE_TYPE_LASER		].Position				= 
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST].Position				= 
	particleDefinitions	[::PARTICLE_TYPE_STAR		].Position				= {};

	particleDefinitions	[::PARTICLE_TYPE_LASER		].SetMass				(1);
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST].SetMass				(1);
	particleDefinitions	[::PARTICLE_TYPE_STAR		].SetMass				(1);

	particleDefinitions	[::PARTICLE_TYPE_LASER		].Damping				= 1.0f;
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST].Damping				= 0.80f;
	particleDefinitions	[::PARTICLE_TYPE_STAR		].Damping				= 1.0f;

	particleDefinitions	[::PARTICLE_TYPE_LASER		].Forces.Velocity		= {};
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST].Forces.Velocity		= {};
	particleDefinitions	[::PARTICLE_TYPE_STAR		].Forces.Velocity		= {};
}

					::SApplication										* g_ApplicationInstance						= 0;

static				::cho::error_t										updateSizeDependentResources				(::SApplication& applicationInstance)											{ 
	static constexpr	const ::cho::SCoord2<uint32_t>							newSize										= {640, 360};// applicationInstance.Framework.MainDisplay.Size / 4; // {1280, 720};//
	::cho::updateSizeDependentTarget(applicationInstance.Framework.Offscreen.Texels, applicationInstance.Framework.Offscreen.View, newSize);
	return 0;
}

// --- Cleanup application resources.
					::cho::error_t										cleanup										(::SApplication& applicationInstance)											{
#if defined(CHO_WINDOWS)
	::cho::SDisplayPlatformDetail												& displayDetail								= applicationInstance.Framework.MainDisplay.PlatformDetail;
	if(displayDetail.WindowHandle) {
		error_if(0 == ::DestroyWindow(displayDetail.WindowHandle), "Not sure why would this fail.");
		error_if(errored(::cho::displayUpdate(applicationInstance.Framework.MainDisplay)), "Not sure why this would fail");
	}
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
#endif

	g_ApplicationInstance													= 0;
	//error_printf("Error message test. Press F5 to continue if the debugger breaks execution at this point.");
	return 0;
}

static				::cho::error_t										setupSprite									(::cho::STextureProcessable<::cho::SColorBGRA>& textureToProcess, ::cho::SCoord2<int32_t>& textureCenter, const ::cho::view_const_string& filename)	{ 
	error_if(errored(::cho::bmpFileLoad(filename, textureToProcess.Original)), "Failed to load bitmap from file: %s.", filename);
	textureCenter															= (textureToProcess.Original.View.metrics() / 2).Cast<int32_t>();
	textureToProcess.Processed.View											= textureToProcess.Original.View;
	return 0;
}

static				::cho::error_t										setupSprites								(::SApplication& applicationInstance)											{ 
	static constexpr	const char*												bmpFileNames	[]							= 
		{ "ship_1.bmp"
		, "pow_core_0.bmp"
		, "pow_core_1.bmp"
		, "crosshair_template.bmp"
		, "pow_icon_0.bmp"
		, "enemy_0.bmp"
		};
	for(uint32_t iSprite = 0, spriteCount = ::cho::size(bmpFileNames); iSprite < spriteCount; ++iSprite)
		::setupSprite(applicationInstance.Textures[iSprite], applicationInstance.TextureCenters[iSprite], {bmpFileNames[iSprite], (uint32_t)strlen(bmpFileNames[iSprite])});

	applicationInstance.TexturesPowerup0.push_back(applicationInstance.Textures[GAME_TEXTURE_POWERUP0	].Processed.View);
	applicationInstance.TexturesPowerup0.push_back(applicationInstance.Textures[GAME_TEXTURE_POWICON	].Processed.View);
	applicationInstance.TexturesPowerup1.push_back(applicationInstance.Textures[GAME_TEXTURE_POWERUP1	].Processed.View);
	applicationInstance.TexturesPowerup1.push_back(applicationInstance.Textures[GAME_TEXTURE_POWICON	].Processed.View);
	return 0;
}

					::cho::error_t										mainWindowCreate							(::cho::SDisplay& mainWindow, HINSTANCE hInstance);
					::cho::error_t										setup										(::SApplication& applicationInstance)											{ 
	//_CrtSetBreakAlloc(120);
	g_ApplicationInstance													= &applicationInstance;
	error_if(errored(::mainWindowCreate(applicationInstance.Framework.MainDisplay, applicationInstance.Framework.RuntimeValues.PlatformDetail.EntryPointArgs.hInstance)), "Failed to create main window why?????!?!?!?!?");
	::setupParticles();
	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	ree_if	(errored(::setupSprites					(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	applicationInstance.PositionShip									= applicationInstance.Framework.Offscreen.View.metrics().Cast<float>() / 2U;
	applicationInstance.PositionPowerup								= applicationInstance.Framework.Offscreen.View.metrics().Cast<float>() / 4U * 3U;
	applicationInstance.PositionEnemy									= applicationInstance.Framework.Offscreen.View.metrics().Cast<float>() / 4U;
	applicationInstance.PositionCrosshair								= applicationInstance.PositionShip + ::cho::SCoord2<float>{64,};

	applicationInstance.PSOffsetFromShipCenter								= {-applicationInstance.TextureCenters[GAME_TEXTURE_SHIP].x};
	return 0;
}

					::cho::error_t										drawBackground								(::SApplication& applicationInstance);	
					::cho::error_t										drawShots									(::SApplication& applicationInstance);	
					::cho::error_t										drawThrust									(::SApplication& applicationInstance);	
					::cho::error_t										drawPowerups								(::SApplication& applicationInstance);	
					::cho::error_t										drawShips									(::SApplication& applicationInstance);	
					::cho::error_t										drawCrosshair								(::SApplication& applicationInstance);	
					::cho::error_t										drawCollisions								(::SApplication& applicationInstance);	
					::cho::error_t										draw										(::SApplication& applicationInstance)											{	
	error_if(errored(::drawBackground		(applicationInstance)), "Why??");		// --- Draw stars
	error_if(errored(::drawPowerups			(applicationInstance)), "Why??");		// --- Draw powerups
	error_if(errored(::drawShips			(applicationInstance)), "Why??");		// --- Draw ship
	error_if(errored(::drawCrosshair		(applicationInstance)), "Why??");		// --- Draw crosshair
	error_if(errored(::drawThrust			(applicationInstance)), "Why??");		// --- Draw propulsion engine
	error_if(errored(::drawShots			(applicationInstance)), "Why??");		// --- Draw lasers
	error_if(errored(::drawCollisions		(applicationInstance)), "Why??");		// --- Draw lasers
	return 0;
}

 					::cho::error_t										updateGUI									(::SApplication& applicationInstance);
					::cho::error_t										updateShots									(::SApplication& applicationInstance, const ::cho::array_view<::SApplication::TParticleSystem::TIntegrator::TParticle> & particleDefinitions);
					::cho::error_t										updateSpawn									(::SApplication& applicationInstance, const ::cho::array_view<::SApplication::TParticleSystem::TIntegrator::TParticle> & particleDefinitions);
					::cho::error_t										updateShip									(::SApplication& applicationInstance);
					::cho::error_t										updateParticles								(::SApplication& applicationInstance);
					::cho::error_t										updateInput									(::SApplication& applicationInstance);
					::cho::error_t										update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	::cho::SFramework															& framework									= applicationInstance.Framework;
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");

	::cho::error_t																frameworkResult								= ::cho::updateFramework(framework);
	ree_if	(errored(frameworkResult), "Unknown error.");
	rvi_if	(1, frameworkResult == 1, "Framework requested close. Terminating execution.");

	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	error_if(errored(::updateInput					(applicationInstance)), "Unknown error.");

	// update background
	const float																	windDirection								= (float)(sin(framework.FrameInfo.Seconds.Total / 10.0) * .5 + .5);
	applicationInstance.ColorBackground.g									= (uint8_t)(windDirection * (applicationInstance.ColorBackground.b / 3.0));
	applicationInstance.ColorBackground.r									= (uint8_t)(windDirection * (applicationInstance.ColorBackground.b / 3.0));

	error_if(errored(::updateParticles				(applicationInstance)), "Unknown error.");
	error_if(errored(::updateSpawn					(applicationInstance, particleDefinitions)), "Unknown error.");
	error_if(errored(::updateShip					(applicationInstance)), "Unknown error.");

	::cho::SFramework::TOffscreen												& offscreen									= applicationInstance.Framework.Offscreen;
	{
		static float																timerPath									= 0;
		timerPath																+= (float)framework.Timer.LastTimeSeconds;
		if(timerPath > 10.0f) {
			timerPath = 0;
			++applicationInstance.PathStep;
			if(applicationInstance.PathStep >= ::cho::size(applicationInstance.PathEnemy))
				applicationInstance.PathStep = 0;
		}
		const ::cho::SCoord2<float>													& pathTarget							= applicationInstance.PathEnemy[applicationInstance.PathStep];
		::cho::SCoord2<float>														directionEnemy							= (pathTarget - applicationInstance.PositionEnemy);
		if(directionEnemy.LengthSquared() < 0.5) {
			timerPath = 0;
			++applicationInstance.PathStep;
			if(applicationInstance.PathStep >= ::cho::size(applicationInstance.PathEnemy))
				applicationInstance.PathStep = 0;
		}
		else {
			directionEnemy.Normalize();
			// update enemy
			applicationInstance.PositionEnemy									+= directionEnemy * (float)(framework.FrameInfo.Seconds.LastFrame * 100);// * (applicationInstance.ShipState.Brakes ? .25f : (applicationInstance.ShipState.Thrust ? 2 : 1));
			applicationInstance.PositionEnemy									= 
				{ ::cho::clamp(applicationInstance.PositionEnemy.x, .1f, (float)offscreen.View.metrics().x - 1)
				, ::cho::clamp(applicationInstance.PositionEnemy.y, .1f, (float)offscreen.View.metrics().y - 1)
				};
		}
	}

	{ // update crosshair 
		applicationInstance.PositionCrosshair								= applicationInstance.PositionShip + ::cho::SCoord2<float>{96,};
		applicationInstance.PositionCrosshair.x							= ::cho::min(applicationInstance.PositionCrosshair.x, (float)offscreen.View.metrics().x);
	}
	error_if(errored(::updateShots					(applicationInstance, particleDefinitions)), "Unknown error.");
	error_if(errored(::updateGUI					(applicationInstance)), "Unknown error.");
	::cho::STimer																& timer										= framework.Timer;
	::cho::SDisplay																& mainWindow								= framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. Particle count: %u. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, applicationInstance.ParticleSystem.Instances.size(), 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}
