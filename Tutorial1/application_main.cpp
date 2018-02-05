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
	const ::cho::SCoord2<uint32_t>												newSize										= applicationInstance.Framework.MainDisplay.Size; // / 2;
	::cho::updateSizeDependentTarget	(applicationInstance.Framework.Offscreen				, newSize);
	::cho::updateSizeDependentTexture	(applicationInstance.TextureBackgroundScaledDay			, applicationInstance.TextureBackgroundDay		.View, newSize);
	::cho::updateSizeDependentTexture	(applicationInstance.TextureBackgroundScaledNight		, applicationInstance.TextureBackgroundNight	.View, newSize);
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
	static constexpr	const char												bmpFileName0	[]									= "earth_color.bmp";
	static constexpr	const char												bmpFileName1	[]									= "earth_light.bmp";
	static constexpr	const char												bmpFileName2	[]									= "Codepage-437-24.bmp";
	error_if(errored(::cho::bmpFileLoad((::cho::view_const_string)bmpFileName0, applicationInstance.TextureBackgroundDay	)), "Failed to load bitmap from file: %s.", bmpFileName0);
	error_if(errored(::cho::bmpFileLoad((::cho::view_const_string)bmpFileName1, applicationInstance.TextureBackgroundNight	)), "Failed to load bitmap from file: %s.", bmpFileName1);
	error_if(errored(::cho::bmpFileLoad((::cho::view_const_string)bmpFileName2, applicationInstance.TextureFont				)), "Failed to load bitmap from file: %s.", bmpFileName2);

	{
		uint8_t																		testBitfield[]									= {1,3,7,15,31,63,127,255};
		::cho::bit_array_view<uint8_t>												testView										= testBitfield;
		for(uint32_t i=0; i < testView.size(); ++i)
			printf("%X", (uint32_t)testView[i]);
		printf("\n");
		for(uint32_t i=0; i < testView.size(); ++i) {
			testView[i]																= i % 2;
			printf("%X", (uint32_t)testView[i]);
		}
		printf("\n");
		for(uint32_t i=0; i < testView.size(); ++i)
			printf("%X", (uint32_t)testView[i]);
		printf("\n");
	}
	printf("\n");
	{
		uint8_t																		testBitfield2[]									= {1,3,7,15,31,63,127,255};
		::cho::bit_array_view<uint8_t>												testView2										= testBitfield2;
		for(::cho::bit_array_view_iterator<uint8_t> item = testView2.begin(); item != testView2.end(); item++) {
			bool																		value											= item;
			printf("%X", (uint32_t)value);
		}
		printf("\n");
		for(::cho::bit_array_view_iterator<uint8_t> item = testView2.begin(); item != testView2.end(); ++item) {
			item																	= 1;
			bool																		value											= item;
			printf("%X", (uint32_t)value);
		}
		printf("\n");
		for(::cho::bit_array_view_iterator<uint8_t> item = testView2.begin(); item != testView2.end(); ++item) {
			bool																		value											= item;
			printf("%X", (uint32_t)value);
		}
		printf("\n");
	}
	return 0;
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
	return 0;

}

					::cho::error_t										update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");

	uint32_t																	frameworkResult								= ::cho::updateFramework(applicationInstance.Framework);
	ree_if	(errored(frameworkResult), "Unknown error.");
	rvi_if	(1, frameworkResult == 1, "Framework requested close. Terminating execution.");
	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	error_if(errored(::updateInput					(applicationInstance)), "Unknown error.");

	::cho::STimer																& timer										= applicationInstance.Framework.Timer;
	::cho::SDisplay																& mainWindow								= applicationInstance.Framework.MainDisplay;

	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

//static				::cho::error_t										textCalcSizeLine						(const ::cho::SCoord2<int32_t>& sizeCharCell, const ::cho::view_const_string& text0)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
//	return (::cho::error_t)(text0.size() ? sizeCharCell.x * strlen(text0.begin()) : 0);
//}

template<size_t _sizeString>
static				::cho::error_t										textCalcSizeLine						(const ::cho::SCoord2<int32_t>& sizeCharCell, const char (&text0)[_sizeString] )	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	return (::cho::error_t)(sizeCharCell.x * ::cho::size(text0) - 1);
}

static				::cho::error_t										textDrawFixedSize						(::cho::grid_view<::cho::SColorBGRA>& bmpTarget, const ::cho::grid_view<::cho::SColorBGRA>& viewTextureFont, uint32_t characterCellsX, int32_t dstOffsetY, const ::cho::SCoord2<int32_t>& sizeCharCell, const ::cho::view_const_string& text0, const ::cho::SCoord2<int32_t> dstTextOffset)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
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
	::cho::grid_view	<::cho::SColorBGRA>										& bmpOffscreen								= offscreen.View;
	const::cho::SCoord2	<int32_t>												screenCenter								= {(int32_t)mainWindow.Size.x / 2, (int32_t)mainWindow.Size.y / 2};
	::cho::SRectangle2D	<int32_t>												geometry0									= {{2, 2}, {(int32_t)((frameInfo.FrameNumber / 2) % (mainWindow.Size.x - 2)), 5}};
	::cho::SCircle2D	<int32_t>												geometry1									= {5.0 + (frameInfo.FrameNumber / 5) % 5, screenCenter};	
	::cho::STriangle2D	<int32_t>												geometry2									= {{0, 0}, {15, 15}, {-5, 10}};	
	geometry2.A																+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.B																+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.C																+= screenCenter + ::cho::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};

	::cho::SRectangle2D	<uint32_t>												rectangle									= {};
	rectangle.Offset.x														= mainWindow.Size.x / 4;
	rectangle.Offset.y														= mainWindow.Size.y / 4;
	rectangle.Size	.x														= mainWindow.Size.x - rectangle.Offset.x * 2;
	rectangle.Size	.y														= mainWindow.Size.y - rectangle.Offset.y * 2;
	::memset(bmpOffscreen.begin(), 0, sizeof(::cho::SColorBGRA) * bmpOffscreen.size());	// Clear target.
	error_if(errored(::cho::grid_copy(bmpOffscreen, applicationInstance.TextureBackgroundScaledDay	.View)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy(bmpOffscreen, applicationInstance.TextureBackgroundScaledNight.View, rectangle, rectangle)), "I believe this never fails.");
	//error_if(errored(::cho::grid_copy(bmpTarget.Colors, applicationInstance.ViewTextureFont)), "I believe this never fails.");
	error_if(errored(::cho::drawRectangle	(bmpOffscreen, (::cho::SColorBGRA)::cho::WHITE		, geometry0)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawRectangle	(bmpOffscreen, (::cho::SColorBGRA)::cho::BLUE		, ::cho::SRectangle2D<int32_t>{geometry0.Offset + ::cho::SCoord2<int32_t>{1, 1}, geometry0.Size - ::cho::SCoord2<int32_t>{2, 2}})	), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawCircle		(bmpOffscreen, (::cho::SColorBGRA)::cho::GREEN		, geometry1)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawCircle		(bmpOffscreen, (::cho::SColorBGRA)::cho::RED		, ::cho::SCircle2D<int32_t>{geometry1.Radius - 1, geometry1.Center})									), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawTriangle	(bmpOffscreen, (::cho::SColorBGRA)::cho::YELLOW		, geometry2)																							), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(bmpOffscreen, (::cho::SColorBGRA)::cho::MAGENTA	, ::cho::SLine2D<int32_t>{geometry2.A, geometry2.B})													), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(bmpOffscreen, (::cho::SColorBGRA)::cho::WHITE		, ::cho::SLine2D<int32_t>{geometry2.B, geometry2.C})													), "Not sure if these functions could ever fail");
	error_if(errored(::cho::drawLine		(bmpOffscreen, (::cho::SColorBGRA)::cho::LIGHTGREEN	, ::cho::SLine2D<int32_t>{geometry2.C, geometry2.A})													), "Not sure if these functions could ever fail");

	static constexpr const ::cho::SCoord2<int32_t>								sizeCharCell								= {9, 16};
	uint32_t																	lineOffset									= 0;
	{
		static constexpr const char													testText0	[]								= "Some";
		const ::cho::SCoord2<int32_t>												dstTextOffset								= {(int32_t)mainWindow.Size.x / 2 - (int32_t)textCalcSizeLine(sizeCharCell, testText0) / 2, };
		uint32_t																	dstOffsetY									= (int32_t)(mainWindow.Size.y - lineOffset * sizeCharCell.y - sizeCharCell.y);
		textDrawFixedSize(bmpOffscreen, applicationInstance.TextureFont.View, 32, dstOffsetY, sizeCharCell, {testText0, ::cho::size(testText0) -1}, dstTextOffset);
	}
	++lineOffset;
	{
		static constexpr const char													testText0	[]								= "Text";
		const ::cho::SCoord2<int32_t>												dstTextOffset								= {(int32_t)mainWindow.Size.x / 2 - (int32_t)textCalcSizeLine(sizeCharCell, testText0) / 2, };
		uint32_t																	dstOffsetY									= (int32_t)(mainWindow.Size.y - lineOffset * sizeCharCell.y - sizeCharCell.y);
		textDrawFixedSize(bmpOffscreen, applicationInstance.TextureFont.View, 32, dstOffsetY, sizeCharCell, {testText0, ::cho::size(testText0) -1}, dstTextOffset);
	}
	++lineOffset;
	{
		static constexpr const char													testText0	[]								= "Lines";
		const ::cho::SCoord2<int32_t>												dstTextOffset								= {(int32_t)mainWindow.Size.x / 2 - (int32_t)textCalcSizeLine(sizeCharCell, testText0) / 2, };
		uint32_t																	dstOffsetY									= (int32_t)(mainWindow.Size.y - lineOffset * sizeCharCell.y - sizeCharCell.y);
		textDrawFixedSize(bmpOffscreen, applicationInstance.TextureFont.View, 32, dstOffsetY, sizeCharCell, {testText0, ::cho::size(testText0) -1}, dstTextOffset);
	}
	++lineOffset;

	const ::cho::SRectangle2D<uint32_t>											dstRect0									= ::cho::SRectangle2D<uint32_t>{{ 9,  16}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>											dstRect1									= ::cho::SRectangle2D<uint32_t>{{18,  32}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>											dstRect2									= ::cho::SRectangle2D<uint32_t>{{27,  48}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>											dstRect3									= ::cho::SRectangle2D<uint32_t>{{36,  64}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>											dstRect4									= ::cho::SRectangle2D<uint32_t>{{45,  80}, sizeCharCell.Cast<uint32_t>()};
	
	const ::cho::SRectangle2D<uint32_t>											srcRect0									= ::cho::SRectangle2D<uint32_t>{{ 9,  applicationInstance.TextureFont.View.height() - 16}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>											srcRect1									= ::cho::SRectangle2D<uint32_t>{{18,  applicationInstance.TextureFont.View.height() - 16}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>											srcRect2									= ::cho::SRectangle2D<uint32_t>{{27,  applicationInstance.TextureFont.View.height() - 16}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>											srcRect3									= ::cho::SRectangle2D<uint32_t>{{36,  applicationInstance.TextureFont.View.height() - 16}, sizeCharCell.Cast<uint32_t>()};
	const ::cho::SRectangle2D<uint32_t>											srcRect4									= ::cho::SRectangle2D<uint32_t>{{45,  applicationInstance.TextureFont.View.height() - 16}, sizeCharCell.Cast<uint32_t>()};
	error_if(errored(::cho::grid_copy(bmpOffscreen, applicationInstance.TextureFont.View, dstRect0, srcRect0.Offset)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy(bmpOffscreen, applicationInstance.TextureFont.View, dstRect1.Offset, srcRect1.Offset)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy(bmpOffscreen, applicationInstance.TextureFont.View, ::cho::SCoord2<uint32_t>{mainWindow.Size.x - applicationInstance.TextureFont.View.width() / 2, dstRect1.Offset.y}, srcRect1.Offset)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy(bmpOffscreen, applicationInstance.TextureFont.View, dstRect2.Offset, srcRect2)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy(bmpOffscreen, applicationInstance.TextureFont.View, dstRect3, srcRect3)), "I believe this never fails.");
	error_if(errored(::cho::grid_copy(bmpOffscreen, applicationInstance.TextureFont.View, dstRect4, srcRect4)), "I believe this never fails.");
	return 0;
}
