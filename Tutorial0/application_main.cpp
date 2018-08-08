// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "cho_bitmap_target.h"
#include "cho_bitmap_file.h"
#include "cho_grid_copy.h"


#include "cho_app_impl.h"

static constexpr	const uint32_t										ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t										ASCII_SCREEN_HEIGHT							= 50	;

CHO_DEFINE_APPLICATION_ENTRY_POINT(::SApplication);	

					::SApplication										* g_ApplicationInstance						= 0;

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
	static constexpr	const char												bmpFileName0	[]									= "..\\gpk_data\\images\\earth_color.bmp";
	static constexpr	const char												bmpFileName1	[]									= "..\\gpk_data\\images\\earth_light.bmp";
	static constexpr	const char												bmpFileName2	[]									= "..\\gpk_data\\images\\Codepage-437-24.bmp";
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName0), applicationInstance.TextureBackgroundDay	)), "Failed to load bitmap from file: %s.", bmpFileName0);
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName1), applicationInstance.TextureBackgroundNight	)), "Failed to load bitmap from file: %s.", bmpFileName1);
	error_if(errored(::cho::bmpFileLoad(::cho::view_const_string(bmpFileName2), applicationInstance.TextureFont				)), "Failed to load bitmap from file: %s.", bmpFileName2);
	ree_if	(errored(::cho::updateSizeDependentTarget(applicationInstance.Framework.Offscreen, applicationInstance.Framework.MainDisplay.Size)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	return 0;
}

					::cho::error_t										updateInput									(::SApplication& applicationInstance)											{ 
	::cho::SInput																& systemInput								= applicationInstance.Framework.Input;
	if(systemInput.KeyUp	('W')) ::Beep(440, 100);
	if(systemInput.KeyUp	('A')) ::Beep(520, 100);
	if(systemInput.KeyUp	('S')) ::Beep(600, 100);
	if(systemInput.KeyUp	('D')) ::Beep(680, 100);
	if(systemInput.KeyDown	('W')) ::Beep(760, 100);
	if(systemInput.KeyDown	('A')) ::Beep(840, 100);
	if(systemInput.KeyDown	('S')) ::Beep(920, 100);
	if(systemInput.KeyDown	('D')) ::Beep(1000, 100);
	return 0;

}

					::cho::error_t										update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");

	::cho::error_t																frameworkResult								= ::cho::updateFramework(applicationInstance.Framework);
	ree_if	(errored(frameworkResult), "Unknown error.");
	rvi_if	(1, frameworkResult == 1, "Framework requested close. Terminating execution.");
	ree_if	(errored(::cho::updateSizeDependentTarget	(applicationInstance.Framework.Offscreen, applicationInstance.Framework.MainDisplay.Size)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	error_if(errored(::updateInput						(applicationInstance)), "Unknown error.");

	::cho::STimer																& timer										= applicationInstance.Framework.Timer;
	::cho::SDisplay																& mainWindow								= applicationInstance.Framework.MainDisplay;

	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}
					::cho::error_t										drawTextFixedSize						(::cho::grid_view<::cho::SColorBGRA>& bmpTarget, const ::cho::grid_view<::cho::SColorBGRA>& viewTextureFont, uint32_t characterCellsX, int32_t dstOffsetY, const ::cho::SCoord2<int32_t>& sizeCharCell, const ::cho::view_const_string& text0, const ::cho::SCoord2<int32_t> dstTextOffset)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	for(int32_t iChar = 0, charCount = (int32_t)text0.size(); iChar < charCount; ++iChar) {
		int32_t																	coordTableX								= text0[iChar] % characterCellsX;
		int32_t																	coordTableY								= text0[iChar] / characterCellsX;
		const ::cho::SCoord2<int32_t>											coordCharTable							= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
		const ::cho::SCoord2<int32_t>											dstOffset1								= {sizeCharCell.x * iChar, dstOffsetY};
		const ::cho::SRectangle2D<int32_t>										srcRect0								= ::cho::SRectangle2D<int32_t>{{coordCharTable.x, (int32_t)viewTextureFont.height() - sizeCharCell.y - coordCharTable.y}, sizeCharCell};
		error_if(errored(::cho::grid_copy(bmpTarget, viewTextureFont, dstTextOffset + dstOffset1, srcRect0)), "I believe this never fails.");
	}
	return 0;
}

					::cho::error_t										draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::STexture<::cho::SColorBGRA>											& offscreen									= framework.Offscreen;
	::cho::SFrameInfo															& frameInfo									= framework.FrameInfo;
	const ::cho::SDisplay														& mainWindow								= framework.MainDisplay;
	::cho::SCoord2		<int32_t>												screenCenter								= {(int32_t)mainWindow.Size.x / 2, (int32_t)mainWindow.Size.y / 2};
	::cho::SRectangle2D	<int32_t>												geometry0									= {{2, 2}, {(int32_t)((frameInfo.FrameNumber / 2) % (mainWindow.Size.x - 2)), 5}};
	::cho::SCircle2D	<int32_t>												geometry1									= {5.0 + (frameInfo.FrameNumber / 5) % 5, screenCenter};	
	::cho::STriangle2D	<int32_t>												geometry2									= {{0, 0}, {15, 15}, {-5, 10}};	
	geometry2.A																+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.B																+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.C																+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};

	const ::cho::SCoord2<uint32_t>												dstOffsetNight								= {mainWindow.Size.x / 2 - applicationInstance.TextureBackgroundNight	.View.width() / 2, mainWindow.Size.y / 2 - applicationInstance.TextureBackgroundNight	.View.height() / 2};
	const ::cho::SCoord2<uint32_t>												dstOffsetDay								= {mainWindow.Size.x / 2 - applicationInstance.TextureBackgroundDay		.View.width() / 2, mainWindow.Size.y / 2 - applicationInstance.TextureBackgroundDay		.View.height() / 2};
	error_if(errored(::cho::grid_copy		(offscreen.View, applicationInstance.TextureBackgroundDay.View, dstOffsetDay)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy		(offscreen.View, applicationInstance.TextureBackgroundNight.View, dstOffsetNight)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy		(offscreen.View, applicationInstance.TextureFont.View, dstOffsetNight, ::cho::SRectangle2D<uint32_t>{{0,0}, {9, 128}})), "I believe this never fails.");
	error_if(errored(::cho::drawRectangle	(offscreen.View, (::cho::SColorBGRA)::cho::WHITE		, geometry0)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawRectangle	(offscreen.View, (::cho::SColorBGRA)::cho::BLUE			, ::cho::SRectangle2D<int32_t>{geometry0.Offset + ::cho::SCoord2<int32_t>{1, 1}, geometry0.Size - ::cho::SCoord2<int32_t>{2, 2}})	), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawCircle		(offscreen.View, (::cho::SColorBGRA)::cho::GREEN		, geometry1)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawCircle		(offscreen.View, (::cho::SColorBGRA)::cho::RED			, ::cho::SCircle2D<int32_t>{geometry1.Radius - 1, geometry1.Center})									), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawTriangle	(offscreen.View, (::cho::SColorBGRA)::cho::YELLOW		, geometry2)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(offscreen.View, (::cho::SColorBGRA)::cho::MAGENTA		, ::cho::SLine2D<int32_t>{geometry2.A, geometry2.B})													), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(offscreen.View, (::cho::SColorBGRA)::cho::WHITE		, ::cho::SLine2D<int32_t>{geometry2.B, geometry2.C})													), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(offscreen.View, (::cho::SColorBGRA)::cho::LIGHTGREEN	, ::cho::SLine2D<int32_t>{geometry2.C, geometry2.A})													), "Not sure if these functions could ever fail");

	static constexpr const ::cho::SCoord2<int32_t>								sizeCharCell								= {9, 16};
	uint32_t																	lineOffset									= 0;
	{
		static constexpr const char													testText0	[]								= "Some";
		static constexpr const uint32_t												sizeLine									= sizeCharCell.x * ::cho::size(testText0) - 1;
		const ::cho::SCoord2<int32_t>												dstTextOffset								= {(int32_t)mainWindow.Size.x / 2 - (int32_t)sizeLine / 2, };
		for(int32_t iChar = 0; iChar < (int32_t)::cho::size(testText0) - 1; ++iChar) {
			int32_t																			coordTableX								= testText0[iChar] % 32;
			int32_t																			coordTableY								= testText0[iChar] / 32;
			const ::cho::SCoord2<int32_t>													coordCharTable							= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
			const ::cho::SRectangle2D<int32_t>												dstRect0								= ::cho::SRectangle2D<int32_t>{{sizeCharCell.x * iChar, (int32_t)(mainWindow.Size.y - lineOffset * sizeCharCell.y - sizeCharCell.y)}, sizeCharCell};
			const ::cho::SRectangle2D<int32_t>												srcRect0								= ::cho::SRectangle2D<int32_t>{{coordCharTable.x, (int32_t)applicationInstance.TextureFont.View.height() - sizeCharCell.y - coordCharTable.y}, sizeCharCell};
			error_if(errored(::cho::grid_copy(offscreen.View, applicationInstance.TextureFont.View, {dstTextOffset + dstRect0.Offset, dstRect0.Size}, srcRect0)), "I believe this never fails.");
		}
	}
	++lineOffset;
	{
		static constexpr const char														testText0	[]							= "Text";
		const uint32_t																	sizeLine								= sizeCharCell.x * ::cho::size(testText0) - 1;
		const ::cho::SCoord2<int32_t>													dstTextOffset							= {(int32_t)mainWindow.Size.x / 2 - (int32_t)sizeLine / 2, };
		for(int32_t iChar = 0; iChar < (int32_t)::cho::size(testText0) - 1; ++iChar) {
			int32_t																			coordTableX								= testText0[iChar] % 32;
			int32_t																			coordTableY								= testText0[iChar] / 32;
			const ::cho::SCoord2<int32_t>													coordCharTable							= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
			const ::cho::SCoord2<int32_t>													dstOffset1								= {sizeCharCell.x * iChar, (int32_t)(mainWindow.Size.y - lineOffset * sizeCharCell.y - sizeCharCell.y)};
			const ::cho::SRectangle2D<int32_t>												srcRect0								= ::cho::SRectangle2D<int32_t>{{coordCharTable.x, (int32_t)applicationInstance.TextureFont.View.height() - sizeCharCell.y - coordCharTable.y}, sizeCharCell};
			error_if(errored(::cho::grid_copy(offscreen.View, applicationInstance.TextureFont.View, dstTextOffset + dstOffset1, srcRect0)), "I believe this never fails.");
		}
	}
	++lineOffset;
	{
		static constexpr const char														testText0	[]							= "Lines";
		const uint32_t																	sizeLine								= sizeCharCell.x * ::cho::size(testText0) - 1;
		const ::cho::SCoord2<int32_t>													dstTextOffset							= {(int32_t)mainWindow.Size.x / 2 - (int32_t)sizeLine / 2, };
		uint32_t																		dstOffsetY								= (int32_t)(mainWindow.Size.y - lineOffset * sizeCharCell.y - sizeCharCell.y);
		drawTextFixedSize(offscreen.View, applicationInstance.TextureFont.View, 32, dstOffsetY, sizeCharCell, {testText0, ::cho::size(testText0) -1}, dstTextOffset);
	}
	++lineOffset;

	const ::cho::SRectangle2D<uint32_t>												dstRect0								= ::cho::SRectangle2D<uint32_t>{{ 9,  16}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>												dstRect1								= ::cho::SRectangle2D<uint32_t>{{18,  32}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>												dstRect2								= ::cho::SRectangle2D<uint32_t>{{27,  48}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>												dstRect3								= ::cho::SRectangle2D<uint32_t>{{36,  64}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>												dstRect4								= ::cho::SRectangle2D<uint32_t>{{45,  80}, sizeCharCell.Cast<uint32_t>()};
	
	const ::cho::SRectangle2D<uint32_t>												srcRect0								= ::cho::SRectangle2D<uint32_t>{{ 9,  applicationInstance.TextureFont.View.height() - 16}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>												srcRect1								= ::cho::SRectangle2D<uint32_t>{{18,  applicationInstance.TextureFont.View.height() - 16}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>												srcRect2								= ::cho::SRectangle2D<uint32_t>{{27,  applicationInstance.TextureFont.View.height() - 16}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>												srcRect3								= ::cho::SRectangle2D<uint32_t>{{36,  applicationInstance.TextureFont.View.height() - 16}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>												srcRect4								= ::cho::SRectangle2D<uint32_t>{{45,  applicationInstance.TextureFont.View.height() - 16}, sizeCharCell.Cast<uint32_t>()};
	error_if(errored(::cho::grid_copy(offscreen.View, applicationInstance.TextureFont.View, dstRect0, srcRect0.Offset)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy(offscreen.View, applicationInstance.TextureFont.View, dstRect1.Offset, srcRect1.Offset)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy(offscreen.View, applicationInstance.TextureFont.View, ::cho::SCoord2<uint32_t>{mainWindow.Size.x - applicationInstance.TextureFont.View.width() / 2, dstRect1.Offset.y}, srcRect1.Offset)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy(offscreen.View, applicationInstance.TextureFont.View, dstRect2.Offset, srcRect2)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy(offscreen.View, applicationInstance.TextureFont.View, dstRect3, srcRect3)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy(offscreen.View, applicationInstance.TextureFont.View, dstRect4, srcRect4)), "I believe this never fails.");
	return 0;
}
