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
	particleDefinitions	[::PARTICLE_TYPE_SNOW].Position						= 
	particleDefinitions	[::PARTICLE_TYPE_FIRE].Position						= 
	particleDefinitions	[::PARTICLE_TYPE_RAIN].Position						= 
	particleDefinitions	[::PARTICLE_TYPE_LAVA].Position						= 
	particleDefinitions	[::PARTICLE_TYPE_STAR].Position						= {};

	particleDefinitions	[::PARTICLE_TYPE_SNOW].SetMass						( 2);
	particleDefinitions	[::PARTICLE_TYPE_FIRE].SetMass						(-1);
	particleDefinitions	[::PARTICLE_TYPE_RAIN].SetMass						( 1);
	particleDefinitions	[::PARTICLE_TYPE_LAVA].SetMass						( 1);
	particleDefinitions	[::PARTICLE_TYPE_STAR].SetMass						( 1);

	particleDefinitions	[::PARTICLE_TYPE_SNOW].Damping						= 0.80f;
	particleDefinitions	[::PARTICLE_TYPE_FIRE].Damping						= 0.80f;
	particleDefinitions	[::PARTICLE_TYPE_RAIN].Damping						= 1.0f;
	particleDefinitions	[::PARTICLE_TYPE_LAVA].Damping						= 0.99f;
	particleDefinitions	[::PARTICLE_TYPE_STAR].Damping						= 1.0f;

	particleDefinitions	[::PARTICLE_TYPE_SNOW].Forces.Velocity				= {};
	particleDefinitions	[::PARTICLE_TYPE_FIRE].Forces.Velocity				= {-(float)(rand() % 400), (float)((rand() % 50) - 20)};
	particleDefinitions	[::PARTICLE_TYPE_RAIN].Forces.Velocity				= {};
	particleDefinitions	[::PARTICLE_TYPE_LAVA].Forces.Velocity				= {(float)0, (float)(rand() % 100)+20};
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
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName0), applicationInstance.TextureShip.Original)), "Failed to load bitmap from file: %s.", bmpFileName0);
	ree_if	(errored(::updateSizeDependentResources(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	const ::cho::SCoord2<uint32_t>												effectTargetSize							= {64, 64};
	error_if(errored(applicationInstance.TexturePS.Original.resize(effectTargetSize)), "Something about this that shouldn't fail.");
	applicationInstance.ShipCenterPosition									= applicationInstance.Framework.Offscreen.View.metrics().Cast<float>() / 2U;
	applicationInstance.ShipTextureCenter									= (applicationInstance.TextureShip.Original.View.metrics() / 2).Cast<int32_t>();
	applicationInstance.PSOffsetFromShipCenter								= {-applicationInstance.ShipTextureCenter.x};
	return 0;
}

template<typename _tParticleType>
static				::cho::error_t										addParticle														
	(	::PARTICLE_TYPE												particleType
	,	::cho::array_pod<::cho::SParticleInstance<_tParticleType>>	& particleInstances
	,	::SApplication::TParticleSystem::TIntegrator				& particleIntegrator
	,	const ::cho::SCoord2<uint32_t>								& targetSize
	,	const ::cho::SCoord2<float>								& particlePosition
	)														
{
	::cho::SParticleInstance<_tParticleType>										newInstance														= particleInstances[::cho::addParticle(particleType, particleInstances, particleIntegrator, particleDefinitions[particleType])]; 
	particleIntegrator.Particle[newInstance.ParticleIndex].Position					= {float(rand() % targetSize.x), float(rand() % targetSize.y)};
	switch(particleType) {
	case ::PARTICLE_TYPE_FIRE:	
		particleIntegrator.Particle[newInstance.ParticleIndex].Position			= particlePosition; 
		particleIntegrator.Particle[newInstance.ParticleIndex].Forces.Velocity	= {-(float)(rand() % 400), (float)((rand() % 31 * 4) - 15 * 4)};
		break;
	case ::PARTICLE_TYPE_STAR:	
		particleIntegrator.Particle[newInstance.ParticleIndex].Position.x		= (float)(rand() % targetSize.x) + targetSize.x *.5f; 
		particleIntegrator.Particle[newInstance.ParticleIndex].Forces.Velocity	= {-(float)(rand() % 400), };
		break;
	case ::PARTICLE_TYPE_RAIN:	
	case ::PARTICLE_TYPE_SNOW:	particleIntegrator.Particle[newInstance.ParticleIndex].Position.y	= float(targetSize.y - 2); particleIntegrator.Particle[newInstance.ParticleIndex].Forces.Velocity.y	= -.001f; break;
	case ::PARTICLE_TYPE_LAVA:	particleIntegrator.Particle[newInstance.ParticleIndex].Position.y	= 0; break;
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
	///*if(inputSystem.KeyboardCurrent.KeyState['1']) */for(uint32_t i = 0; i < 3; ++i) ::addParticle(PARTICLE_TYPE_SNOW, particleInstances, particleIntegrator, offscreen.View.metrics(), applicationInstance.ShipCenterPosition);
	for(uint32_t i = 0; i < 3; ++i) 
		::addParticle(PARTICLE_TYPE_FIRE, particleInstances, particleIntegrator, offscreen.View.metrics(), applicationInstance.ShipCenterPosition + applicationInstance.PSOffsetFromShipCenter.Cast<float>());

	if(0 == rand() % 5) 
		for(uint32_t i = 0; i < 1; ++i) 
			::addParticle(PARTICLE_TYPE_STAR, particleInstances, particleIntegrator, offscreen.View.metrics(), applicationInstance.ShipCenterPosition);

	if(inputSystem.KeyboardCurrent.KeyState['W']) applicationInstance.ShipCenterPosition.y += (float)(applicationInstance.Framework.FrameInfo.Seconds.LastFrame * 100) * (inputSystem.KeyboardCurrent.KeyState[VK_SHIFT] ? 2 : 1);
	if(inputSystem.KeyboardCurrent.KeyState['S']) applicationInstance.ShipCenterPosition.y -= (float)(applicationInstance.Framework.FrameInfo.Seconds.LastFrame * 100) * (inputSystem.KeyboardCurrent.KeyState[VK_SHIFT] ? 2 : 1);
	if(inputSystem.KeyboardCurrent.KeyState['D']) applicationInstance.ShipCenterPosition.x += (float)(applicationInstance.Framework.FrameInfo.Seconds.LastFrame * 100) * (inputSystem.KeyboardCurrent.KeyState[VK_SHIFT] ? 2 : 1);
	if(inputSystem.KeyboardCurrent.KeyState['A']) applicationInstance.ShipCenterPosition.x -= (float)(applicationInstance.Framework.FrameInfo.Seconds.LastFrame * 100) * (inputSystem.KeyboardCurrent.KeyState[VK_SHIFT] ? 2 : 1);
	applicationInstance.ShipCenterPosition.y								= ::cho::clamp(applicationInstance.ShipCenterPosition.y, 0.0f, (float)offscreen.View.height());
	applicationInstance.ShipCenterPosition.x								= ::cho::clamp(applicationInstance.ShipCenterPosition.x, 0.0f, (float)offscreen.View.width());
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
		if( !::cho::in_range((uint32_t)particleNext.Position.x, 0U, framework.Offscreen.View.width	())
		 || !::cho::in_range((uint32_t)particleNext.Position.y, 0U, framework.Offscreen.View.height	())
		 || (particleInstance.TimeLived >= .125 && particleInstance.Type == PARTICLE_TYPE_FIRE)
		 ) { // Remove the particle instance and related information.
			particleIntegrator.ParticleState[physicsId].Unused							= true;
			ree_if(errored(particleInstances.remove(iParticle)), "Not sure why would this fail.");
			--iParticle;
		}
		else { // Apply forces from wind and gravity.
			particleInstance.TimeLived												+= lastFrameSeconds;
			static constexpr	const double											gravity										= 9.8;
			TParticle																	& particleCurrent							= particleIntegrator.Particle[physicsId];
			particleCurrent															= particleIntegrator.ParticleNext[physicsId];
			if(particleInstance.Type != PARTICLE_TYPE_FIRE && particleInstance.Type != PARTICLE_TYPE_STAR) 
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
	rvi_if	(1, frameworkResult == 1, "Framework requested close. Terminating execution.");

	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	error_if(errored(::updateInput					(applicationInstance)), "Unknown error.");
	error_if(errored(::updateParticles				(applicationInstance)), "Unknown error.");

	::cho::STimer																& timer										= framework.Timer;
	::cho::SDisplay																& mainWindow								= framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. Particle count: %u. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, applicationInstance.ParticleSystem.Instances.size(), 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

					::cho::error_t										draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::STexture<::cho::SColorBGRA>											& offscreen									= framework.Offscreen;
	typedef	::SApplication::TParticleSystem										TParticleSystem;
	typedef	TParticleSystem::TParticleInstance									TParticleInstance;
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= offscreen.View;
	::cho::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystem.Instances;
	memset(offscreen.View.begin(), 0x2D, sizeof(::cho::SColorBGRA) * offscreen.View.size());
	error_if(errored(::cho::grid_copy_alpha(offscreen.View, applicationInstance.TextureShip.Original.View, applicationInstance.ShipCenterPosition.Cast<int32_t>() - applicationInstance.ShipTextureCenter, {0xFF, 0, 0xFF, 0xFF})), "I believe this never fails.");
	for(uint32_t iParticle = 0, particleCount = (uint32_t)particleInstances.size(); iParticle < particleCount; ++iParticle) {
		TParticleInstance															& particleInstance							= particleInstances[iParticle];
		const int32_t																physicsId									= particleInstance.ParticleIndex;
		const ::cho::SCoord2<float>													& particlePosition							= applicationInstance.ParticleSystem.Integrator.Particle[physicsId].Position;
		viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	
			= (PARTICLE_TYPE_SNOW == particleInstance.Type) ? ::cho::SColorBGRA(::cho::CYAN)
			: (PARTICLE_TYPE_FIRE == particleInstance.Type) ? (particleInstance.TimeLived > .05)	? ::cho::SColorBGRA(::cho::GRAY) 
															: (rand() % 2)							? ::cho::SColorBGRA(::cho::RED) 
															: (rand() % 2)							? ::cho::SColorBGRA(::cho::ORANGE) 
															: ::cho::SColorBGRA(::cho::YELLOW) 
			: (PARTICLE_TYPE_RAIN == particleInstance.Type) ? ::cho::SColorBGRA(::cho::BLUE)
			: (PARTICLE_TYPE_STAR == particleInstance.Type) ? (0 == (rand() % 3))					? ::cho::SColorBGRA(::cho::GRAY) 
															: (0 == (rand() % 2))					? ::cho::SColorBGRA(::cho::DARKGRAY) 
															: ::cho::SColorBGRA(::cho::WHITE) 
			: ::cho::SColorBGRA(::cho::GREEN)// (PARTICLE_TYPE_LAVA == particleInstance.Type) ?
			;
		if(PARTICLE_TYPE_FIRE == particleInstance.Type) {
			viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	*= 1.0f - particleInstance.TimeLived;
			for(int32_t y = -1, blendCount = 2; y < blendCount; ++y)
			for(int32_t x = -1; x < blendCount; ++x) {
				::cho::SCoord2<uint32_t>													blendPos									= (particlePosition.Cast<int32_t>() + ::cho::SCoord2<int32_t>{x, y}).Cast<uint32_t>();
				if( blendPos.x < viewOffscreen.width()
				 && blendPos.y < viewOffscreen.height()
				 ) {
					if( y == (int32_t)particlePosition.y
					 && x == (int32_t)particlePosition.x
					 )
						continue;
					viewOffscreen[blendPos.y][blendPos.x]									= ::cho::interpolate_linear(viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x], viewOffscreen[blendPos.y][blendPos.x], 0.75 + (rand() % 6) * .05f); // 0.95 + (rand() % 4) * .025f);
				}
			}
		}
		if(PARTICLE_TYPE_STAR == particleInstance.Type) {
			for(int32_t y = -1, blendCount = 2; y < blendCount; ++y)
			for(int32_t x = -1; x < blendCount; ++x) {
				::cho::SCoord2<uint32_t>													blendPos									= (particlePosition.Cast<int32_t>() + ::cho::SCoord2<int32_t>{x, y}).Cast<uint32_t>();
				if( blendPos.x < viewOffscreen.width()
				 && blendPos.y < viewOffscreen.height()
				 ) {
					if( y == (int32_t)particlePosition.y
					 && x == (int32_t)particlePosition.x
					 )
						continue;
					viewOffscreen[blendPos.y][blendPos.x]									= ::cho::interpolate_linear(viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x], viewOffscreen[blendPos.y][blendPos.x], 0.90 + (rand() % 3) * .05f); // 0.95 + (rand() % 4) * .025f);
				}
			}
		}
	}
	return 0;
}
