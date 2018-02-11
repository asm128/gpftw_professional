// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "cho_bitmap_target.h"
#include "cho_bitmap_file.h"
#include "cho_grid_copy.h"
#include "cho_grid_scale.h"
#include "cho_bit_array_view.h"

#include "cho_app_impl.h"

static constexpr	const uint32_t										ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t										ASCII_SCREEN_HEIGHT							= 50	;

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
	const ::cho::SCoord2<uint32_t>												newSize										= {640, 360};// applicationInstance.Framework.MainDisplay.Size / 4;
	::cho::updateSizeDependentTarget(applicationInstance.Framework.Offscreen.Texels, applicationInstance.Framework.Offscreen.View, newSize);
	return 0;
}

// --- Cleanup application resources.
					::cho::error_t										cleanup										(::SApplication& applicationInstance)											{
	::cho::SDisplayPlatformDetail												& displayDetail								= applicationInstance.Framework.MainDisplay.PlatformDetail;
	if(displayDetail.WindowHandle) {
		error_if(0 == ::DestroyWindow(displayDetail.WindowHandle), "Not sure why would this fail.");
		error_if(errored(::cho::displayUpdate(applicationInstance.Framework.MainDisplay)), "Not sure why this would fail");
	}
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);

	g_ApplicationInstance													= 0;
	//error_printf("Error message test. Press F5 to continue if the debugger breaks execution at this point.");
	return 0;
}

					::cho::error_t										mainWindowCreate							(::cho::SDisplay& mainWindow, HINSTANCE hInstance);

// --- Initialize console.
					::cho::error_t										setup										(::SApplication& applicationInstance)											{ 
	//_CrtSetBreakAlloc(120);
	g_ApplicationInstance													= &applicationInstance;
	error_if(errored(::mainWindowCreate(applicationInstance.Framework.MainDisplay, applicationInstance.Framework.RuntimeValues.PlatformDetail.EntryPointArgs.hInstance)), "Failed to create main window why?????!?!?!?!?");
	::setupParticles();

	static constexpr	const char												bmpFileName0	[]							= "ship_0.bmp";
	static constexpr	const char												bmpFileName1	[]							= "pow_core_1.bmp";
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName0), applicationInstance.TextureShip		.Original)), "Failed to load bitmap from file: %s.", bmpFileName0);
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName1), applicationInstance.TexturePowerup	.Original)), "Failed to load bitmap from file: %s.", bmpFileName1);
	char																		bmpFileName2	[]							= "crosshair_?.bmp";
	for(char iFrame = 0, frameCount = 6; iFrame < frameCount; ++iFrame) {
		bmpFileName2[10] = iFrame + '0';
		error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName2), applicationInstance.TextureCrosshair[iFrame].Original)), "Failed to load bitmap from file: %s.", bmpFileName2);
	}
	ree_if	(errored(::updateSizeDependentResources(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	applicationInstance.CenterPositionShip									= applicationInstance.Framework.Offscreen.View.metrics().Cast<float>() / 2U;
	applicationInstance.CenterPositionPowerup								= applicationInstance.Framework.Offscreen.View.metrics().Cast<float>() / 4U;
	applicationInstance.CenterPositionCrosshair								= applicationInstance.CenterPositionShip + ::cho::SCoord2<float>{64,};
	applicationInstance.TextureCenterShip									= (applicationInstance.TextureShip			.Original.View.metrics() / 2).Cast<int32_t>();
	applicationInstance.TextureCenterPowerup								= (applicationInstance.TexturePowerup		.Original.View.metrics() / 2).Cast<int32_t>();
	applicationInstance.TextureCenterCrosshair								= (applicationInstance.TextureCrosshair[0]	.Original.View.metrics() / 2).Cast<int32_t>();
	applicationInstance.PSOffsetFromShipCenter								= {-applicationInstance.TextureCenterShip.x};
	//const ::cho::SCoord2<uint32_t>												effectTargetSize							= {64, 64};
	//error_if(errored(applicationInstance.TexturePS.Original.resize(effectTargetSize)), "Something about this that shouldn't fail.");
	return 0;
}

template<typename _tParticleType>
static				::cho::error_t										addParticle														
	(	::PARTICLE_TYPE												particleType
	,	::cho::array_pod<::cho::SParticleInstance<_tParticleType>>	& particleInstances
	,	::SApplication::TParticleSystem::TIntegrator				& particleIntegrator
	,	const ::cho::SCoord2<float>									& particlePosition
	,	const ::cho::SCoord2<float>									& particleDirection
	,	float														speed
	)														
{
	::cho::SParticleInstance<_tParticleType>									& newInstance													= particleInstances[::cho::addParticle(particleType, particleInstances, particleIntegrator, particleDefinitions[particleType])]; 
	::SApplication::TParticleSystem::TIntegrator::TParticle						& newParticle													= particleIntegrator.Particle[newInstance.ParticleIndex];
	newParticle.Position													= particlePosition; 
	::cho::SCoord2<float>														newDirection													= particleDirection;
	const float value = .5;
	switch(particleType) {
	default							: break;
	case ::PARTICLE_TYPE_SHIP_THRUST:	
		newParticle.Position.y													+= rand() % 3 - 1;
		newDirection.Rotate(((rand() % 32767) / 32766.0f) * value - value / 2);
		newInstance.Lit															= 0 == (rand() % 2);
		break;
	case ::PARTICLE_TYPE_STAR		:	
		newInstance.Lit															= 0 == (rand() % 3);
		break;
	}
	newParticle.Forces.Velocity												= newDirection * speed;	//{ -, (float)((rand() % 31 * 4) - 15 * 4)};
	return 0;
}

static				::cho::error_t										updateInput									(::SApplication& applicationInstance)											{ 
	::cho::SInput																& inputSystem								= applicationInstance.Framework.SystemInput;
	applicationInstance.ShipState.Firing									= inputSystem.KeyboardCurrent.KeyState[VK_SPACE] != 0;
	applicationInstance.ShipState.Thrust									= inputSystem.KeyboardCurrent.KeyState[VK_SHIFT] != 0;

	applicationInstance.DirectionShip										= {};
	if(inputSystem.KeyboardCurrent.KeyState['W']) applicationInstance.DirectionShip.y	+= 1;
	if(inputSystem.KeyboardCurrent.KeyState['S']) applicationInstance.DirectionShip.y	-= 1;
	if(inputSystem.KeyboardCurrent.KeyState['D']) applicationInstance.DirectionShip.x	+= 1;
	if(inputSystem.KeyboardCurrent.KeyState['A']) applicationInstance.DirectionShip.x	-= 1;
	applicationInstance.DirectionShip.Normalize();
	return 0;
}

static				::cho::error_t										updateParticles								(::SApplication& applicationInstance)											{ 
	::cho::SFramework															& framework									= applicationInstance.Framework;
	typedef	::SApplication::TParticleSystem										TParticleSystem;
	typedef	TParticleSystem::TParticleInstance									TParticleInstance;
	typedef	TParticleSystem::TIntegrator::TParticle								TParticle;
	TParticleSystem::TIntegrator												& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	const float																	lastFrameSeconds							= (float)framework.FrameInfo.Seconds.LastFrame;
	ree_if(errored(particleIntegrator.Integrate(lastFrameSeconds, framework.FrameInfo.Seconds.LastFrameHalfSquared)), "Not sure why would this fail.");

	const float																	windDirection								= (float)sin(framework.FrameInfo.Seconds.Total/3.0f) * .025f;

	::cho::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystem.Instances;
	applicationInstance.StuffToDraw.ProjectilePaths.clear();
	for(uint32_t iParticle = 0; iParticle < particleInstances.size(); ++iParticle) {
		TParticleInstance															& particleInstance							= particleInstances[iParticle];
		int32_t																		physicsId									= particleInstance.ParticleIndex;
		TParticle																	& particleNext								= particleIntegrator.ParticleNext[physicsId];
		TParticle																	& particleCurrent							= particleIntegrator.Particle[physicsId];
		if(particleInstance.Type == PARTICLE_TYPE_LASER)
			applicationInstance.StuffToDraw.ProjectilePaths.push_back({particleCurrent.Position, particleNext.Position});
		if( ((uint32_t)particleNext.Position.x) >= framework.Offscreen.View.width	()
		 || ((uint32_t)particleNext.Position.y) >= framework.Offscreen.View.height	()
		 || (particleInstance.TimeLived >= .125 && particleInstance.Type == PARTICLE_TYPE_SHIP_THRUST)
		 ) { // Remove the particle instance and related information.
			particleIntegrator.ParticleState[physicsId].Unused						= true;
			ree_if(errored(particleInstances.remove(iParticle)), "Not sure why would this fail.");
			--iParticle;
		}
		else {
				 if(particleInstance.Type == PARTICLE_TYPE_STAR)		applicationInstance.StuffToDraw.Stars	.push_back({physicsId, (int32_t)iParticle, particleInstance.TimeLived, particleCurrent.Position.Cast<int32_t>(), particleInstance.Lit});
			else if(particleInstance.Type == PARTICLE_TYPE_SHIP_THRUST)	applicationInstance.StuffToDraw.Thrust	.push_back({physicsId, (int32_t)iParticle, particleInstance.TimeLived, particleCurrent.Position.Cast<int32_t>(), particleInstance.Lit});
			particleInstance.TimeLived												+= lastFrameSeconds;
			particleCurrent															= particleNext;
		}
	}
	return 0;
}

					::cho::error_t										update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	::cho::SFramework															& framework									= applicationInstance.Framework;
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");

	::cho::error_t																frameworkResult								= ::cho::updateFramework(framework);
	ree_if	(errored(frameworkResult), "Unknown error.");
	rvi_if	(1, frameworkResult == 1, "Framework requested close. Terminating execution.");

	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	error_if(errored(::updateInput					(applicationInstance)), "Unknown error.");
	error_if(errored(::updateParticles				(applicationInstance)), "Unknown error.");

	// Add some effect particles
	typedef	::SApplication::TParticleSystem										TParticleSystem;
	typedef	TParticleSystem::TParticleInstance									TParticleInstance;
	::cho::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystem.Instances;
	TParticleSystem::TIntegrator												& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	::cho::SFramework::TOffscreen												& offscreen									= applicationInstance.Framework.Offscreen;
	static double																delayThrust									= 0;
	static double																delayStar									= 0;
	static double																delayWeapon									= 0;
	delayThrust																+= framework.FrameInfo.Seconds.LastFrame;
	delayStar																+= framework.FrameInfo.Seconds.LastFrame;
	delayWeapon																+= framework.FrameInfo.Seconds.LastFrame;
	bool																		isTurbo										= applicationInstance.ShipState.Thrust;
	if(delayThrust > .01) {
		delayThrust																= 0;
		for(int32_t i = 0, particleCountToSpawn = 1 + rand() % 4; i < particleCountToSpawn; ++i) 
			::addParticle(PARTICLE_TYPE_SHIP_THRUST, particleInstances, particleIntegrator, applicationInstance.CenterPositionShip + applicationInstance.PSOffsetFromShipCenter.Cast<float>(), applicationInstance.DirectionShip * -1.0, (float)(rand() % 400) + (isTurbo ? 400 : 0));
	}
	if(delayStar > .1) {
		delayStar																= 0;
		::addParticle(PARTICLE_TYPE_STAR, particleInstances, particleIntegrator, {offscreen.View.metrics().x - 1.0f, (float)(rand() % offscreen.View.metrics().y)}, {-1, 0}, (float)(rand() % (isTurbo ? 400 : 75)) + 25);
	}

	if(applicationInstance.ShipState.Firing) {
		if(delayWeapon >= applicationInstance.Laser.Delay) {
			const ::cho::SCoord2<float>													textureShipMetrics					= applicationInstance.TextureShip.Original.View.metrics().Cast<float>();
			const ::cho::SCoord2<float>													weaponParticleOffset				= {textureShipMetrics.x - (textureShipMetrics.x - applicationInstance.TextureCenterShip.x), -1};
			delayWeapon																= 0;
			::addParticle(PARTICLE_TYPE_LASER, particleInstances, particleIntegrator, applicationInstance.CenterPositionShip + weaponParticleOffset, {1, 0}, applicationInstance.Laser.Speed);
		}
	}

	// update ship
	applicationInstance.CenterPositionShip									+= applicationInstance.DirectionShip * (float)(applicationInstance.Framework.FrameInfo.Seconds.LastFrame * 100) * (applicationInstance.ShipState.Thrust ? 2 : 1);
	applicationInstance.CenterPositionShip.x								= ::cho::clamp(applicationInstance.CenterPositionShip.x, .1f, (float)offscreen.View.metrics().x - 1);
	applicationInstance.CenterPositionShip.y								= ::cho::clamp(applicationInstance.CenterPositionShip.y, .1f, (float)offscreen.View.metrics().y - 1);

	// update background
	const float																	windDirection								= (float)(sin(framework.FrameInfo.Seconds.Total / 10.0) * .5 + .5);
	applicationInstance.ColorBackground.g									= (uint8_t)(windDirection * (applicationInstance.ColorBackground.b / 3.0));
	applicationInstance.ColorBackground.r									= (uint8_t)(windDirection * (applicationInstance.ColorBackground.b / 3.0));

	// update crosshair
	applicationInstance.CenterPositionCrosshair								= applicationInstance.CenterPositionShip + ::cho::SCoord2<float>{64,};

	::cho::STimer																& timer										= framework.Timer;
	::cho::SDisplay																& mainWindow								= framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. Particle count: %u. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, applicationInstance.ParticleSystem.Instances.size(), 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}
//
//static				::cho::error_t										raster_callback						(void* bitmapTarget, const ::cho::SCoord2<uint32_t>& bitmapMetrics, const ::cho::SCoord2<uint32_t>& cellPos, const void* value) {
//	::cho::grid_view<::cho::SColorBGRA>											target								= {(::cho::SColorBGRA*)bitmapTarget, bitmapMetrics};
//	::cho::drawPixelLight(target, cellPos.Cast<float>(), *(::cho::SColorBGRA*)value, .1f, 3.0f);
//	return 0;
//}

					::cho::error_t										draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
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

	// --- Draw powerups
	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TexturePowerup	.Original.View, applicationInstance.CenterPositionPowerup	.Cast<int32_t>() - applicationInstance.TextureCenterPowerup	, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	static double					beaconTimer			= 0;
	beaconTimer					+= framework.FrameInfo.Seconds.LastFrame * 4;
	::cho::SCoord2<int32_t>			centerPowerup	= applicationInstance.CenterPositionPowerup.Cast<int32_t>();
	int32_t							halfWidth		= (framework.FrameInfo.FrameNumber / 1000) % 6;
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

	::cho::drawPixelLight(viewOffscreen, selectedLightPos0.Cast<float>(), ::cho::SColorBGRA(::cho::RED), .3f, 3.0f);
	::cho::drawPixelLight(viewOffscreen, selectedLightPos2.Cast<float>(), ::cho::SColorBGRA(::cho::RED), .3f, 3.0f);

	// --- Draw ship
	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TextureShip		.Original.View, applicationInstance.CenterPositionShip		.Cast<int32_t>() - applicationInstance.TextureCenterShip	, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	
	// --- Draw crosshair
	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TextureCrosshair[(uint32_t)(framework.FrameInfo.Seconds.Total * 10.0f) % 5].Original.View, applicationInstance.CenterPositionCrosshair.Cast<int32_t>() - applicationInstance.TextureCenterCrosshair, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	
	// --- Draw propulsion engine
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

	for(uint32_t iRay = 0, rayCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iRay < rayCount; ++iRay) {
		//::cho::rasterLine(viewOffscreen, ::cho::SColorBGRA(::cho::RED), applicationInstance.ProjectilePaths[iRay], raster_callback);
		//::cho::drawLine(viewOffscreen, ::cho::SColorBGRA(::cho::RED), applicationInstance.ProjectilePaths[iRay]);
		applicationInstance.CacheLinePoints.clear();
		::cho::drawLine(viewOffscreen.metrics(), applicationInstance.StuffToDraw.ProjectilePaths[iRay], applicationInstance.CacheLinePoints);
		for(uint32_t iLinePoint = 0, pointCount = applicationInstance.CacheLinePoints.size(); iLinePoint < pointCount; ++iLinePoint) {
			const ::cho::SCoord2<float>								& pointToDraw									= applicationInstance.CacheLinePoints[iLinePoint].Cast<float>();
			::cho::drawPixelLight(viewOffscreen, pointToDraw, ::cho::SColorBGRA(::cho::RED), .1f, 3.0f);
		}
	}
	return 0;
}
