#include "application.h"

static constexpr	const uint32_t														BMP_SCREEN_WIDTH							= 1280;
static constexpr	const uint32_t														BMP_SCREEN_HEIGHT							= uint32_t(::BMP_SCREEN_WIDTH * (9.0 / 16.0));
static				::RECT																minClientRect								= {100, 100, 100 + 128, 100 + 64};

extern				::SApplication														* g_ApplicationInstance						;

					LRESULT WINAPI														mainWndProc									(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)							{
	::SApplication																				& applicationInstance						= *g_ApplicationInstance;
	static	const int																			adjustedMinRect								= ::AdjustWindowRectEx(&minClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	::cho::SDisplay																				& mainDisplay								= applicationInstance.Framework.MainDisplay;
	::cho::SInput																				& input										= applicationInstance.Framework.Input;
	::cho::SDisplayPlatformDetail																& displayDetail								= applicationInstance.Framework.MainDisplay.PlatformDetail;
	switch(uMsg) {
	default: break;		
	case WM_CLOSE			: ::DestroyWindow(hWnd); return 0;
	case WM_KEYDOWN			: if(wParam > ::cho::size(input.KeyboardPrevious.KeyState)) break; input.KeyboardPrevious.KeyState[wParam] = input.KeyboardCurrent.KeyState[wParam]; input.KeyboardCurrent.KeyState[wParam]  =  1; return 0;
	case WM_KEYUP			: if(wParam > ::cho::size(input.KeyboardPrevious.KeyState)) break; input.KeyboardPrevious.KeyState[wParam] = input.KeyboardCurrent.KeyState[wParam]; input.KeyboardCurrent.KeyState[wParam] &= ~1; return 0;
	case WM_GETMINMAXINFO	:	// Catch this message so to prevent the window from becoming too small.
		((::MINMAXINFO*)lParam)->ptMinTrackSize													= {minClientRect.right - minClientRect.left, minClientRect.bottom - minClientRect.top}; 
		return 0;
	case WM_SIZE			: 
		if(lParam) {
			::cho::SCoord2<uint32_t>																	newMetrics									= ::cho::SCoord2<WORD>{LOWORD(lParam), HIWORD(lParam)}.Cast<uint32_t>();
			if(newMetrics != mainDisplay.Size) {
				mainDisplay.PreviousSize																= mainDisplay.Size;
				mainDisplay.Size																		= newMetrics;
				mainDisplay.Resized																		= true;
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
			mainDisplay.Resized																		= true;
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

					void																initWndClass								(::HINSTANCE hInstance, const TCHAR* className, ::WNDCLASSEX& wndClassToInit)	{
	wndClassToInit																			= {sizeof(::WNDCLASSEX),};
	wndClassToInit.lpfnWndProc																= ::mainWndProc;
	wndClassToInit.hInstance																= hInstance;
	wndClassToInit.hCursor																	= LoadCursor(NULL, IDC_ARROW);
	wndClassToInit.hbrBackground															= (::HBRUSH)(COLOR_3DFACE + 1);
	wndClassToInit.lpszClassName															= className;
}

					::cho::error_t														mainWindowCreate							(::cho::SDisplay& mainWindow, HINSTANCE hInstance)													{ 
	::cho::SDisplayPlatformDetail																& displayDetail								= mainWindow.PlatformDetail;
	::initWndClass(hInstance, displayDetail.WindowClassName, displayDetail.WindowClass);
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
