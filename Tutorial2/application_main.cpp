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

					::SApplication										* g_ApplicationInstance						= 0;
					
static				::cho::error_t										updateSizeDependentResources				(::SApplication& applicationInstance)											{ 
	const ::cho::SCoord2<uint32_t>												newSize										= applicationInstance.MainDisplay.Size / 2;
	::cho::updateSizeDependentTarget	(applicationInstance.BitmapOffscreen					, newSize);
	::cho::updateSizeDependentTexture	(applicationInstance.TextureBackgroundScaledDay			, applicationInstance.TextureBackgroundDay		.View, newSize);
	::cho::updateSizeDependentTexture	(applicationInstance.TextureBackgroundScaledNight		, applicationInstance.TextureBackgroundNight	.View, newSize);
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
	static constexpr	const char												bmpFileName0	[]									= "earth_color.bmp";
	static constexpr	const char												bmpFileName1	[]									= "earth_light.bmp";
	error_if(errored(::cho::bmpFileLoad((::cho::view_const_string)bmpFileName0, applicationInstance.TextureBackgroundDay	)), "Failed to load bitmap from file: %s.", bmpFileName0);
	error_if(errored(::cho::bmpFileLoad((::cho::view_const_string)bmpFileName1, applicationInstance.TextureBackgroundNight	)), "Failed to load bitmap from file: %s.", bmpFileName1);
	return 0;
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
	error_if(errored(::cho::displayPresentTarget(mainWindow, applicationInstance.BitmapOffscreen.View)), "Unknown error.");

	::updateInput(applicationInstance);

	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / applicationInstance.Timer.LastTimeSeconds, applicationInstance.Timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

					::cho::error_t										drawTextFixedSize							(::cho::grid_view<::cho::SColorBGRA>& bmpTarget, ::cho::grid_view<::cho::SColorBGRA> viewTextureFont, uint32_t characterCellsX, int32_t dstOffsetY, const ::cho::SCoord2<int32_t>& sizeCharCell, const ::cho::view_const_string& text0, const ::cho::SCoord2<int32_t> dstTextOffset)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	for(int32_t iChar = 0, charCount = (int32_t)text0.size(); iChar < charCount; ++iChar) {
		int32_t																		coordTableX									= text0[iChar] % characterCellsX;
		int32_t																		coordTableY									= text0[iChar] / characterCellsX;
		const ::cho::SCoord2<int32_t>												coordCharTable								= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
		const ::cho::SCoord2<int32_t>												dstOffset1									= {sizeCharCell.x * iChar, dstOffsetY};
		const ::cho::SRectangle2D<int32_t>											srcRect0									= ::cho::SRectangle2D<int32_t>{{coordCharTable.x, (int32_t)viewTextureFont.height() - sizeCharCell.y - coordCharTable.y}, sizeCharCell};
		error_if(errored(::cho::grid_copy(bmpTarget, viewTextureFont, dstTextOffset + dstOffset1, srcRect0)), "I believe this never fails.");
	}
	return 0;
}

					::cho::error_t										draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::cho::grid_view<::cho::SColorBGRA>									& viewOffscreen								= applicationInstance.BitmapOffscreen.View;
	const::cho::SCoord2	<int32_t>												screenCenter								= {(int32_t)viewOffscreen.width() / 2, (int32_t)viewOffscreen.height() / 2};
	::cho::SRectangle2D	<int32_t>												geometry0									= {{2, 2}, {(int32_t)((applicationInstance.FrameInfo.FrameNumber) % (viewOffscreen.width() - 2)), 5}};
	::cho::SCircle2D	<int32_t>												geometry1									= {5.0 + (applicationInstance.FrameInfo.FrameNumber / 5) % 5, screenCenter};	
	::cho::STriangle2D	<int32_t>												geometry2									= {{0, 0}, {15, 15}, {-5, 10}};	
	geometry2.A																+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.B																+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.C																+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};

	::cho::SRectangle2D	<uint32_t>												rectangle									= {};
	rectangle.Offset.x														= applicationInstance.TextureBackgroundScaledNight.View.width() / 4;
	rectangle.Offset.y														= 0; 
	rectangle.Size	.x														= rectangle.Offset.x * 2;
	rectangle.Size	.y														= applicationInstance.TextureBackgroundScaledNight.View.height(); // - rectangle.Offset.y * 2;

	rectangle.Offset.x														+= (uint32_t)applicationInstance.FrameInfo.FrameNumber ;
	rectangle.Offset.x														%= applicationInstance.TextureBackgroundScaledNight.View.width();

	error_if(errored(::cho::grid_copy(applicationInstance.BitmapOffscreen.View, applicationInstance.TextureBackgroundScaledDay.View)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy(applicationInstance.BitmapOffscreen.View, applicationInstance.TextureBackgroundScaledNight.View, rectangle, rectangle)), "I believe this never fails.");

	error_if(errored(::cho::drawRectangle	(applicationInstance.BitmapOffscreen.View, (::cho::SColorBGRA)::cho::WHITE		, geometry0)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawRectangle	(applicationInstance.BitmapOffscreen.View, (::cho::SColorBGRA)::cho::BLUE		, ::cho::SRectangle2D<int32_t>{geometry0.Offset + ::cho::SCoord2<int32_t>{1, 1}, geometry0.Size - ::cho::SCoord2<int32_t>{2, 2}})	), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawCircle		(applicationInstance.BitmapOffscreen.View, (::cho::SColorBGRA)::cho::GREEN		, geometry1)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawCircle		(applicationInstance.BitmapOffscreen.View, (::cho::SColorBGRA)::cho::RED			, ::cho::SCircle2D<int32_t>{geometry1.Radius - 1, geometry1.Center})									), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawTriangle	(applicationInstance.BitmapOffscreen.View, (::cho::SColorBGRA)::cho::YELLOW		, geometry2)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(applicationInstance.BitmapOffscreen.View, (::cho::SColorBGRA)::cho::MAGENTA		, ::cho::SLine2D<int32_t>{geometry2.A, geometry2.B})													), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(applicationInstance.BitmapOffscreen.View, (::cho::SColorBGRA)::cho::WHITE		, ::cho::SLine2D<int32_t>{geometry2.B, geometry2.C})													), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(applicationInstance.BitmapOffscreen.View, (::cho::SColorBGRA)::cho::LIGHTGREEN	, ::cho::SLine2D<int32_t>{geometry2.C, geometry2.A})													), "Not sure if these functions could ever fail");
	return 0;
}
