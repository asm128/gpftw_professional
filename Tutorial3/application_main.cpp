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

static ::cho::SParticle2<float>											particleDefinitions	[::PARTICLE_TYPE_COUNT]				= {};

void																	setupParticles													()														{
	particleDefinitions	[::PARTICLE_TYPE_SNOW].Position						= 
	particleDefinitions	[::PARTICLE_TYPE_FIRE].Position						= 
	particleDefinitions	[::PARTICLE_TYPE_RAIN].Position						= 
	particleDefinitions	[::PARTICLE_TYPE_LAVA].Position						= {};

	particleDefinitions	[::PARTICLE_TYPE_SNOW].SetMass						( 2);
	particleDefinitions	[::PARTICLE_TYPE_FIRE].SetMass						(-1);
	particleDefinitions	[::PARTICLE_TYPE_RAIN].SetMass						( 1);
	particleDefinitions	[::PARTICLE_TYPE_LAVA].SetMass						( 1);

	particleDefinitions	[::PARTICLE_TYPE_SNOW].Damping						= 0.80f;
	particleDefinitions	[::PARTICLE_TYPE_FIRE].Damping						= 0.80f;
	particleDefinitions	[::PARTICLE_TYPE_RAIN].Damping						= 1.0f;
	particleDefinitions	[::PARTICLE_TYPE_LAVA].Damping						= 0.99f;

	particleDefinitions	[::PARTICLE_TYPE_SNOW].Forces.Velocity				= {};
	particleDefinitions	[::PARTICLE_TYPE_FIRE].Forces.Velocity				= {(float)0, (float)(rand() % 40)};
	particleDefinitions	[::PARTICLE_TYPE_RAIN].Forces.Velocity				= {};
	particleDefinitions	[::PARTICLE_TYPE_LAVA].Forces.Velocity				= {(float)0, (float)(rand() % 100)+20};
}


					::SApplication										* g_ApplicationInstance						= 0;
					
static				::cho::error_t										updateSizeDependentResources				(::SApplication& applicationInstance)											{ 
	const ::cho::SCoord2<uint32_t>												newSize										= applicationInstance.Framework.MainDisplay.Size;// / 2;
	::cho::updateSizeDependentTarget(applicationInstance.Framework.Offscreen, newSize);
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
	return 0;
}

template<typename _tParticleType>
void																	addParticle														
	(	_tParticleType												particleType
	,	::cho::array_pod<::cho::SParticleInstance<_tParticleType>>	& particleInstances
	,	::cho::SParticle2Integrator<float>							& particleEngine
	,	const ::cho::SCoord2<uint32_t>								& targetSize
	)														
{
	::cho::SParticleInstance<_tParticleType>										newInstance														= particleInstances[::cho::addParticle(particleType, particleInstances, particleEngine, particleDefinitions[particleType])]; 
	particleEngine.Particle[newInstance.ParticleIndex].Position					= {float(rand() % targetSize.x), float(rand() % targetSize.y)};
	switch(particleType) {
	case ::PARTICLE_TYPE_FIRE:	particleEngine.Particle[newInstance.ParticleIndex].Position		= {float(targetSize.x / 2 + (rand() % 3 - 1.0f) * (targetSize.x / 5)), float(targetSize.y / 2 + targetSize.y / 4)}; break;
	case ::PARTICLE_TYPE_LAVA:	particleEngine.Particle[newInstance.ParticleIndex].Position.y	= 0; break;
	case ::PARTICLE_TYPE_RAIN:
	case ::PARTICLE_TYPE_SNOW:	particleEngine.Particle[newInstance.ParticleIndex].Position.y	= float(targetSize.y - 2); particleEngine.Particle[newInstance.ParticleIndex].Forces.Velocity.y	= -.001f; break;
	}
	particleInstances.push_back(newInstance);
}

					::cho::error_t										updateInput									(::SApplication& applicationInstance)											{ 
	::cho::SInput																& systemInput								= applicationInstance.Framework.SystemInput;
	if(systemInput.KeyUp	('W')) ::Beep(440, 100);
	if(systemInput.KeyUp	('A')) ::Beep(520, 100);
	if(systemInput.KeyUp	('S')) ::Beep(600, 100);
	if(systemInput.KeyUp	('D')) ::Beep(680, 100);
	if(systemInput.KeyDown	('W')) ::Beep(760, 100);
	if(systemInput.KeyDown	('A')) ::Beep(840, 100);
	if(systemInput.KeyDown	('S')) ::Beep(920, 100);
	if(systemInput.KeyDown	('D')) ::Beep(1000, 100);

	systemInput.KeyboardPrevious											= systemInput.KeyboardCurrent;
	systemInput.MousePrevious												= systemInput.MouseCurrent;
	::SApplication::TParticleSystem												& particleSystem							= applicationInstance.ParticleSystem;
	::cho::SFramework::TOffscreen												& offscreen									= applicationInstance.Framework.Offscreen;
	if(::GetAsyncKeyState('1')) for(uint32_t i = 0; i < 3; ++i) ::addParticle(PARTICLE_TYPE_SNOW, particleSystem.Instances, particleSystem.Integrator, { offscreen.View.width(), offscreen.View.height() });
	if(::GetAsyncKeyState('2')) for(uint32_t i = 0; i < 3; ++i) ::addParticle(PARTICLE_TYPE_FIRE, particleSystem.Instances, particleSystem.Integrator, { offscreen.View.width(), offscreen.View.height() });
	if(::GetAsyncKeyState('3')) for(uint32_t i = 0; i < 3; ++i) ::addParticle(PARTICLE_TYPE_RAIN, particleSystem.Instances, particleSystem.Integrator, { offscreen.View.width(), offscreen.View.height() });
	if(::GetAsyncKeyState('4')) for(uint32_t i = 0; i < 3; ++i) ::addParticle(PARTICLE_TYPE_LAVA, particleSystem.Instances, particleSystem.Integrator, { offscreen.View.width(), offscreen.View.height() });
	return 0;
}

					::cho::error_t										updateParticles								(::SApplication& applicationInstance)					{ 
	::cho::SFrameInfo															& frameInfo									= applicationInstance.Framework.FrameInfo;
	static float																windDirection								= 0.1f;
	SApplication::TParticleSystem::TIntegrator									& particleEngine							= applicationInstance.ParticleSystem.Integrator;
	const float																	lastFrameSeconds							= (float)frameInfo.Seconds.LastFrame;
	particleEngine.Integrate(lastFrameSeconds, frameInfo.Seconds.LastFrameHalfSquared);

	windDirection															= (float)sin(frameInfo.Seconds.Total/5.0f) * .25f;

	::cho::SFramework::TOffscreen												& offscreen									= applicationInstance.Framework.Offscreen;
	::cho::array_pod<SApplication::TParticleSystem::TParticleInstance>			& particleInstances							= applicationInstance.ParticleSystem.Instances;
	for(uint32_t iParticle = 0; iParticle < particleInstances.size(); ++iParticle) {
		SApplication::TParticleSystem::TParticleInstance							& particleInstance							= particleInstances[iParticle];
		int32_t																		physicsId									= particleInstance.ParticleIndex;
		::cho::SParticle2<float>													& particleNext								= particleEngine.ParticleNext[physicsId];
		if( particleNext.Position.x < 0 || particleNext.Position.x >= offscreen.View.width	()
		 || particleNext.Position.y < 0 || particleNext.Position.y >= offscreen.View.height	()
		 ) { // Remove the particle instance and related information.
			particleEngine.ParticleState[physicsId].Unused								= true;
			particleInstances.erase(particleInstances.begin()+iParticle);
			--iParticle;
		}
		else { // Apply forces from wind and gravity.
			static constexpr	const double												gravity										= 9.8;
			::cho::SParticle2<float>														& particleCurrent							= particleEngine.Particle[physicsId];
			particleCurrent																= particleEngine.ParticleNext[physicsId];
			particleCurrent.Forces.AccumulatedForce										= {0, float(gravity * lastFrameSeconds) * -1};
			particleCurrent.Forces.AccumulatedForce.x									+= float(windDirection * abs(particleCurrent.Forces.Velocity.y) * .5) + float((rand() % 100) / 100.0 - .5) / 2.0f;
		}
	}
	return 0;
}

					::cho::error_t										update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");

	uint32_t																	frameworkResult								= ::cho::updateFramework(applicationInstance.Framework);
	ree_if	(errored(frameworkResult), "Unknown error.");
	rvi_if	(1, frameworkResult == 1, "Framework requested close. Terminating execution.");

	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	error_if(errored(::updateInput					(applicationInstance)), "Unknown error.");
	error_if(errored(::updateParticles				(applicationInstance)), "Unknown error.");

	::cho::STimer																& timer										= applicationInstance.Framework.Timer;
	::cho::SDisplay																& mainWindow								= applicationInstance.Framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. Particle count: %u. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, applicationInstance.ParticleSystem.Instances.size(), 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

					::cho::error_t										draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= applicationInstance.Framework.Offscreen.View;
	::cho::array_pod<::SApplication::TParticleSystem::TParticleInstance>		& particleInstances							= applicationInstance.ParticleSystem.Instances;
	for(uint32_t iParticle = 0, particleCount = (uint32_t)particleInstances.size(); iParticle < particleCount; ++iParticle) {
		::cho::SParticleInstance<PARTICLE_TYPE>										& particleInstance							= particleInstances[iParticle];
		const int32_t																physicsId									= particleInstance.ParticleIndex;
		const ::cho::SCoord2<float>													particlePosition							= applicationInstance.ParticleSystem.Integrator.Particle[physicsId].Position;
		viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	
			= (PARTICLE_TYPE_SNOW == particleInstance.Type) ? ::cho::SColorBGRA(::cho::CYAN)
			: (PARTICLE_TYPE_FIRE == particleInstance.Type) ? ::cho::SColorBGRA(::cho::RED )
			: (PARTICLE_TYPE_RAIN == particleInstance.Type) ? ::cho::SColorBGRA(::cho::BLUE)
			: ::cho::SColorBGRA(::cho::ORANGE)// (PARTICLE_TYPE_LAVA == particleInstance.Type) ?
			;
	}
	return 0;
}
