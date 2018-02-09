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
	//particleDefinitions	[::PARTICLE_TYPE_SNOW].Position						= 
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST].Position						= 
	//particleDefinitions	[::PARTICLE_TYPE_RAIN].Position						= 
	//particleDefinitions	[::PARTICLE_TYPE_LAVA].Position						= 
	particleDefinitions	[::PARTICLE_TYPE_STAR].Position						= {};

	//particleDefinitions	[::PARTICLE_TYPE_SNOW].SetMass						( 2);
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST].SetMass						(-1);
	//particleDefinitions	[::PARTICLE_TYPE_RAIN].SetMass						( 1);
	//particleDefinitions	[::PARTICLE_TYPE_LAVA].SetMass						( 1);
	particleDefinitions	[::PARTICLE_TYPE_STAR].SetMass						( 1);

	//particleDefinitions	[::PARTICLE_TYPE_SNOW].Damping						= 0.80f;
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST].Damping						= 0.80f;
	//particleDefinitions	[::PARTICLE_TYPE_RAIN].Damping						= 1.0f;
	//particleDefinitions	[::PARTICLE_TYPE_LAVA].Damping						= 0.99f;
	particleDefinitions	[::PARTICLE_TYPE_STAR].Damping						= 1.0f;

	//particleDefinitions	[::PARTICLE_TYPE_SNOW].Forces.Velocity				= {};
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST].Forces.Velocity				= {-(float)(rand() % 400) - 400, (float)((rand() % 50) - 20)};
	//particleDefinitions	[::PARTICLE_TYPE_RAIN].Forces.Velocity				= {};
	//particleDefinitions	[::PARTICLE_TYPE_LAVA].Forces.Velocity				= {(float)0, (float)(rand() % 100)+20};
	particleDefinitions	[::PARTICLE_TYPE_STAR].Forces.Velocity				= {-(float)(rand() % 400), };
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
	g_ApplicationInstance													= &applicationInstance;
	error_if(errored(::mainWindowCreate(applicationInstance.Framework.MainDisplay, applicationInstance.Framework.RuntimeValues.PlatformDetail.EntryPointArgs.hInstance)), "Failed to create main window why?????!?!?!?!?");
	::setupParticles();

	static constexpr	const char												bmpFileName0	[]							= "ship_0.bmp";
	static constexpr	const char												bmpFileName1	[]							= "pow_0.bmp";
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName0), applicationInstance.TextureShip		.Original)), "Failed to load bitmap from file: %s.", bmpFileName0);
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName1), applicationInstance.TexturePowerup	.Original)), "Failed to load bitmap from file: %s.", bmpFileName1);
	char																		bmpFileName2	[]							= "crosshair_?.bmp";
	for(char iFrame = 0, frameCount = 5; iFrame < frameCount; ++iFrame) {
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
	,	const ::cho::SCoord2<uint32_t>								& targetSize
	,	const ::cho::SCoord2<float>									& particlePosition
	,	bool														isTurbo
	)														
{
	::cho::SParticleInstance<_tParticleType>										newInstance														= particleInstances[::cho::addParticle(particleType, particleInstances, particleIntegrator, particleDefinitions[particleType])]; 
	particleIntegrator.Particle[newInstance.ParticleIndex].Position					= {float(rand() % targetSize.x), float(rand() % targetSize.y)};
	switch(particleType) {
	default							: return -1;
	case ::PARTICLE_TYPE_SHIP_THRUST:	
		particleIntegrator.Particle[newInstance.ParticleIndex].Position			= particlePosition; 
		particleIntegrator.Particle[newInstance.ParticleIndex].Forces.Velocity	= { -(float)(rand() % 400) - (isTurbo ? 400 : 0), (float)((rand() % 31 * 4) - 15 * 4)};
		break;
	case ::PARTICLE_TYPE_STAR		:	
		particleIntegrator.Particle[newInstance.ParticleIndex].Position.x		= (float)(rand() % targetSize.x) + targetSize.x *.5f; 
		particleIntegrator.Particle[newInstance.ParticleIndex].Forces.Velocity	= {-(float)(rand() % 400) - 25, };
		break;
	//case ::PARTICLE_TYPE_RAIN:	
	//case ::PARTICLE_TYPE_SNOW:	particleIntegrator.Particle[newInstance.ParticleIndex].Position.y	= float(targetSize.y - 2); particleIntegrator.Particle[newInstance.ParticleIndex].Forces.Velocity.y	= -.001f; break;
	//case ::PARTICLE_TYPE_LAVA:	particleIntegrator.Particle[newInstance.ParticleIndex].Position.y	= 0; break;
	}
	return particleInstances.push_back(newInstance);
}


static				::cho::error_t										updateInput									(::SApplication& applicationInstance)											{ 
	typedef	::SApplication::TParticleSystem										TParticleSystem;
	typedef	TParticleSystem::TParticleInstance									TParticleInstance;
	::cho::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystem.Instances;
	TParticleSystem::TIntegrator												& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	::cho::SFramework::TOffscreen												& offscreen									= applicationInstance.Framework.Offscreen;
	::cho::SInput																& inputSystem								= applicationInstance.Framework.SystemInput;

	::addParticle(PARTICLE_TYPE_SHIP_THRUST, particleInstances, particleIntegrator, offscreen.View.metrics(), applicationInstance.CenterPositionShip + applicationInstance.PSOffsetFromShipCenter.Cast<float>(), inputSystem.KeyboardCurrent.KeyState[VK_SHIFT] ? 1 : 0);
	if(0 == rand() % 5) 
		for(uint32_t i = 0; i < 1; ++i) 
			::addParticle(PARTICLE_TYPE_STAR, particleInstances, particleIntegrator, offscreen.View.metrics(), applicationInstance.CenterPositionShip, false);

	if(inputSystem.KeyboardCurrent.KeyState['W']) applicationInstance.CenterPositionShip.y += (float)(applicationInstance.Framework.FrameInfo.Seconds.LastFrame * 100) * (inputSystem.KeyboardCurrent.KeyState[VK_SHIFT] ? 2 : 1);
	if(inputSystem.KeyboardCurrent.KeyState['S']) applicationInstance.CenterPositionShip.y -= (float)(applicationInstance.Framework.FrameInfo.Seconds.LastFrame * 100) * (inputSystem.KeyboardCurrent.KeyState[VK_SHIFT] ? 2 : 1);
	if(inputSystem.KeyboardCurrent.KeyState['D']) applicationInstance.CenterPositionShip.x += (float)(applicationInstance.Framework.FrameInfo.Seconds.LastFrame * 100) * (inputSystem.KeyboardCurrent.KeyState[VK_SHIFT] ? 2 : 1);
	if(inputSystem.KeyboardCurrent.KeyState['A']) applicationInstance.CenterPositionShip.x -= (float)(applicationInstance.Framework.FrameInfo.Seconds.LastFrame * 100) * (inputSystem.KeyboardCurrent.KeyState[VK_SHIFT] ? 2 : 1);
	applicationInstance.CenterPositionShip.y								= ::cho::clamp(applicationInstance.CenterPositionShip.y, 0.0f, (float)offscreen.View.height());
	applicationInstance.CenterPositionShip.x								= ::cho::clamp(applicationInstance.CenterPositionShip.x, 0.0f, (float)offscreen.View.width());
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
	for(uint32_t iParticle = 0; iParticle < particleInstances.size(); ++iParticle) {
		TParticleInstance															& particleInstance							= particleInstances[iParticle];
		int32_t																		physicsId									= particleInstance.ParticleIndex;
		TParticle																	& particleNext								= particleIntegrator.ParticleNext[physicsId];
		if( ((uint32_t)particleNext.Position.x) >= framework.Offscreen.View.width	()
		 || ((uint32_t)particleNext.Position.y) >= framework.Offscreen.View.height	()
		 || (particleInstance.TimeLived >= .125 && particleInstance.Type == PARTICLE_TYPE_SHIP_THRUST)
		 ) { // Remove the particle instance and related information.
			particleIntegrator.ParticleState[physicsId].Unused						= true;
			ree_if(errored(particleInstances.remove(iParticle)), "Not sure why would this fail.");
			--iParticle;
		}
		else { // Apply forces from wind and gravity.
			particleInstance.TimeLived												+= lastFrameSeconds;
			static constexpr	const double											gravity										= 9.8;
			TParticle																	& particleCurrent							= particleIntegrator.Particle[physicsId];
			particleCurrent															= particleIntegrator.ParticleNext[physicsId];
			if(particleInstance.Type != PARTICLE_TYPE_SHIP_THRUST && particleInstance.Type != PARTICLE_TYPE_STAR) 
				particleCurrent.Forces.AccumulatedForce									= {0, float(gravity * lastFrameSeconds) * -1};
			if(particleInstance.Type != PARTICLE_TYPE_STAR)
				particleCurrent.Forces.AccumulatedForce.x								+= float(windDirection * abs(particleCurrent.Forces.Velocity.y) * .5) + float((rand() % 100) / 100.0 - .5) / 2.0f;
		}
	}
	return 0;
}
					::cho::error_t										update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	::cho::SFramework															& framework									= applicationInstance.Framework;
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");

	::cho::error_t																frameworkResult								= ::cho::updateFramework(framework);
	ree_if	(errored(frameworkResult), "Unknown error.");
	//rvi_if	(1, frameworkResult == 1, "Framework requested close. Terminating execution.");

	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	error_if(errored(::updateInput					(applicationInstance)), "Unknown error.");
	error_if(errored(::updateParticles				(applicationInstance)), "Unknown error.");
	const float																	windDirection								= (float)(sin(framework.FrameInfo.Seconds.Total / 10.0f) * .5f + .5f) * ((applicationInstance.ColorBackground.b) / 255);
	applicationInstance.ColorBackground.g									= (uint8_t)(windDirection * 255);
	applicationInstance.ColorBackground.r									= (uint8_t)(windDirection * 255);
	applicationInstance.CenterPositionCrosshair								= applicationInstance.CenterPositionShip + ::cho::SCoord2<float>{64,};

	::cho::STimer																& timer										= framework.Timer;
	::cho::SDisplay																& mainWindow								= framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. Particle count: %u. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, applicationInstance.ParticleSystem.Instances.size(), 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

					::cho::error_t										blendPixel									(::cho::grid_view<::cho::SColorBGRA> & viewOffscreen, const ::cho::SCoord2<float> & particlePosition, float factor)								{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	for(int32_t y = -1, blendCount = 2; y < blendCount; ++y)
	for(int32_t x = -1; x < blendCount; ++x) {
		::cho::SCoord2<uint32_t>													blendPos									= (particlePosition.Cast<int32_t>() + ::cho::SCoord2<int32_t>{x, y}).Cast<uint32_t>();
		if( blendPos.x < viewOffscreen.width ()
		 && blendPos.y < viewOffscreen.height()
		 ) {
			if( y != (int32_t)particlePosition.y
			 || x != (int32_t)particlePosition.x
			 )
				viewOffscreen[blendPos.y][blendPos.x]									= ::cho::interpolate_linear(viewOffscreen[blendPos.y][blendPos.x], viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x], factor);
		}
	}
	return 0;
}

					::cho::error_t										draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::STexture<::cho::SColorBGRA>											& offscreen									= framework.Offscreen;
	typedef	::SApplication::TParticleSystem										TParticleSystem;
	typedef	TParticleSystem::TParticleInstance									TParticleInstance;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= offscreen.View;
	::cho::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystem.Instances;
	//memset(offscreen.View.begin(), 0x1D, sizeof(::cho::SColorBGRA) * offscreen.View.size());
	::cho::drawRectangle(offscreen.View, applicationInstance.ColorBackground, ::cho::SRectangle2D<uint32_t>{{}, offscreen.View.metrics()});
	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TexturePowerup	.Original.View, applicationInstance.CenterPositionPowerup	.Cast<int32_t>() - applicationInstance.TextureCenterPowerup	, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TextureShip		.Original.View, applicationInstance.CenterPositionShip		.Cast<int32_t>() - applicationInstance.TextureCenterShip	, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TextureCrosshair[(uint32_t)(framework.FrameInfo.Seconds.Total * 10.0f) % 5].Original.View, applicationInstance.CenterPositionCrosshair.Cast<int32_t>() - applicationInstance.TextureCenterCrosshair, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	for(uint32_t iParticle = 0, particleCount = (uint32_t)particleInstances.size(); iParticle < particleCount; ++iParticle) {
		TParticleInstance															& particleInstance							= particleInstances[iParticle];
		const int32_t																physicsId									= particleInstance.ParticleIndex;
		const ::cho::SCoord2<float>													& particlePosition							= applicationInstance.ParticleSystem.Integrator.Particle[physicsId].Position;
		viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	
			= (particleInstance.Type == PARTICLE_TYPE_SNOW			)	? ::cho::SColorBGRA(::cho::CYAN)
			: (particleInstance.Type == PARTICLE_TYPE_SHIP_THRUST	)	? (particleInstance.TimeLived > .075)	? (framework.SystemInput.KeyboardCurrent.KeyState[VK_SHIFT] ? ::cho::SColorBGRA(::cho::DARKGRAY		) : ::cho::SColorBGRA(::cho::DARKGRAY)		)
																		: (particleInstance.TimeLived > .03 )	? (framework.SystemInput.KeyboardCurrent.KeyState[VK_SHIFT] ? ::cho::SColorBGRA(::cho::GRAY			) : ::cho::SColorBGRA(::cho::GRAY) 			)
																		: (physicsId % 3)						? (framework.SystemInput.KeyboardCurrent.KeyState[VK_SHIFT] ? ::cho::SColorBGRA(::cho::CYAN			) : ::cho::SColorBGRA(::cho::RED) 		 	)
																		: (physicsId % 2)						? (framework.SystemInput.KeyboardCurrent.KeyState[VK_SHIFT] ? ::cho::SColorBGRA(::cho::CYAN			) : ::cho::SColorBGRA(::cho::ORANGE)		)
																		: ::cho::SColorBGRA(::cho::YELLOW) 
			//: (particleInstance.Type == PARTICLE_TYPE_RAIN) ? ::cho::SColorBGRA(::cho::BLUE)
			: (particleInstance.Type == PARTICLE_TYPE_STAR) ? (0 == (physicsId % 8))					? ::cho::SColorBGRA(::cho::DARKGRAY) 
															: (0 == (physicsId % 7))					? ::cho::SColorBGRA(::cho::GRAY) 
															: (0 == (physicsId % 6))					? ::cho::SColorBGRA(::cho::WHITE)
															: (0 == (physicsId % 5))					? ::cho::SColorBGRA(::cho::YELLOW) / 2.0f
															: (0 == (physicsId % 4))					? ::cho::SColorBGRA(::cho::DARKCYAN) 
															: (0 == (physicsId % 3))					? ::cho::SColorBGRA(::cho::DARKYELLOW) / 2.0f
															: (0 == (physicsId % 2))					? ::cho::SColorBGRA(::cho::DARKRED) / 2.0f
															: ::cho::SColorBGRA(::cho::DARKBLUE) / 2.0f
			: ::cho::SColorBGRA(::cho::MAGENTA)// (PARTICLE_TYPE_LAVA == particleInstance.Type) ?
			;
		//if(rand() % 4) {
		//	if(PARTICLE_TYPE_SHIP_THRUST == particleInstance.Type) {
		//		viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	*= 1.0f - particleInstance.TimeLived;
		//		::blendPixel(viewOffscreen, particlePosition, (rand() % 3) * .15f);
		//	}
		//	else if(PARTICLE_TYPE_STAR == particleInstance.Type)
		//		::blendPixel(viewOffscreen, particlePosition, (rand() % 3) * .05f);
		//}
		if(physicsId % 4) {
			if(particleInstance.Type == PARTICLE_TYPE_STAR || particleInstance.Type == PARTICLE_TYPE_SHIP_THRUST) {
				float scale = 1.0f;
				if(PARTICLE_TYPE_SHIP_THRUST == particleInstance.Type) {
					viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	*= 1.0f - ::cho::min(1.0f, particleInstance.TimeLived);
					scale = 0.15f;
				}
				else 
					scale = 0.05f;
				::blendPixel(viewOffscreen, particlePosition, (rand() % 3) * scale);
			}
		}
	}
	return 0;
}
