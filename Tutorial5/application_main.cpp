// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "cho_bitmap_file.h"
#include "cho_bitmap_target.h"
#include "cho_grid_scale.h"
#include "cho_bit_view.h"

#include "cho_app_impl.h"
#include "cho_collision.h"

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

					::cho::error_t										mainWindowCreate							(::cho::SDisplay& mainWindow, HINSTANCE hInstance);

					::cho::error_t										setupSprites								(::SApplication& applicationInstance)											{ 

	applicationInstance.TextureCenterShip									= (applicationInstance.TextureShip		.Original.View.metrics() / 2).Cast<int32_t>();
	applicationInstance.TextureCenterPowerup								= (applicationInstance.TexturePowerup	.Original.View.metrics() / 2).Cast<int32_t>();
	applicationInstance.TextureCenterCrosshair								= (applicationInstance.TextureCrosshair	.Original.View.metrics() / 2).Cast<int32_t>();

	applicationInstance.TextureShip			.Processed.View					= applicationInstance.TextureShip		.Original.View;
	applicationInstance.TexturePowerup		.Processed.View					= applicationInstance.TexturePowerup	.Original.View;
	applicationInstance.TextureCrosshair	.Processed.View					= applicationInstance.TextureCrosshair	.Original.View;
	return 0;
}

// --- Initialize console.
					::cho::error_t										setup										(::SApplication& applicationInstance)											{ 
	//_CrtSetBreakAlloc(120);
	g_ApplicationInstance													= &applicationInstance;
	error_if(errored(::mainWindowCreate(applicationInstance.Framework.MainDisplay, applicationInstance.Framework.RuntimeValues.PlatformDetail.EntryPointArgs.hInstance)), "Failed to create main window why?????!?!?!?!?");
	::setupParticles();

	static constexpr	const char												bmpFileName0	[]							= "..\\gpk_data\\images\\ship_0.bmp";
	static constexpr	const char												bmpFileName1	[]							= "..\\gpk_data\\images\\pow_core_1.bmp";
	static constexpr	const char												bmpFileName2	[]							= "..\\gpk_data\\images\\crosshair_template.bmp";
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName0), applicationInstance.TextureShip			.Original)), "Failed to load bitmap from file: %s.", bmpFileName0);
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName1), applicationInstance.TexturePowerup		.Original)), "Failed to load bitmap from file: %s.", bmpFileName1);
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName2), applicationInstance.TextureCrosshair	.Original)), "Failed to load bitmap from file: %s.", bmpFileName1);
	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	ree_if	(errored(::setupSprites					(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	applicationInstance.PositionShip									= applicationInstance.Framework.Offscreen.View.metrics().Cast<float>() / 2U;
	applicationInstance.PositionPowerup								= applicationInstance.Framework.Offscreen.View.metrics().Cast<float>() / 4U * 3U;
	applicationInstance.PositionCrosshair								= applicationInstance.PositionShip + ::cho::SCoord2<float>{64,};
	applicationInstance.PSOffsetFromShipCenter								= {-applicationInstance.TextureCenterShip.x};
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

template<typename _tParticleType>
static				::cho::error_t										addParticle														
	(	::SGameParticle												particleType
	,	::cho::array_pod<::cho::SParticleBinding<_tParticleType>>	& particleInstances
	,	::SApplication::TParticleSystem::TIntegrator				& particleIntegrator
	,	const ::cho::SCoord2<float>									& particlePosition
	,	const ::cho::SCoord2<float>									& particleDirection
	,	float														speed
	)														
{
	::cho::SParticleBinding<_tParticleType>									& newInstance													= particleInstances[::cho::addParticle(particleType, particleInstances, particleIntegrator, particleDefinitions[particleType.Type])]; 
	::SApplication::TParticleSystem::TIntegrator::TParticle						& newParticle													= particleIntegrator.Particle[newInstance.IndexParticlePhysics];
	newParticle.Position													= particlePosition; 
	::cho::SCoord2<float>														newDirection													= particleDirection;
	const float																	value															= .5;
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

static				::cho::error_t										updateInput									(::SApplication& applicationInstance)											{ 
	::cho::SInput																& inputSystem								= applicationInstance.Framework.Input;
	applicationInstance.ShipState.Firing									= inputSystem.KeyboardCurrent.KeyState[VK_SPACE		] != 0;
	applicationInstance.ShipState.Thrust									= inputSystem.KeyboardCurrent.KeyState[VK_SHIFT		] != 0;
	applicationInstance.ShipState.Brakes									= inputSystem.KeyboardCurrent.KeyState[VK_CONTROL	] != 0;

	applicationInstance.DirectionShip										= {};
	if(inputSystem.KeyboardCurrent.KeyState['W']) applicationInstance.DirectionShip.y += 1;
	if(inputSystem.KeyboardCurrent.KeyState['S']) applicationInstance.DirectionShip.y -= 1;
	if(inputSystem.KeyboardCurrent.KeyState['D']) applicationInstance.DirectionShip.x += 1;
	if(inputSystem.KeyboardCurrent.KeyState['A']) applicationInstance.DirectionShip.x -= 1;
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

	//const float																	windDirection								= (float)sin(framework.FrameInfo.Seconds.Total/3.0f) * .025f;

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
		int32_t																		physicsId									= particleInstance.IndexParticlePhysics;
		TParticle																	& particleNext								= particleIntegrator.ParticleNext[physicsId];
		TParticle																	& particleCurrent							= particleIntegrator.Particle[physicsId];
		if(particleInstance.Binding.Type == PARTICLE_TYPE_LASER) {
			::SLaserToDraw	laserToDraw	= {physicsId, (int32_t)iParticle, ::cho::SLine2D<float>{particleCurrent.Position, particleNext.Position}};
			applicationInstance.StuffToDraw.ProjectilePaths.push_back(laserToDraw);
		}
		if( ((uint32_t)particleNext.Position.x) >= framework.Offscreen.View.width	()
		 || ((uint32_t)particleNext.Position.y) >= framework.Offscreen.View.height	()
		 || (particleInstance.Binding.TimeLived >= .125 && particleInstance.Binding.Type == PARTICLE_TYPE_SHIP_THRUST)
		 ) { // Remove the particle instance and related information.
			particleIntegrator.ParticleState[physicsId].Unused						= true;
			ree_if(errored(particleInstances.remove(iParticle)), "Not sure why would this fail.");
			--iParticle;
		}
		else {
			::SParticleToDraw particleToDraw = {physicsId, (int32_t)iParticle, particleInstance.Binding.TimeLived, particleCurrent.Position.Cast<int32_t>(), particleInstance.Binding.Lit};
				 if(particleInstance.Binding.Type == PARTICLE_TYPE_STAR		)	applicationInstance.StuffToDraw.Stars	.push_back(particleToDraw);
			else if(particleInstance.Binding.Type == PARTICLE_TYPE_SHIP_THRUST	)	applicationInstance.StuffToDraw.Thrust	.push_back(particleToDraw);
			else if(particleInstance.Binding.Type == PARTICLE_TYPE_DEBRIS		)	applicationInstance.StuffToDraw.Debris	.push_back(particleToDraw);
			particleInstance.Binding.TimeLived											+= lastFrameSeconds;
			particleCurrent															= particleNext;
		}
	}
	return 0;
}

 					::cho::error_t										updateShip									(::SApplication& applicationInstance)					{
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::SFramework::TOffscreen												& offscreen									= framework.Offscreen;
	// update ship
	applicationInstance.PositionShip										+= applicationInstance.DirectionShip * (float)(framework.FrameInfo.Seconds.LastFrame * 100) * 
		(applicationInstance.ShipState.Brakes ? .25f : (applicationInstance.ShipState.Thrust ? 2 : 1));
	applicationInstance.PositionShip.x										= ::cho::clamp(applicationInstance.PositionShip.x, .1f, (float)offscreen.View.metrics().x - 1);
	applicationInstance.PositionShip.y										= ::cho::clamp(applicationInstance.PositionShip.y, .1f, (float)offscreen.View.metrics().y - 1);
	return 0;
}

					::cho::error_t										updateSpawn									(::SApplication& applicationInstance)					{
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::SFramework::TOffscreen												& offscreen									= applicationInstance.Framework.Offscreen;
	// Add some effect particles
	typedef	::SApplication::TParticleSystem										TParticleSystem;
	typedef	TParticleSystem::TParticleInstance									TParticleInstance;
	::cho::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystem.Instances;
	TParticleSystem::TIntegrator												& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	static double																delayThrust									= 0;
	static double																delayStar									= 0;
	static double																delayWeapon									= 0;
	delayThrust																+= framework.FrameInfo.Seconds.LastFrame;
	delayStar																+= framework.FrameInfo.Seconds.LastFrame;
	delayWeapon																+= framework.FrameInfo.Seconds.LastFrame;
	bool																		isTurbo										= applicationInstance.ShipState.Thrust;
	if(delayThrust > .01) {
		delayThrust																= 0;
		for(int32_t i = 0, particleCountToSpawn = 1 + rand() % 4; i < particleCountToSpawn; ++i) {
			::SGameParticle																gameParticle;
			gameParticle.TimeLived													= 0;
			gameParticle.Type														= PARTICLE_TYPE_SHIP_THRUST;
			gameParticle.Lit														= 0 == (rand() % 2);
			::addParticle(gameParticle, particleInstances, particleIntegrator, applicationInstance.PositionShip + applicationInstance.PSOffsetFromShipCenter.Cast<float>(), applicationInstance.DirectionShip * -1.0, (float)(rand() % 400) + (isTurbo ? 400 : 0));
		}
	}
	if(delayStar > .1) {
		delayStar																= 0;
		::SGameParticle																gameParticle;
		gameParticle.TimeLived													= 0;
		gameParticle.Type														= PARTICLE_TYPE_STAR;
		gameParticle.Lit														= 0 == (rand() % 3);
		::addParticle(gameParticle, particleInstances, particleIntegrator, {offscreen.View.metrics().x - 1.0f, (float)(rand() % offscreen.View.metrics().y)}, {-1, 0}, (float)(rand() % (isTurbo ? 400 : 75)) + 25);
	}

	if(applicationInstance.ShipState.Firing) {
		if(delayWeapon >= applicationInstance.Laser.Delay) {
			const ::cho::SCoord2<float>													textureShipMetrics					= applicationInstance.TextureShip.Processed.View.metrics().Cast<float>();
			const ::cho::SCoord2<float>													weaponParticleOffset				= {textureShipMetrics.x - (textureShipMetrics.x - applicationInstance.TextureCenterShip.x), -1};
			delayWeapon																= 0;
			::SGameParticle																gameParticle;
			gameParticle.TimeLived													= 0;
			gameParticle.Type														= PARTICLE_TYPE_LASER;
			gameParticle.Lit														= true;
			::addParticle(gameParticle, particleInstances, particleIntegrator, applicationInstance.PositionShip + weaponParticleOffset, {1, 0}, applicationInstance.Laser.Speed);
		}
	}
	return 0;
}

					::cho::error_t										updateShots									(::SApplication& applicationInstance)					{ 
	applicationInstance.StuffToDraw.CollisionPoints.clear();
	for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
		const cho::SCoord2<float>						& posXHair				= applicationInstance.PositionPowerup;
		float											halfSizeBox				= (float)applicationInstance.TextureCenterPowerup.x / 4 * 3;
		::cho::SLine2D<float>							rectangleSegments[]		= 
			{ {posXHair + ::cho::SCoord2<float>{ halfSizeBox - 1, halfSizeBox - 1	}, posXHair + ::cho::SCoord2<float>{-halfSizeBox	, halfSizeBox - 1	}}
			, {posXHair + ::cho::SCoord2<float>{ halfSizeBox - 1,-halfSizeBox		}, posXHair + ::cho::SCoord2<float>{-halfSizeBox	,-halfSizeBox		}}
			, {posXHair + ::cho::SCoord2<float>{ halfSizeBox - 1, halfSizeBox - 1	}, posXHair + ::cho::SCoord2<float>{ halfSizeBox - 1,-halfSizeBox		}}
			, {posXHair + ::cho::SCoord2<float>{-halfSizeBox	, halfSizeBox - 1	}, posXHair + ::cho::SCoord2<float>{-halfSizeBox	,-halfSizeBox		}}
			};
		const ::SLaserToDraw							& laserToDraw									= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
		const ::cho::SLine2D<float>						& projectilePath								= laserToDraw.Segment;
		::cho::SCoord2<float>							collisions	[::cho::size(rectangleSegments)]	= {};
		for(uint32_t iSeg = 0; iSeg < ::cho::size(rectangleSegments); ++iSeg) {
			::cho::SCoord2<float>							& collision										= collisions[iSeg];
			const ::cho::SLine2D<float>						& segSelected									= rectangleSegments	[iSeg]; 
			if(::cho::segment_segment_intersect(projectilePath, segSelected, collision)) {
				bool											bFound											= false;
				for(uint32_t iS2 = 0; iS2 < iSeg; ++iS2) {
					if(collision == collisions[iS2]) {
						bFound										= true;
						info_printf("Discarded collision point.");
						break;
					}
				}
				if(false == bFound)
 					applicationInstance.StuffToDraw.CollisionPoints.push_back(collision);
			}
		}
	}

	typedef	::SApplication::TParticleSystem										TParticleSystem;
	typedef	TParticleSystem::TParticleInstance									TParticleInstance;
	::cho::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystem.Instances;
	TParticleSystem::TIntegrator												& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	for(uint32_t iCollision = 0, collisionCount = applicationInstance.StuffToDraw.CollisionPoints.size(); iCollision < collisionCount; ++iCollision)
		for(uint32_t i=0; i < 10; ++i) {
			::cho::SCoord2<float>	angle	= {(float)-(rand() % 20) - 10, (float)(rand() % 20 - 1 - 10)};
			angle.Normalize();
			::SGameParticle																gameParticle;
			gameParticle.TimeLived													= 0;
			gameParticle.Type														= PARTICLE_TYPE_DEBRIS;
			gameParticle.Lit														= 0 == rand() % 3;
			::addParticle(gameParticle, particleInstances, particleIntegrator, applicationInstance.StuffToDraw.CollisionPoints[iCollision], angle, (float)(rand() % 400) + 100);
		}
	return 0;
} 
					::cho::error_t										updateGUI									(::SApplication& applicationInstance)					{ 
	applicationInstance.StuffToDraw.CollisionPoints.clear();
	for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
		const cho::SCoord2<float>													& posXHair									= applicationInstance.PositionCrosshair;
		float																		halfSizeBox									= 5.0f;
		::cho::SLine2D<float>														rectangleSegments[]							= 
			{ {posXHair + ::cho::SCoord2<float>{ halfSizeBox - 1, halfSizeBox - 1}, posXHair + ::cho::SCoord2<float>{ halfSizeBox - 1	,-halfSizeBox}}
			, {posXHair + ::cho::SCoord2<float>{-halfSizeBox	, halfSizeBox - 1}, posXHair + ::cho::SCoord2<float>{-halfSizeBox		,-halfSizeBox}}
			};

		const ::SLaserToDraw														& laserToDraw								= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
		const ::cho::SLine2D<float>													& projectilePath							= laserToDraw.Segment;
		::cho::SCoord2<float>														collisions	[::cho::size(rectangleSegments)]	= {};
		for(uint32_t iSeg = 0; iSeg < ::cho::size(rectangleSegments); ++iSeg) {
			::cho::SCoord2<float>														& collision										= collisions		[iSeg];
			const ::cho::SLine2D<float>													& segSelected									= rectangleSegments	[iSeg]; 
			if(::cho::line_line_intersect(projectilePath, segSelected, collision)) {
				bool																		bFound											= false;
				for(uint32_t iS2 = 0; iS2 < iSeg; ++iS2) {
					if(collision == collisions[iS2]) {
						bFound = true;
						info_printf("Discarded collision point.");
						break;
					}
				}
				if(false == bFound)
 					applicationInstance.StuffToDraw.CollisionPoints.push_back(collision);
			}
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

	// update background
	const float																	windDirection								= (float)(sin(framework.FrameInfo.Seconds.Total / 10.0) * .5 + .5);
	applicationInstance.ColorBackground.g									= (uint8_t)(windDirection * (applicationInstance.ColorBackground.b / 3.0));
	applicationInstance.ColorBackground.r									= (uint8_t)(windDirection * (applicationInstance.ColorBackground.b / 3.0));

	error_if(errored(::updateParticles				(applicationInstance)), "Unknown error.");
	error_if(errored(::updateSpawn					(applicationInstance)), "Unknown error.");
	error_if(errored(::updateShip					(applicationInstance)), "Unknown error.");

	// update crosshair
	::cho::SFramework::TOffscreen												& offscreen									= applicationInstance.Framework.Offscreen;
	applicationInstance.PositionCrosshair									= applicationInstance.PositionShip + ::cho::SCoord2<float>{96,};
	applicationInstance.PositionCrosshair.x									= ::cho::min(applicationInstance.PositionCrosshair.x, (float)offscreen.View.metrics().x);

	error_if(errored(::updateShots					(applicationInstance)), "Unknown error.");
	error_if(errored(::updateGUI					(applicationInstance)), "Unknown error.");
	::cho::STimer																& timer										= framework.Timer;
	::cho::SDisplay																& mainWindow								= framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. Particle count: %u. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, applicationInstance.ParticleSystem.Instances.size(), 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}
