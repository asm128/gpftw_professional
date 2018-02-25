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
	return 0;
}

					::cho::error_t										mainWindowCreate							(::cho::SDisplay& mainWindow, HINSTANCE hInstance);
					::cho::error_t										setup										(::SApplication& applicationInstance)											{ 
	g_ApplicationInstance													= &applicationInstance;
	::cho::SDisplay																& mainWindow								= applicationInstance.Framework.MainDisplay;
	error_if(errored(::mainWindowCreate(mainWindow, applicationInstance.Framework.RuntimeValues.PlatformDetail.EntryPointArgs.hInstance)), "Failed to create main window why?????!?!?!?!?");
	static constexpr	const char												bmpFileName2	[]									= "Codepage-437-24.bmp";
	error_if(errored(::cho::bmpFileLoad((::cho::view_const_string)bmpFileName2, applicationInstance.TextureFont				)), "Failed to load bitmap from file: %s.", bmpFileName2);
	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	return 0;
}

					::cho::error_t										update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	::cho::error_t																frameworkResult								= ::cho::updateFramework(applicationInstance.Framework);
	ree_if	(errored(frameworkResult), "Unknown error.");
	rvi_if	(1, frameworkResult == 1, "Framework requested close. Terminating execution.");
	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and this could cause an invalid memory access later on.");
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
	}
	return 0;
}

template<size_t _sizeString>
static				::cho::error_t										textDrawAlignedFixedSize					(::cho::grid_view<::cho::SColorBGRA>& targetView, const ::cho::grid_view<::cho::SColorBGRA>& fontAtlas, uint32_t lineOffset, const ::cho::SCoord2<uint32_t>& targetSize, const ::cho::SCoord2<int32_t>& sizeCharCell, const char (&text0)[_sizeString] )	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::cho::SCoord2<int32_t>												dstTextOffset								= {(int32_t)targetSize.x / 2 - (int32_t)textCalcSizeLine(sizeCharCell, text0) / 2, };
	uint32_t																	dstOffsetY									= (int32_t)(targetSize.y - lineOffset * sizeCharCell.y - sizeCharCell.y);
	return ::textDrawFixedSize(targetView, fontAtlas, 32, dstOffsetY, sizeCharCell, {text0, ::cho::size(text0) -1}, dstTextOffset);
}

					::cho::error_t										draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::SFramework::TOffscreen												& offscreen									= framework.Offscreen;
	::memset(offscreen.Texels.begin(), 0, sizeof(::cho::SFramework::TOffscreen::TTexel) * offscreen.Texels.size());	// Clear target.
	static constexpr const ::cho::SCoord2<int32_t>								sizeCharCell								= {9, 16};
	uint32_t																	lineOffset									= 0;
	static constexpr const char													textLine0	[]								= "W: Up, S: Down, A: Left, D: Right";
	static constexpr const char													textLine1	[]								= "T: Shoot. Y: Thrust. U: Handbrake.";
	static constexpr const char													textLine2	[]								= "Press ESC to exit.";
	::textDrawAlignedFixedSize(offscreen.View, applicationInstance.TextureFont.View, lineOffset, {offscreen.View.width(), offscreen.View.height()}, sizeCharCell, textLine0);	++lineOffset;
	::textDrawAlignedFixedSize(offscreen.View, applicationInstance.TextureFont.View, lineOffset, {offscreen.View.width(), offscreen.View.height()}, sizeCharCell, textLine1);	++lineOffset;
	::textDrawAlignedFixedSize(offscreen.View, applicationInstance.TextureFont.View, lineOffset = offscreen.View.height() / 16 - 1, {offscreen.View.width(), offscreen.View.height()}, sizeCharCell, textLine2);	--lineOffset;
	return 0;
}
