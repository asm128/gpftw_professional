// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "cho_bitmap_file.h"
#include "cho_bitmap_target.h"
#include "cho_grid_scale.h"
#include "cho_grid_copy.h"
#include "cho_bit_array_view.h"

#include "cho_app_impl.h"

CHO_DEFINE_APPLICATION_ENTRY_POINT(::SApplication);	

static ::SApplication::TParticleSystem::TIntegrator::TParticle			particleDefinitions	[::PARTICLE_TYPE_COUNT]	= {};

static				void												setupParticles								()																				{
	particleDefinitions	[::PARTICLE_TYPE_PROJECTILE		].Position				= 
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST	].Position				= 
	particleDefinitions	[::PARTICLE_TYPE_STAR			].Position				= {};

	particleDefinitions	[::PARTICLE_TYPE_PROJECTILE		].SetMass				(1);
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST	].SetMass				(1);
	particleDefinitions	[::PARTICLE_TYPE_STAR			].SetMass				(1);

	particleDefinitions	[::PARTICLE_TYPE_PROJECTILE		].Damping				= 1.0f;
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST	].Damping				= 0.80f;
	particleDefinitions	[::PARTICLE_TYPE_STAR			].Damping				= 1.0f;

	particleDefinitions	[::PARTICLE_TYPE_PROJECTILE		].Forces.Velocity		= {};
	particleDefinitions	[::PARTICLE_TYPE_SHIP_THRUST	].Forces.Velocity		= {};
	particleDefinitions	[::PARTICLE_TYPE_STAR			].Forces.Velocity		= {};
}

					::SApplication										* g_ApplicationInstance						= 0;

static				::cho::error_t										updateSizeDependentResources				(::SApplication& applicationInstance)											{ 
	static constexpr	const ::cho::SCoord2<uint32_t>							GAME_SCREEN_SIZE							= {640, 360};
	::cho::updateSizeDependentTarget(applicationInstance.Framework.Offscreen.Texels, applicationInstance.Framework.Offscreen.View, GAME_SCREEN_SIZE);
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
	bool																		failedFromFile								= errored(::cho::bmpFileLoad(filename, textureToProcess.Original));
	char_t																		filenameBMGCompo	[256]					= {};
	strcpy_s(filenameBMGCompo, filename.begin());
	filenameBMGCompo[filename.size() - 1]									= 'g';
	const ::cho::view_const_string												filenameBMG									= filenameBMGCompo;
	if(failedFromFile) {
		error_printf("Failed to load bitmap from file: %s.", filename);
		error_if(errored(::cho::bmgFileLoad(filenameBMG, textureToProcess.Original)), "Failed to load image from disk: %s.", filenameBMG.begin());
	} 
	else {
		error_if(errored(::cho::bmgFileWrite(filenameBMG, textureToProcess.Original.View)), "Failed to store file on disk: %s.", filenameBMG.begin());
	}
	textureCenter															= (textureToProcess.Original.View.metrics() / 2).Cast<int32_t>();
	textureToProcess.Processed.View											= textureToProcess.Original.View;
	return 0;
}

static				::cho::error_t										setupSprites								(::SApplication& applicationInstance)											{ 
	static constexpr	const char*												bmpFileNames	[]							= 
		{ "Codepage-437-24.bmp"
		, "ship_0.bmp"
		, "ship_1.bmp"
		, "pow_core_0.bmp"
		, "pow_core_1.bmp"
		, "crosshair_template.bmp"
		, "pow_icon_0.bmp"
		, "enemy_0.bmp"
		};
	for(uint32_t iSprite = 0, spriteCount = ::cho::size(bmpFileNames); iSprite < spriteCount; ++iSprite)
		::setupSprite(applicationInstance.Textures[iSprite], applicationInstance.TextureCenters[iSprite], {bmpFileNames[iSprite], (uint32_t)strlen(bmpFileNames[iSprite])});

	const ::cho::grid_view<::cho::SColorBGRA>									& fontAtlas									= applicationInstance.Textures[GAME_TEXTURE_FONT_ATLAS].Processed.View;
	const ::cho::SCoord2<uint32_t>												& textureFontMetrics						= fontAtlas.metrics();
	applicationInstance.TextureFontMonochrome.resize(textureFontMetrics);
	for(uint32_t y = 0, yMax = textureFontMetrics.y; y < yMax; ++y)
	for(uint32_t x = 0, xMax = textureFontMetrics.x; x < xMax; ++x) {
		const ::cho::SColorBGRA														& pixelToTest								= fontAtlas[y][x];
		applicationInstance.TextureFontMonochrome.View[y * textureFontMetrics.x + x]	
		=	0 != pixelToTest.r
		||	0 != pixelToTest.g
		||	0 != pixelToTest.b
		;
	}
	applicationInstance.StuffToDraw.TexturesPowerup0.push_back(applicationInstance.Textures[GAME_TEXTURE_POWCORESQUARE		].Processed.View);
	applicationInstance.StuffToDraw.TexturesPowerup0.push_back(applicationInstance.Textures[GAME_TEXTURE_POWICON			].Processed.View);
	applicationInstance.StuffToDraw.TexturesPowerup1.push_back(applicationInstance.Textures[GAME_TEXTURE_POWCOREDIAGONAL	].Processed.View);
	applicationInstance.StuffToDraw.TexturesPowerup1.push_back(applicationInstance.Textures[GAME_TEXTURE_POWICON			].Processed.View);
	return 0;
}

					::cho::error_t										mainWindowCreate							(::cho::SDisplay& mainWindow, HINSTANCE hInstance);
					::cho::error_t										setup										(::SApplication& applicationInstance)											{ 
	//_CrtSetBreakAlloc(120);
	g_ApplicationInstance													= &applicationInstance;
	::cho::SFramework															& framework									= applicationInstance.Framework;
	error_if(errored(::mainWindowCreate(framework.MainDisplay, framework.RuntimeValues.PlatformDetail.EntryPointArgs.hInstance)), "Failed to create main window why?????!?!?!?!?");
	::setupParticles();
	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	ree_if	(errored(::setupSprites					(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	::cho::grid_view<::cho::SColorBGRA>											& fontAtlasView								= applicationInstance.Textures[GAME_TEXTURE_FONT_ATLAS].Processed.View;
	const ::cho::SCoord2<uint32_t>												& fontAtlasMetrics							= fontAtlasView.metrics();
	for(uint32_t y = 0, yMax = fontAtlasMetrics.y; y < yMax; ++y) 
	for(uint32_t x = 0, xMax = fontAtlasMetrics.x; x < xMax; ++x) {
		::cho::SColorBGRA															& curTexel									= fontAtlasView[y][x];
		if(curTexel.r == 0x00 && curTexel.g == 0x00 && curTexel.b == 0x00)
			curTexel																= {0xFF, 0x00, 0xFF, 0xFF};
	}

	::SGame																		& gameInstance								= applicationInstance.Game;
	for(uint32_t iShip = 0, shipCount = applicationInstance.Game.ShipsPlaying; iShip < shipCount; ++iShip) {
		gameInstance.Ships.Alive		[iShip]									= 1;
		gameInstance.Ships.Position		[iShip]									= framework.Offscreen.View.metrics().Cast<float>() / 4 + ::cho::SCoord2<float>{0, (float)iShip * 64};
		gameInstance.Ships.Weapon		[iShip]									= {.10f, 2000, iShip ? WEAPON_TYPE_PLASMA : WEAPON_TYPE_LASER};
		gameInstance.Ships.Health		[iShip].Health							= 5000;
		gameInstance.Ships.Health		[iShip].Shield							= 5000;
		gameInstance.PositionCrosshair	[iShip]									= gameInstance.Ships.Position[iShip] + ::cho::SCoord2<float>{64, };
		gameInstance.Powerups.Alive		[iShip]									= 1;
		gameInstance.Powerups.Position	[iShip]									= framework.Offscreen.View.metrics().Cast<float>() / 4 * 3 + ::cho::SCoord2<float>{0, (float)iShip * 64};
		gameInstance.Powerups.Family	[iShip]									= (POWERUP_FAMILY)iShip;
		if(gameInstance.Powerups.Family	[iShip] == POWERUP_FAMILY_WEAPON)
			gameInstance.Powerups.Type		[iShip].TypeWeapon						= (WEAPON_TYPE)(rand() % WEAPON_TYPE_COUNT);
		else
			gameInstance.Powerups.Type		[iShip].TypeHealth						= (HEALTH_TYPE)(rand() % 2);
		++gameInstance.CountPowerups;
	}
	//gameInstance.PositionPowerup											= framework.Offscreen.View.metrics().Cast<float>() / 4U * 3U;
	applicationInstance.PSOffsetFromShipCenter								= {-applicationInstance.TextureCenters[GAME_TEXTURE_SHIP0].x};
	return 0;
}

					::cho::error_t										drawBackground								(::SApplication& applicationInstance);	
					::cho::error_t										drawShots									(::SApplication& applicationInstance);	
					::cho::error_t										drawThrust									(::SApplication& applicationInstance);	
					::cho::error_t										drawPowerups								(::SApplication& applicationInstance);	
					::cho::error_t										drawShips									(::SApplication& applicationInstance);	
					::cho::error_t										drawCrosshair								(::SApplication& applicationInstance);	
					::cho::error_t										drawCollisions								(::SApplication& applicationInstance);	


template<size_t _sizeString>
static				::cho::error_t										textCalcSizeLine							(const ::cho::SCoord2<int32_t>& sizeCharCell, const char (&text0)[_sizeString] )	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	return (::cho::error_t)(sizeCharCell.x * ::cho::size(text0) - 1);
}

static				::cho::error_t										textDrawFixedSize							(::cho::grid_view<::cho::SColorBGRA>& bmpTarget, const ::cho::grid_view<::cho::SColorBGRA>& viewTextureFont, uint32_t characterCellsX, int32_t dstOffsetY, const ::cho::SCoord2<int32_t>& sizeCharCell, const ::cho::view_const_string& text0, const ::cho::SCoord2<int32_t> dstTextOffset)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	for(int32_t iChar = 0, charCount = (int32_t)text0.size(); iChar < charCount; ++iChar) {
		int32_t																	coordTableX										= text0[iChar] % characterCellsX;
		int32_t																	coordTableY										= text0[iChar] / characterCellsX;
		const ::cho::SCoord2<int32_t>											coordCharTable									= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
		const ::cho::SCoord2<int32_t>											dstOffset1										= {sizeCharCell.x * iChar, dstOffsetY};
		const ::cho::SRectangle2D<int32_t>										srcRect0										= ::cho::SRectangle2D<int32_t>{{coordCharTable.x, (int32_t)viewTextureFont.height() - sizeCharCell.y - coordCharTable.y}, sizeCharCell};
		error_if(errored(::cho::grid_copy_alpha(bmpTarget, viewTextureFont, dstTextOffset + dstOffset1, srcRect0, {0xFF, 0x00, 0xFF, 0xFF})), "I believe this never fails.");
		//error_if(errored(::cho::grid_copy(bmpTarget, viewTextureFont, dstTextOffset + dstOffset1, srcRect0)), "I believe this never fails.");
	}
	return 0;
}

template<size_t _sizeString>
static				::cho::error_t										textDrawAlignedFixedSize					(::cho::grid_view<::cho::SColorBGRA>& targetView, const ::cho::grid_view<::cho::SColorBGRA>& fontAtlas, uint32_t lineOffset, const ::cho::SCoord2<uint32_t>& targetSize, const ::cho::SCoord2<int32_t>& sizeCharCell, const char (&text0)[_sizeString] )	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::cho::SCoord2<int32_t>												dstTextOffset								= {(int32_t)targetSize.x / 2 - (int32_t)textCalcSizeLine(sizeCharCell, text0) / 2, };
	uint32_t																	dstOffsetY									= (int32_t)(targetSize.y - lineOffset * sizeCharCell.y - sizeCharCell.y);
	return ::textDrawFixedSize(targetView, fontAtlas, 32, dstOffsetY, sizeCharCell, {text0, ::cho::size(text0) -1}, dstTextOffset);
}

///---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static				::cho::error_t										textDrawFixedSize							(::cho::grid_view<::cho::SColorBGRA>& bmpTarget, const ::cho::bit_array_view<uint32_t>& viewTextureFont, const ::cho::SCoord2<uint32_t> & viewMetrics, uint32_t characterCellsX, int32_t dstOffsetY, const ::cho::SCoord2<int32_t>& sizeCharCell, const ::cho::view_const_string& text0, const ::cho::SCoord2<int32_t> dstTextOffset, const ::cho::SColorBGRA& color)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::array_pod<::cho::SCoord2<uint32_t>>								dstCoords;
	for(int32_t iChar = 0, charCount = (int32_t)text0.size(); iChar < charCount; ++iChar) {
		int32_t																	coordTableX										= text0[iChar] % characterCellsX;
		int32_t																	coordTableY										= text0[iChar] / characterCellsX;
		const ::cho::SCoord2<int32_t>											coordCharTable									= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
		const ::cho::SCoord2<int32_t>											dstOffset1										= {sizeCharCell.x * iChar, dstOffsetY};
		const ::cho::SRectangle2D<int32_t>										srcRect0										= ::cho::SRectangle2D<int32_t>{{coordCharTable.x, (int32_t)viewMetrics.y - sizeCharCell.y - coordCharTable.y}, sizeCharCell};
		//error_if(errored(::cho::grid_copy_alpha_bit(bmpTarget, viewTextureFont, dstTextOffset + dstOffset1, viewMetrics, ::cho::SColorBGRA{0, 0xFF, 0xFF, 0xFF}, srcRect0)), "I believe this never fails.");
		dstCoords.clear();
		error_if(errored(::cho::grid_raster_alpha_bit(bmpTarget, viewTextureFont, dstTextOffset + dstOffset1, viewMetrics, srcRect0, dstCoords)), "I believe this never fails.");
		for(uint32_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord) {
			::cho::drawPixelLight(bmpTarget, dstCoords[iCoord], color, 0.05f, 0.75);
		}
	}
	return 0;
}

template<size_t _sizeString>
static				::cho::error_t										textDrawAlignedFixedSize					(::cho::grid_view<::cho::SColorBGRA>& targetView, const ::cho::bit_array_view<uint32_t>& fontAtlas, const ::cho::SCoord2<uint32_t> & viewMetrics, uint32_t lineOffset, const ::cho::SCoord2<uint32_t>& targetSize, const ::cho::SCoord2<int32_t>& sizeCharCell, const char (&text0)[_sizeString], const ::cho::SColorBGRA& color)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::cho::SCoord2<int32_t>												dstTextOffset								= {(int32_t)targetSize.x / 2 - (int32_t)textCalcSizeLine(sizeCharCell, text0) / 2, };
	uint32_t																	dstOffsetY									= (int32_t)(targetSize.y - lineOffset * sizeCharCell.y - sizeCharCell.y);
	return ::textDrawFixedSize(targetView, fontAtlas, viewMetrics, 32, dstOffsetY, sizeCharCell, {text0, ::cho::size(text0) -1}, dstTextOffset, color);
}


					::cho::error_t										draw										(::SApplication& applicationInstance)											{	
	error_if(errored(::drawBackground		(applicationInstance)), "Why??");		// --- Draw stars
	error_if(errored(::drawPowerups			(applicationInstance)), "Why??");		// --- Draw powerups
	error_if(errored(::drawShips			(applicationInstance)), "Why??");		// --- Draw ship
	error_if(errored(::drawCrosshair		(applicationInstance)), "Why??");		// --- Draw crosshair
	error_if(errored(::drawThrust			(applicationInstance)), "Why??");		// --- Draw propulsion engine
	error_if(errored(::drawShots			(applicationInstance)), "Why??");		// --- Draw lasers
	error_if(errored(::drawCollisions		(applicationInstance)), "Why??");		// --- Draw lasers

	static constexpr const ::cho::SCoord2<int32_t>								sizeCharCell								= {9, 16};
	uint32_t																	lineOffset									= 0;
	static constexpr const char													textLine0	[]								= "W: Up, S: Down, A: Left, D: Right";
	static constexpr const char													textLine1	[]								= "T: Shoot. Y: Thrust. U: Handbrake.";
	static constexpr const char													textLine2	[]								= "Press ESC to exit.";
	::cho::grid_view<::cho::SColorBGRA>											& offscreenView								= applicationInstance.Framework.Offscreen.View;
	::cho::grid_view<::cho::SColorBGRA>											& fontAtlasView								= applicationInstance.Textures[GAME_TEXTURE_FONT_ATLAS].Processed.View;
	const ::cho::SCoord2<uint32_t>												& offscreenMetrics							= offscreenView.metrics();
	::textDrawAlignedFixedSize(offscreenView, applicationInstance.TextureFontMonochrome.View, fontAtlasView.metrics(), lineOffset, offscreenMetrics, sizeCharCell, textLine0, ::cho::SColorBGRA{0, applicationInstance.Framework.FrameInfo.FrameNumber % 0xFF, 0xFFU, 0xFFU});	++lineOffset;
	::textDrawAlignedFixedSize(offscreenView, applicationInstance.TextureFontMonochrome.View, fontAtlasView.metrics(), lineOffset, offscreenMetrics, sizeCharCell, textLine1, ::cho::SColorBGRA{applicationInstance.Framework.FrameInfo.FrameNumber % 0xFFU, 0xFFU, 0, 0xFFU});	++lineOffset;
	::textDrawAlignedFixedSize(offscreenView, fontAtlasView, lineOffset = offscreenMetrics.y / 16 - 1, offscreenMetrics, sizeCharCell, textLine2);	--lineOffset;
	return 0;
}

					::cho::error_t										removeDeadStuff								(::SApplication& applicationInstance);
					::cho::error_t										updateInput									(::SApplication& applicationInstance);
					::cho::error_t										updateShots									(::SApplication& applicationInstance, const ::cho::array_view<::SApplication::TParticleSystem::TIntegrator::TParticle> & particleDefinitions);
					::cho::error_t										updateSpawn									(::SApplication& applicationInstance, const ::cho::array_view<::SApplication::TParticleSystem::TIntegrator::TParticle> & particleDefinitions);
					::cho::error_t										updateShips									(::SApplication& applicationInstance);
					::cho::error_t										updateEnemies								(::SApplication& applicationInstance);
					::cho::error_t										updateParticles								(::SApplication& applicationInstance);
 					::cho::error_t										updateGUI									(::SApplication& applicationInstance);
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

	error_if(errored(::removeDeadStuff	(applicationInstance)), "Unknown error.");
	error_if(errored(::updateParticles	(applicationInstance)), "Unknown error.");
	error_if(errored(::updateSpawn		(applicationInstance, particleDefinitions)), "Unknown error.");
	error_if(errored(::updateShips		(applicationInstance)), "Unknown error.");
	error_if(errored(::updateEnemies	(applicationInstance)), "Unknown error.");
	error_if(errored(::updateShots		(applicationInstance, particleDefinitions)), "Unknown error.");
	error_if(errored(::updateGUI		(applicationInstance)), "Unknown error.");

	::cho::STimer																& timer										= framework.Timer;
	::cho::SDisplay																& mainWindow								= framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. Projecitle fx count: %u. Thrust fx count: %u. Stars fx count: %u. Debris fx count: %u. Total fx count: %u. Enemy count: %u. Projectile count: %u. Powerup count: %u. FPS: %g. Last frame seconds: %g."
		, mainWindow.Size.x, mainWindow.Size.y
		, applicationInstance.ParticleSystemProjectiles	.Instances.size()
		, applicationInstance.ParticleSystemThrust		.Instances.size()
		, applicationInstance.ParticleSystemStars		.Instances.size()
		, applicationInstance.ParticleSystemDebris		.Instances.size()
		, applicationInstance.ParticleSystemProjectiles	.Instances.size()
		+ applicationInstance.ParticleSystemThrust		.Instances.size()
		+ applicationInstance.ParticleSystemStars		.Instances.size()
		+ applicationInstance.ParticleSystemDebris		.Instances.size()
		, applicationInstance.Game.CountEnemies
		, applicationInstance.Game.CountProjectiles
		, applicationInstance.Game.CountPowerups
		, 1 / timer.LastTimeSeconds
		, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}
