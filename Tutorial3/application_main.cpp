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

void																		setupParticles													()														{
	particleDefinitions	[::PARTICLE_TYPE_SNOW].Position					= 
	particleDefinitions	[::PARTICLE_TYPE_FIRE].Position					= 
	particleDefinitions	[::PARTICLE_TYPE_RAIN].Position					= 
	particleDefinitions	[::PARTICLE_TYPE_LAVA].Position					= {};

	particleDefinitions	[::PARTICLE_TYPE_SNOW].SetMass					( 2);
	particleDefinitions	[::PARTICLE_TYPE_FIRE].SetMass					(-1);
	particleDefinitions	[::PARTICLE_TYPE_RAIN].SetMass					( 1);
	particleDefinitions	[::PARTICLE_TYPE_LAVA].SetMass					( 1);

	particleDefinitions	[::PARTICLE_TYPE_SNOW].Damping					= 0.80f;
	particleDefinitions	[::PARTICLE_TYPE_FIRE].Damping					= 0.80f;
	particleDefinitions	[::PARTICLE_TYPE_RAIN].Damping					= 1.0f;
	particleDefinitions	[::PARTICLE_TYPE_LAVA].Damping					= 0.99f;

	particleDefinitions	[::PARTICLE_TYPE_SNOW].Forces.Velocity			= {};
	particleDefinitions	[::PARTICLE_TYPE_FIRE].Forces.Velocity			= {(float)0, (float)(rand() % 40)};
	particleDefinitions	[::PARTICLE_TYPE_RAIN].Forces.Velocity			= {};
	particleDefinitions	[::PARTICLE_TYPE_LAVA].Forces.Velocity			= {(float)0, (float)(rand() % 100)+20};
}


					::SApplication										* g_ApplicationInstance						= 0;
					
static				::cho::error_t										updateSizeDependentResources				(::SApplication& applicationInstance)											{ 
	const ::cho::SCoord2<uint32_t>												newSize										= applicationInstance.MainDisplay.Size;// / 2;
	//const ::cho::SCoord2<uint32_t>												newSize										= applicationInstance.MainDisplay.Size + applicationInstance.MainDisplay.Size ;
	::cho::updateSizeDependentTarget	(applicationInstance.OffscreenBitmap					, applicationInstance.ViewOffscreenBitmap					, newSize);
	//::cho::updateSizeDependentTexture	(applicationInstance.TextureBackgroundScaledDay			, applicationInstance.ViewTextureBackgroundScaledDay		, applicationInstance.ViewTextureBackgroundDay		, newSize);
	//::cho::updateSizeDependentTexture	(applicationInstance.TextureBackgroundScaledNight		, applicationInstance.ViewTextureBackgroundScaledNight		, applicationInstance.ViewTextureBackgroundNight	, newSize);
	return 0;
}

// --- Cleanup application resources.
					::cho::error_t										cleanup										(::SApplication& applicationInstance)											{
	::cho::SDisplayPlatformDetail												& displayDetail								= applicationInstance.MainDisplay.PlatformDetail;
	if(displayDetail.WindowHandle) {
		error_if(0 == ::DestroyWindow(displayDetail.WindowHandle), "Not sure why would this fail.");
		error_if(errored(::cho::displayUpdate(applicationInstance.MainDisplay)), "Not sure why this would fail");
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
	error_if(errored(::mainWindowCreate			(applicationInstance.MainDisplay, applicationInstance.RuntimeValues.PlatformDetail.EntryPointArgs.hInstance)), "Failed to create main window why?????!?!?!?!?");
	//static constexpr	const char												bmpFileName0	[]									= "earth_color.bmp";
	//static constexpr	const char												bmpFileName1	[]									= "earth_light.bmp";
	//error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName0), applicationInstance.TextureBackgroundDay	, applicationInstance.ViewTextureBackgroundDay	)), "Failed to load bitmap from file: %s.", bmpFileName0);
	//error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName1), applicationInstance.TextureBackgroundNight	, applicationInstance.ViewTextureBackgroundNight)), "Failed to load bitmap from file: %s.", bmpFileName1);
	::setupParticles();
	return 0;
}

void																		addParticle														
	(	::PARTICLE_TYPE							particleType
	,	::cho::array_pod<::SParticleInstance>	& particleInstances
	,	::cho::SParticle2Engine<float>			& particleEngine
	,	const ::cho::SCoord2<uint32_t>			& targetSize
	)														
{
	::SParticleInstance																newInstance														= {}; 
	newInstance.Type															= particleType; 
	newInstance.ParticleIndex													= particleEngine.AddParticle(particleDefinitions[newInstance.Type]); 
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
	if(applicationInstance.SystemInput.KeyUp	('W')) ::Beep(440, 100);
	if(applicationInstance.SystemInput.KeyUp	('A')) ::Beep(520, 100);
	if(applicationInstance.SystemInput.KeyUp	('S')) ::Beep(600, 100);
	if(applicationInstance.SystemInput.KeyUp	('D')) ::Beep(680, 100);
	if(applicationInstance.SystemInput.KeyDown	('W')) ::Beep(760, 100);
	if(applicationInstance.SystemInput.KeyDown	('A')) ::Beep(840, 100);
	if(applicationInstance.SystemInput.KeyDown	('S')) ::Beep(920, 100);
	if(applicationInstance.SystemInput.KeyDown	('D')) ::Beep(1000, 100);

	applicationInstance.SystemInput.KeyboardPrevious						= applicationInstance.SystemInput.KeyboardCurrent;
	applicationInstance.SystemInput.MousePrevious							= applicationInstance.SystemInput.MouseCurrent;
	::cho::array_pod<SParticleInstance>												& particleInstances												= applicationInstance.ParticleInstances;
	::cho::SParticle2Engine<float>													& particleEngine												= applicationInstance.ParticleEngine;
	if(::GetAsyncKeyState('1')) for(uint32_t i = 0; i < 3; ++i) ::addParticle(PARTICLE_TYPE_SNOW, particleInstances, particleEngine, { applicationInstance.ViewOffscreenBitmap.width(), applicationInstance.ViewOffscreenBitmap.height() });
	if(::GetAsyncKeyState('2')) for(uint32_t i = 0; i < 3; ++i) ::addParticle(PARTICLE_TYPE_FIRE, particleInstances, particleEngine, { applicationInstance.ViewOffscreenBitmap.width(), applicationInstance.ViewOffscreenBitmap.height() });
	if(::GetAsyncKeyState('3')) for(uint32_t i = 0; i < 3; ++i) ::addParticle(PARTICLE_TYPE_RAIN, particleInstances, particleEngine, { applicationInstance.ViewOffscreenBitmap.width(), applicationInstance.ViewOffscreenBitmap.height() });
	if(::GetAsyncKeyState('4')) for(uint32_t i = 0; i < 3; ++i) ::addParticle(PARTICLE_TYPE_LAVA, particleInstances, particleEngine, { applicationInstance.ViewOffscreenBitmap.width(), applicationInstance.ViewOffscreenBitmap.height() });


	return 0;

}

					::cho::error_t										update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	applicationInstance.Timer		.Frame();
	applicationInstance.FrameInfo	.Frame(applicationInstance.Timer.LastTimeMicroseconds);
	::cho::SDisplay																& mainWindow								= applicationInstance.MainDisplay;
	ree_if(errored(::cho::displayUpdate(mainWindow)), "Not sure why this would fail.");
	ree_if(errored(::updateSizeDependentResources(applicationInstance)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	rvi_if(1, 0 == mainWindow.PlatformDetail.WindowHandle, "Application exiting because the main window was closed.");
	error_if(errored(::cho::displayPresentTarget(mainWindow, applicationInstance.ViewOffscreenBitmap)), "Unknown error.");

	::updateInput(applicationInstance);
	::cho::updateGUI(applicationInstance.GUI, applicationInstance.SystemInput);

	static float																	windDirection													= 0.1f;
	::cho::SParticle2Engine<float>													& particleEngine												= applicationInstance.ParticleEngine;
	const float																		lastFrameSeconds												= (float)applicationInstance.FrameInfo.Seconds.LastFrame;
	particleEngine.Integrate(lastFrameSeconds, applicationInstance.FrameInfo.Seconds.LastFrameHalfSquared);

	windDirection																= (float)sin(applicationInstance.FrameInfo.Seconds.Total/5.0f) * .25f;

	::cho::array_pod<SParticleInstance>												& particleInstances												= applicationInstance.ParticleInstances;
	for(uint32_t iParticle = 0; iParticle < particleInstances.size(); ++iParticle) {
		SParticleInstance																& particleInstance												= particleInstances[iParticle];
		int32_t																			physicsId														= particleInstance.ParticleIndex;
		::cho::SParticle2<float>														& particleNext													= particleEngine.ParticleNext[physicsId];
		if( particleNext.Position.x < 0 || particleNext.Position.x >= applicationInstance.ViewOffscreenBitmap.width		()
		 || particleNext.Position.y < 0 || particleNext.Position.y >= applicationInstance.ViewOffscreenBitmap.height	()
		 ) { // Remove the particle instance and related information.
			particleEngine.ParticleState[physicsId].Unused								= true;
			particleInstances.erase(particleInstances.begin()+iParticle);
			--iParticle;
		}
		else { // Apply forces from wind and gravity.
			static constexpr	const double												gravity															= 9.8;
			::cho::SParticle2<float>														& particleCurrent												= particleEngine.Particle[physicsId];
			particleCurrent																= particleEngine.ParticleNext[physicsId];
			particleCurrent.Forces.AccumulatedForce										= {0, float(gravity * lastFrameSeconds) * -1};
			particleCurrent.Forces.AccumulatedForce.x									+= float(windDirection * abs(particleCurrent.Forces.Velocity.y) * .5) + float((rand() % 100) / 100.0 - .5) / 2.0f;
		}
	}

	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / applicationInstance.Timer.LastTimeSeconds, applicationInstance.Timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

					::cho::error_t										drawTextFixedSize							(::cho::SBitmapTargetBGRA& bmpTarget, ::cho::grid_view<::cho::SColorBGRA> viewTextureFont, uint32_t characterCellsX, int32_t dstOffsetY, const ::cho::SCoord2<int32_t>& sizeCharCell, const ::cho::view_const_string& text0, const ::cho::SCoord2<int32_t> dstTextOffset)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	for(int32_t iChar = 0, charCount = (int32_t)text0.size(); iChar < charCount; ++iChar) {
		int32_t																		coordTableX									= text0[iChar] % characterCellsX;
		int32_t																		coordTableY									= text0[iChar] / characterCellsX;
		const ::cho::SCoord2<int32_t>												coordCharTable								= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
		const ::cho::SCoord2<int32_t>												dstOffset1									= {sizeCharCell.x * iChar, dstOffsetY};
		const ::cho::SRectangle2D<int32_t>											srcRect0									= ::cho::SRectangle2D<int32_t>{{coordCharTable.x, (int32_t)viewTextureFont.height() - sizeCharCell.y - coordCharTable.y}, sizeCharCell};
		error_if(errored(::cho::grid_copy(bmpTarget.Colors, viewTextureFont, dstTextOffset + dstOffset1, srcRect0)), "I believe this never fails.");
	}
	return 0;
}

					::cho::error_t										draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::grid_view<::cho::SColorBGRA>											& viewOffscreen								= applicationInstance.ViewOffscreenBitmap;
	::cho::array_pod<SParticleInstance>												& particleInstances												= applicationInstance.ParticleInstances;
	for(uint32_t iParticle = 0, particleCount = (uint32_t)particleInstances.size(); iParticle < particleCount; ++iParticle) {
		SParticleInstance																& particleInstance												= particleInstances[iParticle];
		const int32_t																	physicsId														= particleInstance.ParticleIndex;
		const ::cho::SCoord2<float>														particlePosition												= applicationInstance.ParticleEngine.Particle[physicsId].Position;
		viewOffscreen[(uint32_t)particlePosition.y][(uint32_t)particlePosition.x]	
			= (PARTICLE_TYPE_SNOW == particleInstance.Type) ? ::cho::SColorBGRA(::cho::CYAN)
			: (PARTICLE_TYPE_FIRE == particleInstance.Type) ? ::cho::SColorBGRA(::cho::RED )
			: (PARTICLE_TYPE_RAIN == particleInstance.Type) ? ::cho::SColorBGRA(::cho::BLUE)
			: ::cho::SColorBGRA(::cho::ORANGE)// (PARTICLE_TYPE_LAVA == particleInstance.Type) ?
			;
	}

	return 0;
}
