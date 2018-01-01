// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "cho_bitmap_target.h"
#include "cho_bitmap_file.h"

#include "cho_app_impl.h"

static constexpr	const uint32_t																	ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t																	ASCII_SCREEN_HEIGHT							= 50	;

					::cho::error_t																	setup										(::SApplication& applicationInstance);
					::cho::error_t																	cleanup										(::SApplication& applicationInstance);
					::cho::error_t																	update										(::SApplication& applicationInstance, bool systemRequestedExit);
					::cho::error_t																	draw										(::SApplication& applicationInstance);

CHO_DEFINE_APPLICATION_ENTRY_POINT(::SApplication);	

					::SApplication																	* g_ApplicationInstance						= 0;

static				void																			updateOffscreen								(::SApplication& applicationInstance)											{ 
	::cho::array_pod<::cho::SColorBGRA>																		& bmpOffscreen								= applicationInstance.BitmapOffsceen;
	uint32_t																								linearScreenSize							= applicationInstance.MainDisplay.Size.x * applicationInstance.MainDisplay.Size.y;
	if(bmpOffscreen.size() < linearScreenSize)
		bmpOffscreen.resize(linearScreenSize);
}

// --- Cleanup application resources.
					::cho::error_t																	cleanup										(::SApplication& applicationInstance)											{ 
	::cho::SDisplayPlatformDetail																			& displayDetail								= applicationInstance.MainDisplay.PlatformDetail;
	if(displayDetail.WindowHandle) {
		error_if(0 == ::DestroyWindow(displayDetail.WindowHandle), "Not sure why would this fail.");
		::cho::displayUpdate(applicationInstance.MainDisplay);
	}
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	error_if(errored(::cho::asciiDisplayDestroy	()), "Failed to close ASCII display!");
	error_if(errored(::cho::asciiTargetDestroy	(applicationInstance.ASCIIRenderTarget)), "No known reasons for this to fail. It may mean a mismamagement of the target array pointers!");

	g_ApplicationInstance																				= 0;
	//error_printf("Error message test. Press F5 if the debugger breaks execution at this point.");
	return 0;
}

					::cho::error_t																	mainWindowCreate							(::cho::SDisplay& mainWindow, HINSTANCE hInstance);

// --- Initialize console.
					::cho::error_t																	setup										(::SApplication& applicationInstance)											{ 
	g_ApplicationInstance																				= &applicationInstance;
	error_if(errored(::cho::asciiTargetCreate	(applicationInstance.ASCIIRenderTarget, ::ASCII_SCREEN_WIDTH, ::ASCII_SCREEN_HEIGHT)						), "This should never happen and usually indicates memory corruption or lack of system resources.");
	error_if(errored(::cho::asciiDisplayCreate	(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height())				), "Not sure why this would fail at this point.");
	error_if(errored(::mainWindowCreate			(applicationInstance.MainDisplay, applicationInstance.RuntimeValues.PlatformDetail.EntryPointArgs.hInstance)), "Failed to create main window why?????!?!?!?!?");
	static constexpr	const char																			fontFileName	[]							= "Codepage-437-24.bmp";
	FILE																									* source									= 0; 
	fopen_s(&source, fontFileName, "rb");
	if(source) {
		::cho::array_pod<::cho::SColorBGRA>																		& colors									= applicationInstance.FontTexture;
		cho_necall(::cho::bmpFileLoad(source, colors), "Failed to load file: '%s'. File not found?", fontFileName);
		fclose(source);
	}

	return 0;
}

					::cho::error_t																	update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	error_if(errored(::cho::asciiDisplayPresent	(applicationInstance.ASCIIRenderTarget)), "Could this fail if the console isn't properly initialized?");
	error_if(errored(::cho::asciiTargetClear	(applicationInstance.ASCIIRenderTarget)), "This shouldn't fail either unless memory corruption happened.");
	applicationInstance.Timer		.Frame();
	applicationInstance.FrameInfo	.Frame(applicationInstance.Timer.LastTimeMicroseconds);
	::cho::SDisplay																							& mainWindow								= applicationInstance.MainDisplay;
	::cho::displayUpdate(mainWindow);
	::updateOffscreen	(applicationInstance);
	retval_info_if(1, 0 == mainWindow.PlatformDetail.WindowHandle, "Application exiting because the main window was closed.");
	error_if(errored(::cho::displayPresentTarget(mainWindow, applicationInstance.BitmapOffsceen)), "Unknown error.");

	char																									buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. Last frame seconds: %g. ", mainWindow.Size.x, mainWindow.Size.y, applicationInstance.Timer.LastTimeSeconds);
	::HWND																									windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

					::cho::error_t																	draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::cho::SDisplay																					& mainWindow								= applicationInstance.MainDisplay;
	::cho::array_pod<::cho::SColorBGRA>																		& bmpOffscreen								= applicationInstance.BitmapOffsceen;
	::cho::SCoord2		<int32_t>																			screenCenter								= {(int32_t)mainWindow.Size.x / 2, (int32_t)mainWindow.Size.y / 2};
	::cho::SRectangle2D<int32_t>																			geometry0									= {{2, 2}, {(int32_t)((applicationInstance.FrameInfo.FrameNumber / 2) % (mainWindow.Size.x - 2)), 5}};
	::cho::SCircle2D	<int32_t>																			geometry1									= {5.0 + (applicationInstance.FrameInfo.FrameNumber / 5) % 5, screenCenter};	
	::cho::STriangle2D	<int32_t>																			geometry2									= {{0, 0}, {15, 15}, {-5, 10}};	
	geometry2.A																							+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.B																							+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.C																							+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	::cho::SBitmapTargetBGRA																				bmpTarget									= {{&bmpOffscreen[0], mainWindow.Size.x, mainWindow.Size.y},};
	::memset(&bmpOffscreen[0], 0, sizeof(::cho::SColorBGRA) * bmpOffscreen.size());
	error_if(errored(::cho::drawRectangle	(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_BLUE		]), geometry0)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawRectangle	(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_BLUE		]), {geometry0.Offset + ::cho::SCoord2<int32_t>{1, 1}, geometry0.Size - ::cho::SCoord2<int32_t>{2, 2}})	), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawCircle		(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_GREEN		]), geometry1)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawCircle		(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_RED		]), {geometry1.Radius - 1, geometry1.Center})															), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawTriangle	(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_YELLOW	]), geometry2)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_MAGENTA	]), ::cho::SLine2D<int32_t>{geometry2.A, geometry2.B})													), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_WHITE		]), ::cho::SLine2D<int32_t>{geometry2.B, geometry2.C})													), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(bmpTarget, ::cho::SColorRGBA(applicationInstance.ASCIIPalette[::cho::ASCII_COLOR_LIGHTGREY	]), ::cho::SLine2D<int32_t>{geometry2.C, geometry2.A})													), "Not sure if these functions could ever fail");
	for(uint32_t y = 0, yMax = 128; y < yMax; ++y)
	for(uint32_t x = 0, xMax = 288; x < xMax; ++x) {
		bmpTarget.Colors[y][x]																				= applicationInstance.FontTexture[y * xMax + x];
	}
	return 0;
}
