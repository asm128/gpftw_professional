// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "cho_bitmap_target.h"
#include "cho_bitmap_file.h"

#include "cho_app_impl.h"

static constexpr	const uint32_t										ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t										ASCII_SCREEN_HEIGHT							= 50	;

CHO_DEFINE_APPLICATION_ENTRY_POINT(::SApplication);	

					::SApplication										* g_ApplicationInstance						= 0;

static				::cho::error_t										updateOffscreen								(::SApplication& applicationInstance)											{ 
	::cho::array_pod<::cho::SColorBGRA>											& bmpOffscreen								= applicationInstance.BitmapOffsceen;
	uint32_t																	linearScreenSize							= applicationInstance.MainDisplay.Size.x * applicationInstance.MainDisplay.Size.y;
	if(bmpOffscreen.size() < linearScreenSize)
		ree_if(errored(bmpOffscreen.resize(linearScreenSize)), "Out of memory?");
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
	error_if(errored(::cho::asciiDisplayDestroy	()), "Failed to close ASCII display!");
	error_if(errored(::cho::asciiTargetDestroy	(applicationInstance.ASCIIRenderTarget)), "No known reasons for this to fail. It may mean a mismamagement of the target array pointers!");

	g_ApplicationInstance													= 0;
	//error_printf("Error message test. Press F5 to continue if the debugger breaks execution at this point.");
	return 0;
}

					::cho::error_t										mainWindowCreate							(::cho::SDisplay& mainWindow, HINSTANCE hInstance);

// --- Initialize console.
					::cho::error_t										setup										(::SApplication& applicationInstance)											{ 
	g_ApplicationInstance													= &applicationInstance;
	error_if(errored(::cho::asciiTargetCreate	(applicationInstance.ASCIIRenderTarget, ::ASCII_SCREEN_WIDTH, ::ASCII_SCREEN_HEIGHT)						), "This should never happen and usually indicates memory corruption or lack of system resources.");
	error_if(errored(::cho::asciiDisplayCreate	(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height())				), "Not sure why this would fail at this point.");
	error_if(errored(::mainWindowCreate			(applicationInstance.MainDisplay, applicationInstance.RuntimeValues.PlatformDetail.EntryPointArgs.hInstance)), "Failed to create main window why?????!?!?!?!?");
	static constexpr	const char												fontFileName	[]							= "Codepage-437-24.bmp";
	FILE																		* source									= 0; 
	fopen_s(&source, fontFileName, "rb");
	if(source) {
		if errored(::cho::bmpFileLoad(source, applicationInstance.TextureFont, applicationInstance.ViewTextureFont)) {
			error_printf("Failed to load file: '%s'. File not found?", fontFileName);
			fclose(source);
			return -1;
		}
		fclose(source);
	}

	static constexpr	const char												bkgdFileName	[]							= "earth_light.bmp";
	source																	= 0; 
	fopen_s(&source, bkgdFileName, "rb");
	if(source) {
		if errored(::cho::bmpFileLoad(source, applicationInstance.TextureBackground, applicationInstance.ViewTextureBackground)) {
			error_printf("Failed to load file: '%s'. File not found?", bkgdFileName);
			fclose(source);
			return -1;
		}
		fclose(source);
	}

	return 0;
}


					::cho::error_t										update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	error_if(errored(::cho::asciiDisplayPresent	(applicationInstance.ASCIIRenderTarget)), "Could this fail if the console isn't properly initialized?");
	error_if(errored(::cho::asciiTargetClear	(applicationInstance.ASCIIRenderTarget)), "This shouldn't fail either unless memory corruption happened.");
	applicationInstance.Timer		.Frame();
	applicationInstance.FrameInfo	.Frame(applicationInstance.Timer.LastTimeMicroseconds);
	::cho::SDisplay																& mainWindow								= applicationInstance.MainDisplay;
	ree_if(errored(::cho::displayUpdate(mainWindow)), "Not sure why this would fail.");
	ree_if(errored(::updateOffscreen(applicationInstance)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	retval_info_if(1, 0 == mainWindow.PlatformDetail.WindowHandle, "Application exiting because the main window was closed.");
	error_if(errored(::cho::displayPresentTarget(mainWindow, applicationInstance.BitmapOffsceen)), "Unknown error.");

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

	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. Last frame seconds: %g. ", mainWindow.Size.x, mainWindow.Size.y, applicationInstance.Timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

					::cho::error_t										draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::cho::SDisplay														& mainWindow								= applicationInstance.MainDisplay;
	::cho::array_pod	<::cho::SColorBGRA>										& bmpOffscreen								= applicationInstance.BitmapOffsceen;
	::cho::SCoord2		<int32_t>												screenCenter								= {(int32_t)mainWindow.Size.x / 2, (int32_t)mainWindow.Size.y / 2};
	::cho::SRectangle2D	<int32_t>												geometry0									= {{2, 2}, {(int32_t)((applicationInstance.FrameInfo.FrameNumber / 2) % (mainWindow.Size.x - 2)), 5}};
	::cho::SCircle2D	<int32_t>												geometry1									= {5.0 + (applicationInstance.FrameInfo.FrameNumber / 5) % 5, screenCenter};	
	::cho::STriangle2D	<int32_t>												geometry2									= {{0, 0}, {15, 15}, {-5, 10}};	
	geometry2.A																+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.B																+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.C																+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};

	const ::cho::SCoord2<uint32_t>													dstOffset0								= {mainWindow.Size.x / 2 - applicationInstance.ViewTextureBackground.width() / 2, mainWindow.Size.y / 2 - applicationInstance.ViewTextureBackground.height() / 2};
	const ::cho::SCoord2<uint32_t>													dstSize0								= {mainWindow.Size.x / 2 - applicationInstance.ViewTextureBackground.width() / 2, mainWindow.Size.y / 2 - applicationInstance.ViewTextureBackground.height() / 2};

	::cho::SBitmapTargetBGRA													bmpTarget									= {{&bmpOffscreen[0], mainWindow.Size.x, mainWindow.Size.y},};
	::memset(&bmpOffscreen[0], 0, sizeof(::cho::SColorBGRA) * bmpOffscreen.size());	// Clear target.
	error_if(errored(::cho::bitBlt(bmpTarget.Colors, applicationInstance.ViewTextureBackground, dstOffset0)), "I believe this never fails.");

	error_if(errored(::cho::drawRectangle	(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_BLUE		]), geometry0)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawRectangle	(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_BLUE		]), ::cho::SRectangle2D<int32_t>{geometry0.Offset + ::cho::SCoord2<int32_t>{1, 1}, geometry0.Size - ::cho::SCoord2<int32_t>{2, 2}})	), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawCircle		(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_GREEN		]), geometry1)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawCircle		(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_RED		]), ::cho::SCircle2D<int32_t>{geometry1.Radius - 1, geometry1.Center})															), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawTriangle	(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_YELLOW	]), geometry2)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_MAGENTA	]), ::cho::SLine2D<int32_t>{geometry2.A, geometry2.B})													), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_WHITE		]), ::cho::SLine2D<int32_t>{geometry2.B, geometry2.C})													), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_LIGHTGREY	]), ::cho::SLine2D<int32_t>{geometry2.C, geometry2.A})													), "Not sure if these functions could ever fail");
	//error_if(errored(::cho::bitBlt(bmpTarget.Colors, applicationInstance.ViewTextureFont)), "I believe this never fails.");


	static constexpr const char														testText1	[]							= "Text";
	static constexpr const char														testText2	[]							= "Lines";
	static constexpr const ::cho::SCoord2<int32_t>									sizeCharCell							= {9, 16};
	uint32_t																		lineOffset								= 0;
	{
		static constexpr const char														testText0	[]							= "Some";
		for(int32_t iChar = 0; iChar < (int32_t)::cho::size(testText0); ++iChar) {
			int32_t																			coordTableX								= testText0[iChar] % 32;
			int32_t																			coordTableY								= testText0[iChar] / 32;
			const ::cho::SCoord2<int32_t>													coordCharTable							= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
			const ::cho::SRectangle2D<int32_t>												dstRect0								= ::cho::SRectangle2D<int32_t>{{sizeCharCell.x * iChar, (int32_t)(mainWindow.Size.y - lineOffset * sizeCharCell.y - sizeCharCell.y)}, sizeCharCell};
			const ::cho::SRectangle2D<int32_t>												srcRect0								= ::cho::SRectangle2D<int32_t>{{coordCharTable.x, (int32_t)applicationInstance.ViewTextureFont.height() - sizeCharCell.y - coordCharTable.y}, sizeCharCell};
			error_if(errored(::cho::bitBlt(bmpTarget.Colors, applicationInstance.ViewTextureFont, dstRect0, srcRect0)), "I believe this never fails.");
		}
	}
	++lineOffset;
	{
		static constexpr const char														testText0	[]							= "Text";
		for(int32_t iChar = 0; iChar < (int32_t)::cho::size(testText0); ++iChar) {
			int32_t																			coordTableX								= testText0[iChar] % 32;
			int32_t																			coordTableY								= testText0[iChar] / 32;
			const ::cho::SCoord2<int32_t>													coordCharTable							= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
			const ::cho::SRectangle2D<int32_t>												dstRect0								= ::cho::SRectangle2D<int32_t>{{sizeCharCell.x * iChar, (int32_t)(mainWindow.Size.y - lineOffset * sizeCharCell.y - sizeCharCell.y)}, sizeCharCell};
			const ::cho::SRectangle2D<int32_t>												srcRect0								= ::cho::SRectangle2D<int32_t>{{coordCharTable.x, (int32_t)applicationInstance.ViewTextureFont.height() - sizeCharCell.y - coordCharTable.y}, sizeCharCell};
			error_if(errored(::cho::bitBlt(bmpTarget.Colors, applicationInstance.ViewTextureFont, dstRect0, srcRect0)), "I believe this never fails.");
		}
	}
	++lineOffset;
	{
		static constexpr const char														testText0	[]							= "Lines";
		for(int32_t iChar = 0; iChar < (int32_t)::cho::size(testText0); ++iChar) {
			int32_t																			coordTableX								= testText0[iChar] % 32;
			int32_t																			coordTableY								= testText0[iChar] / 32;
			const ::cho::SCoord2<int32_t>													coordCharTable							= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
			const ::cho::SRectangle2D<int32_t>												dstRect0								= ::cho::SRectangle2D<int32_t>{{sizeCharCell.x * iChar, (int32_t)(mainWindow.Size.y - lineOffset * sizeCharCell.y - sizeCharCell.y)}, sizeCharCell};
			const ::cho::SRectangle2D<int32_t>												srcRect0								= ::cho::SRectangle2D<int32_t>{{coordCharTable.x, (int32_t)applicationInstance.ViewTextureFont.height() - sizeCharCell.y - coordCharTable.y}, sizeCharCell};
			error_if(errored(::cho::bitBlt(bmpTarget.Colors, applicationInstance.ViewTextureFont, dstRect0, srcRect0)), "I believe this never fails.");
		}
	}
	++lineOffset;

	const ::cho::SRectangle2D<uint32_t>												dstRect0								= ::cho::SRectangle2D<uint32_t>{{ 9,  16}, {9, 16}};
	const ::cho::SRectangle2D<uint32_t>												dstRect1								= ::cho::SRectangle2D<uint32_t>{{18,  32}, {9, 16}};
	const ::cho::SRectangle2D<uint32_t>												dstRect2								= ::cho::SRectangle2D<uint32_t>{{27,  48}, {9, 16}};
	const ::cho::SRectangle2D<uint32_t>												dstRect3								= ::cho::SRectangle2D<uint32_t>{{36,  64}, {9, 16}};
	const ::cho::SRectangle2D<uint32_t>												dstRect4								= ::cho::SRectangle2D<uint32_t>{{45,  80}, {9, 16}};
	
	const ::cho::SRectangle2D<uint32_t>												srcRect0								= ::cho::SRectangle2D<uint32_t>{{ 9,  applicationInstance.ViewTextureFont.height() - 16}, {9, 16}};
	const ::cho::SRectangle2D<uint32_t>												srcRect1								= ::cho::SRectangle2D<uint32_t>{{18,  applicationInstance.ViewTextureFont.height() - 16}, {9, 16}};
	const ::cho::SRectangle2D<uint32_t>												srcRect2								= ::cho::SRectangle2D<uint32_t>{{27,  applicationInstance.ViewTextureFont.height() - 16}, {9, 16}};
	const ::cho::SRectangle2D<uint32_t>												srcRect3								= ::cho::SRectangle2D<uint32_t>{{36,  applicationInstance.ViewTextureFont.height() - 16}, {9, 16}};
	const ::cho::SRectangle2D<uint32_t>												srcRect4								= ::cho::SRectangle2D<uint32_t>{{45,  applicationInstance.ViewTextureFont.height() - 16}, {9, 16}};
	error_if(errored(::cho::bitBlt(bmpTarget.Colors, applicationInstance.ViewTextureFont, dstRect0, srcRect0.Offset)), "I believe this never fails.");
	error_if(errored(::cho::bitBlt(bmpTarget.Colors, applicationInstance.ViewTextureFont, dstRect1.Offset, srcRect1.Offset)), "I believe this never fails.");
	error_if(errored(::cho::bitBlt(bmpTarget.Colors, applicationInstance.ViewTextureFont, dstRect2, srcRect2)), "I believe this never fails.");
	error_if(errored(::cho::bitBlt(bmpTarget.Colors, applicationInstance.ViewTextureFont, dstRect3, srcRect3)), "I believe this never fails.");
	error_if(errored(::cho::bitBlt(bmpTarget.Colors, applicationInstance.ViewTextureFont, dstRect4, srcRect4)), "I believe this never fails.");
	return 0;
}
