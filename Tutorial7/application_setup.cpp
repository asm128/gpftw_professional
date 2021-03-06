// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

static constexpr	const uint32_t														BMP_SCREEN_WIDTH							= 1280;
static constexpr	const uint32_t														BMP_SCREEN_HEIGHT							= uint32_t(::BMP_SCREEN_WIDTH * (9.0 / 16.0));
static				::RECT																minClientRect								= {100, 100, 100 + 320, 100 + 180};

extern				::SApplication														* g_ApplicationInstance						;

					LRESULT WINAPI														mainWndProc									(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)							{
	::SApplication																				& applicationInstance						= *g_ApplicationInstance;
	static	const int																			adjustedMinRect								= ::AdjustWindowRectEx(&minClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	::cho::SFramework																			& framework									= applicationInstance.Framework;
	::cho::SDisplay																				& mainDisplay								= framework.MainDisplay;
	::cho::SInput																				& input										= framework.Input;
	::cho::SDisplayPlatformDetail																& displayDetail								= framework.MainDisplay.PlatformDetail;
	switch(uMsg) {
	default: break;		
	case WM_CLOSE			: ::DestroyWindow(hWnd); return 0;
	case WM_KEYDOWN			: break_warn_if(wParam > ::cho::size(input.KeyboardPrevious.KeyState), "Key index out of range: %p.", (void*)wParam); input.KeyboardCurrent.KeyState[wParam]  =  1; return 0; // input.KeyboardPrevious.KeyState[wParam] = input.KeyboardCurrent.KeyState[wParam]; 
	case WM_KEYUP			: break_warn_if(wParam > ::cho::size(input.KeyboardPrevious.KeyState), "Key index out of range: %p.", (void*)wParam); input.KeyboardCurrent.KeyState[wParam] &= ~1; return 0; // input.KeyboardPrevious.KeyState[wParam] = input.KeyboardCurrent.KeyState[wParam]; 
	case WM_GETMINMAXINFO	:	// Catch this message so to prevent the window from becoming too small.
		((::MINMAXINFO*)lParam)->ptMinTrackSize													= {minClientRect.right - minClientRect.left, minClientRect.bottom - minClientRect.top}; 
		return 0;
	case WM_SIZE			: 
		if(lParam) {
			::cho::SCoord2<uint32_t>																	newMetrics									= ::cho::SCoord2<WORD>{LOWORD(lParam), HIWORD(lParam)}.Cast<uint32_t>();
			if(newMetrics != mainDisplay.Size) {
				mainDisplay.PreviousSize																= mainDisplay.Size;
				mainDisplay.Size																		= newMetrics;
				mainDisplay.Resized																		= 
				mainDisplay.Repaint																		= true; 
			}
		}
		if( wParam == SIZE_MINIMIZED ) {
			mainDisplay.MinOrMaxed = mainDisplay.NoDraw												= true;
		}
		else if( wParam == SIZE_MAXIMIZED ) {
			mainDisplay.Resized = mainDisplay.MinOrMaxed											= true;
			mainDisplay.NoDraw																		= false;
		}
		else if( wParam == SIZE_RESTORED ) {
			mainDisplay.Resized																		= 
			mainDisplay.MinOrMaxed																	= true;
			mainDisplay.NoDraw																		= false;
		}
		else {
			//State.Resized									= true;	? 
			mainDisplay.MinOrMaxed = mainDisplay.NoDraw												= false;
		}
		break;
	case WM_PAINT			: break;
	case WM_DESTROY			: 
		::PostQuitMessage(0); 
		displayDetail.WindowHandle																= 0;
		mainDisplay.Closed																		= true;
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

					::cho::error_t														mainWindowCreate							(::cho::SDisplay& mainWindow, HINSTANCE hInstance)													{ 
	::cho::SDisplayPlatformDetail																& displayDetail								= mainWindow.PlatformDetail;
	::cho::initWndClass(hInstance, displayDetail.WindowClassName, displayDetail.WindowClass, ::mainWndProc);
	::RegisterClassEx(&displayDetail.WindowClass);
	mainWindow.Size																			= {::BMP_SCREEN_WIDTH, ::BMP_SCREEN_HEIGHT};
	::RECT																						finalClientRect								= {100, 100, 100 + (LONG)mainWindow.Size.x, 100 + (LONG)mainWindow.Size.y};
	::AdjustWindowRectEx(&finalClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
	mainWindow.PlatformDetail.WindowHandle													= ::CreateWindowEx(0, displayDetail.WindowClassName, TEXT("Window Sugar"), WS_OVERLAPPEDWINDOW
		, finalClientRect.left
		, finalClientRect.top
		, finalClientRect.right		- finalClientRect.left
		, finalClientRect.bottom	- finalClientRect.top
		, 0, 0, displayDetail.WindowClass.hInstance, 0
		);
	::ShowWindow	(displayDetail.WindowHandle, SW_SHOW);
	::UpdateWindow	(displayDetail.WindowHandle);
	return 0;
}
