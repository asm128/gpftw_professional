#include "cho_framework.h"

					::cho::error_t										cho::updateFramework						(::cho::SFramework& framework)													{
	framework.SystemInput.KeyboardPrevious											= framework.SystemInput.KeyboardCurrent;
	framework.SystemInput.MousePrevious												= framework.SystemInput.MouseCurrent;
	::cho::SFrameInfo															& frameInfo									= framework.FrameInfo;
	::cho::STimer																& timer										= framework.Timer;
	timer		.Frame();
	frameInfo	.Frame(timer.LastTimeMicroseconds);
	::cho::SDisplay																& mainWindow								= framework.MainDisplay;
	ree_if(errored(::cho::displayUpdate(mainWindow)), "Not sure why this would fail.");
	rvi_if(1, 0 == mainWindow.PlatformDetail.WindowHandle, "Application exiting because the main window was closed.");
	::cho::SFramework::TOffscreen												& offscreen									= framework.Offscreen;
	error_if(errored(::cho::displayPresentTarget(mainWindow, offscreen.View)), "Unknown error.");
	return 0;
}
