// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "cho_bitmap_file.h"
#include "cho_bitmap_target.h"
#include "cho_grid_scale.h"
#include "cho_bit_array_view.h"

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

	static constexpr	const char												bmpFileName0	[]							= "ship_2.bmp";
	static constexpr	const char												bmpFileName1	[]							= "pow_core_1.bmp";
	static constexpr	const char												bmpFileName2	[]							= "crosshair_template.bmp";
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName0), applicationInstance.TextureShip			.Original)), "Failed to load bitmap from file: %s.", bmpFileName0);
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName1), applicationInstance.TexturePowerup		.Original)), "Failed to load bitmap from file: %s.", bmpFileName1);
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName2), applicationInstance.TextureCrosshair	.Original)), "Failed to load bitmap from file: %s.", bmpFileName1);
	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	ree_if	(errored(::setupSprites					(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	applicationInstance.CenterPositionShip									= applicationInstance.Framework.Offscreen.View.metrics().Cast<float>() / 2U;
	applicationInstance.CenterPositionPowerup								= applicationInstance.Framework.Offscreen.View.metrics().Cast<float>() / 4U * 3U;
	applicationInstance.CenterPositionCrosshair								= applicationInstance.CenterPositionShip + ::cho::SCoord2<float>{64,};
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
	case ::PARTICLE_TYPE_DEBRIS		:	
	case ::PARTICLE_TYPE_STAR		:	
		newInstance.Lit															= 0 == (rand() % 3);
		break;
	}
	newParticle.Forces.Velocity												= newDirection * speed;	//{ -, (float)((rand() % 31 * 4) - 15 * 4)};
	return 0;
}

static				::cho::error_t										updateInput									(::SApplication& applicationInstance)											{ 
	::cho::SInput																& inputSystem								= applicationInstance.Framework.SystemInput;
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
			::SParticleToDraw particleToDraw = {physicsId, (int32_t)iParticle, particleInstance.TimeLived, particleCurrent.Position.Cast<int32_t>(), particleInstance.Lit};
				 if(particleInstance.Type == PARTICLE_TYPE_STAR			)	applicationInstance.StuffToDraw.Stars	.push_back(particleToDraw);
			else if(particleInstance.Type == PARTICLE_TYPE_SHIP_THRUST	)	applicationInstance.StuffToDraw.Thrust	.push_back(particleToDraw);
			else if(particleInstance.Type == PARTICLE_TYPE_DEBRIS		)	applicationInstance.StuffToDraw.Debris	.push_back(particleToDraw);
			particleInstance.TimeLived												+= lastFrameSeconds;
			particleCurrent															= particleNext;
		}
	}
	return 0;
}

template<typename _tCoord>
static inline	double		determinant					(const ::cho::SLine2D<_tCoord>& line)			noexcept { return ::cho::determinant((double)line.A.x, (double)line.A.y, (double)line.B.x, (double)line.B.y); }

// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
template<typename _tCoord>
::cho::error_t				line_line_intersect			
	( const ::cho::SLine2D<_tCoord>	& line1
	, const ::cho::SLine2D<_tCoord>	& line2
	, ::cho::SCoord2<_tCoord>		& out_point
	)
{
	double detL1 = ::determinant(line1);
	double detL2 = ::determinant(line2);

	double x1mx2 = line1.A.x - line1.B.x;
	double y1my2 = line1.A.y - line1.B.y;

	double x3mx4 = line2.A.x - line2.B.x;
	double y3my4 = line2.A.y - line2.B.y;

	double xnom  = ::cho::determinant(detL1, x1mx2, detL2, x3mx4);
	double ynom  = ::cho::determinant(detL1, y1my2, detL2, y3my4);
	double denom = ::cho::determinant(x1mx2, y1my2, x3mx4, y3my4);
	if(denom == 0.0) { // Lines don't seem to cross
		out_point.x = NAN;
		out_point.y = NAN;
		return 0;
	}
	out_point.x = (_tCoord)(xnom / denom);	
	out_point.y = (_tCoord)(ynom / denom);
	ree_if(!isfinite((_tCoord)out_point.x) 
		|| !isfinite((_tCoord)out_point.y)
		, "Probably a numerical issue");
	return 1; //All OK
}

// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
template<typename _tCoord>
::cho::error_t				line_segment_intersect			
	( const ::cho::SLine2D<_tCoord>	& line
	, const ::cho::SLine2D<_tCoord>	& segment
	, ::cho::SCoord2<_tCoord>		& out_point
	)
{
	::cho::error_t																collision									= line_line_intersect(line, segment, out_point);
	if(1 == collision) {
		collision 
			= ( out_point.x >=	::cho::min(segment.A.x, segment.B.x)
			&&	out_point.y >=	::cho::min(segment.A.y, segment.B.y)
			&&	out_point.x <=	::cho::max(segment.A.x, segment.B.x)
			&&	out_point.y <=	::cho::max(segment.A.y, segment.B.y)
			) ? 1 : 0;
	}
	return collision;
}

// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
template<typename _tCoord>
::cho::error_t				segment_segment_intersect			
	( const ::cho::SLine2D<_tCoord>	& segment1
	, const ::cho::SLine2D<_tCoord>	& segment2
	, ::cho::SCoord2<_tCoord>		& out_point
	)
{
	::cho::error_t																collision									= line_segment_intersect(segment1, segment2, out_point);
	if(1 == collision) {
		collision 
			= ( out_point.x >=	::cho::min(segment1.A.x, segment1.B.x)
			&&	out_point.y >=	::cho::min(segment1.A.y, segment1.B.y)
			&&	out_point.x <=	::cho::max(segment1.A.x, segment1.B.x)
			&&	out_point.y <=	::cho::max(segment1.A.y, segment1.B.y)
			) ? 1 : 0;
	}
	return collision;
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
			const ::cho::SCoord2<float>													textureShipMetrics					= applicationInstance.TextureShip.Processed.View.metrics().Cast<float>();
			const ::cho::SCoord2<float>													weaponParticleOffset				= {textureShipMetrics.x - (textureShipMetrics.x - applicationInstance.TextureCenterShip.x), -1};
			delayWeapon																= 0;
			::addParticle(PARTICLE_TYPE_LASER, particleInstances, particleIntegrator, applicationInstance.CenterPositionShip + weaponParticleOffset, {1, 0}, applicationInstance.Laser.Speed);
		}
	}

	// update background
	const float																	windDirection								= (float)(sin(framework.FrameInfo.Seconds.Total / 10.0) * .5 + .5);
	applicationInstance.ColorBackground.g									= (uint8_t)(windDirection * (applicationInstance.ColorBackground.b / 3.0));
	applicationInstance.ColorBackground.r									= (uint8_t)(windDirection * (applicationInstance.ColorBackground.b / 3.0));

	// update ship
	applicationInstance.CenterPositionShip									+= applicationInstance.DirectionShip * (float)(applicationInstance.Framework.FrameInfo.Seconds.LastFrame * 100) * 
		(applicationInstance.ShipState.Brakes ? .25f : (applicationInstance.ShipState.Thrust ? 2 : 1));
	applicationInstance.CenterPositionShip.x								= ::cho::clamp(applicationInstance.CenterPositionShip.x, .1f, (float)offscreen.View.metrics().x - 1);
	applicationInstance.CenterPositionShip.y								= ::cho::clamp(applicationInstance.CenterPositionShip.y, .1f, (float)offscreen.View.metrics().y - 1);

	// update crosshair
	applicationInstance.CenterPositionCrosshair								= applicationInstance.CenterPositionShip + ::cho::SCoord2<float>{96,};
	applicationInstance.CenterPositionCrosshair.x							= ::cho::min(applicationInstance.CenterPositionCrosshair.x, (float)offscreen.View.metrics().x);

	::cho::STimer																& timer										= framework.Timer;
	::cho::SDisplay																& mainWindow								= framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. Particle count: %u. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, applicationInstance.ParticleSystem.Instances.size(), 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);

	for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
		const cho::SCoord2<float>						& posXHair				= applicationInstance.CenterPositionPowerup;
		float											halfSizeBox				= (float)applicationInstance.TextureCenterPowerup.x;
		::cho::SLine2D<float>							rectangleSegments[]		= 
			{ {posXHair + ::cho::SCoord2<float>{ halfSizeBox, halfSizeBox}, posXHair + ::cho::SCoord2<float>{-halfSizeBox, halfSizeBox}}
			, {posXHair + ::cho::SCoord2<float>{ halfSizeBox, halfSizeBox}, posXHair + ::cho::SCoord2<float>{ halfSizeBox,-halfSizeBox}}
			, {posXHair + ::cho::SCoord2<float>{-halfSizeBox, halfSizeBox}, posXHair + ::cho::SCoord2<float>{-halfSizeBox,-halfSizeBox}}
			, {posXHair + ::cho::SCoord2<float>{ halfSizeBox,-halfSizeBox}, posXHair + ::cho::SCoord2<float>{-halfSizeBox,-halfSizeBox}}
			};
		const ::cho::SLine2D<float>						& projectilePath								= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
		::cho::SCoord2<float>							collisions	[::cho::size(rectangleSegments)]	= {};
		for(uint32_t iSeg = 0; iSeg < ::cho::size(rectangleSegments); ++iSeg) {
			::cho::SCoord2<float>							& collision										= collisions[iSeg];
			const ::cho::SLine2D<float>						& segSelected									= rectangleSegments	[iSeg]; 
			if(segment_segment_intersect(projectilePath, segSelected, collision)) {
				bool											bFound											= false;
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
	for(uint32_t iCollision = 0, collisionCount = applicationInstance.StuffToDraw.CollisionPoints.size(); iCollision < collisionCount; ++iCollision)
		for(uint32_t i=0; i < 10; ++i) {
			::cho::SCoord2<float>	angle	= {(float)-(rand() % 20) - 10, (float)(rand() % 20 - 1 - 10)};
			angle.Normalize();
			::addParticle(PARTICLE_TYPE_DEBRIS, particleInstances, particleIntegrator, applicationInstance.StuffToDraw.CollisionPoints[iCollision], angle, (float)(rand() % 400) + 100);
		}
	applicationInstance.StuffToDraw.CollisionPoints.clear();

	for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
		const cho::SCoord2<float>						& posXHair				= applicationInstance.CenterPositionCrosshair;
		float											halfSizeBox				= 5.0f;
		::cho::SLine2D<float>							rectangleSegment0		= {posXHair + ::cho::SCoord2<float>{ halfSizeBox, halfSizeBox}, posXHair + ::cho::SCoord2<float>{-halfSizeBox, halfSizeBox}};
		::cho::SLine2D<float>							rectangleSegment1		= {posXHair + ::cho::SCoord2<float>{ halfSizeBox, halfSizeBox}, posXHair + ::cho::SCoord2<float>{ halfSizeBox,-halfSizeBox}};
		::cho::SLine2D<float>							rectangleSegment2		= {posXHair + ::cho::SCoord2<float>{-halfSizeBox, halfSizeBox}, posXHair + ::cho::SCoord2<float>{-halfSizeBox,-halfSizeBox}};
		::cho::SLine2D<float>							rectangleSegment3		= {posXHair + ::cho::SCoord2<float>{ halfSizeBox,-halfSizeBox}, posXHair + ::cho::SCoord2<float>{-halfSizeBox,-halfSizeBox}};
		::cho::SCoord2<float>							collision0				= {};
		::cho::SCoord2<float>							collision1				= {};
		::cho::SCoord2<float>							collision2				= {};
		::cho::SCoord2<float>							collision3				= {};
		::cho::SLine2D<float>							projectilePath			= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
		if(line_line_intersect(projectilePath, rectangleSegment0, collision0)) applicationInstance.StuffToDraw.CollisionPoints.push_back(collision0);
		if(line_line_intersect(projectilePath, rectangleSegment1, collision1)) applicationInstance.StuffToDraw.CollisionPoints.push_back(collision1);
		if(line_line_intersect(projectilePath, rectangleSegment2, collision2)) applicationInstance.StuffToDraw.CollisionPoints.push_back(collision2);
		if(line_line_intersect(projectilePath, rectangleSegment3, collision3)) applicationInstance.StuffToDraw.CollisionPoints.push_back(collision3);
	}
	return 0;
}
