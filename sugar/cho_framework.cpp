// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "cho_framework.h"

#if defined(CHO_WINDOWS)
#	include <Windows.h>
#endif

					::cho::error_t										cho::updateFramework						(::cho::SFramework& framework)													{
	framework.Input.KeyboardPrevious											= framework.Input.KeyboardCurrent;
	framework.Input.MousePrevious												= framework.Input.MouseCurrent;
	::cho::SFrameInfo															& frameInfo									= framework.FrameInfo;
	::cho::STimer																& timer										= framework.Timer;
	timer		.Frame();
	frameInfo	.Frame(::cho::min(timer.LastTimeMicroseconds, 200000ULL));
	::cho::SDisplay																& mainWindow								= framework.MainDisplay;
	ree_if(errored(::cho::displayUpdate(mainWindow)), "Not sure why this would fail.");
	rvi_if(1, 0 == mainWindow.PlatformDetail.WindowHandle, "Application exiting because the main window was closed.");
	::cho::SFramework::TOffscreen												& offscreen									= framework.Offscreen;
	error_if(errored(::cho::displayPresentTarget(mainWindow, offscreen.View)), "Unknown error.");
#if defined(CHO_WINDOWS)
	Sleep(1);
#endif
	return 0;
}

