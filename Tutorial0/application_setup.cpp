#include "application.h"

static constexpr	const uint32_t																	BMP_SCREEN_WIDTH							= 512;
static constexpr	const uint32_t																	BMP_SCREEN_HEIGHT							= uint32_t(::BMP_SCREEN_WIDTH * (9.0 / 16.0));
static				::RECT																			minClientRect								= {100, 100, 100 + 320, 100 + 200};

extern				::SApplication																	* g_ApplicationInstance						;

					LRESULT WINAPI																	mainWndProc									(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)							{
	::SApplication																							& applicationInstance						= *g_ApplicationInstance;
	static	const int																						adjustedMinRect								= ::AdjustWindowRectEx(&minClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	::cho::SDisplayPlatformDetail																			& displayDetail								= applicationInstance.MainDisplay.PlatformDetail;
	switch(uMsg) {
	default: break;		
	case WM_CLOSE			: ::DestroyWindow(hWnd); return 0;
	case WM_GETMINMAXINFO	:	// Catch this message so to prevent the window from becoming too small.
		((::MINMAXINFO*)lParam)->ptMinTrackSize																= {minClientRect.right - minClientRect.left, minClientRect.bottom - minClientRect.top}; 
		return 0;
	case WM_SIZE			: 
		if(lParam) {
			::cho::SCoord2<int32_t>																					newMetrics									= ::cho::SCoord2<WORD>{LOWORD(lParam), HIWORD(lParam)}.Cast<int32_t>();
			if(newMetrics != applicationInstance.MainDisplay.Size.Cast<int32_t>()) {
				applicationInstance.MainDisplay.PreviousSize														= applicationInstance.MainDisplay.Size;
				applicationInstance.MainDisplay.Size																= newMetrics.Cast<uint32_t>();
				applicationInstance.MainDisplay.Resized																= true;
				applicationInstance.MainDisplay.Repaint																= true; 
				//::updateOffscreen	(applicationInstance);
				//error_if(errored(::draw							(applicationInstance)), "Not sure why these would fail.");
				//error_if(errored(::cho::displayPresentTarget	(applicationInstance.MainDisplay, applicationInstance.BitmapOffsceen)), "Not sure why these would fail.");
			}
		}
		if( wParam == SIZE_MINIMIZED ) {
			applicationInstance.MainDisplay.MinOrMaxed = applicationInstance.MainDisplay.NoDraw					= true;
		}
		else if( wParam == SIZE_MAXIMIZED ) {
			applicationInstance.MainDisplay.Resized = applicationInstance.MainDisplay.MinOrMaxed				= true;
			applicationInstance.MainDisplay.NoDraw																= false;
		}
		else if( wParam == SIZE_RESTORED ) {
			applicationInstance.MainDisplay.Resized																= true;
			applicationInstance.MainDisplay.MinOrMaxed															= true;
			applicationInstance.MainDisplay.NoDraw																= false;
		}
		else {
			//State.Resized									= true;	? 
			applicationInstance.MainDisplay.MinOrMaxed = applicationInstance.MainDisplay.NoDraw					= false;
		}
		break;
	case WM_PAINT			: break;
	case WM_DESTROY			: 
		::PostQuitMessage(0); 
		displayDetail.WindowHandle																			= 0;
		applicationInstance.MainDisplay.Closed																= true;
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

					void																			initWndClass								(::HINSTANCE hInstance, const TCHAR* className, ::WNDCLASSEX& wndClassToInit)	{
	wndClassToInit																						= {sizeof(::WNDCLASSEX),};
	wndClassToInit.lpfnWndProc																			= ::mainWndProc;
	wndClassToInit.hInstance																			= hInstance;
	wndClassToInit.hCursor																				= LoadCursor(NULL, IDC_ARROW);
	wndClassToInit.hbrBackground																		= (::HBRUSH)(COLOR_3DFACE + 1);
	wndClassToInit.lpszClassName																		= className;
}

					::cho::error_t																	mainWindowCreate							(::cho::SDisplay& mainWindow, HINSTANCE hInstance)													{ 
	::cho::SDisplayPlatformDetail																			& displayDetail								= mainWindow.PlatformDetail;
	::initWndClass(hInstance, displayDetail.WindowClassName, displayDetail.WindowClass);
	::RegisterClassEx(&displayDetail.WindowClass);
	mainWindow.Size																						= {::BMP_SCREEN_WIDTH, ::BMP_SCREEN_HEIGHT};
	::RECT																									finalClientRect								= {100, 100, 100 + (LONG)mainWindow.Size.x, 100 + (LONG)mainWindow.Size.y};
	::AdjustWindowRectEx(&finalClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
	mainWindow.PlatformDetail.WindowHandle																= ::CreateWindowEx(0, displayDetail.WindowClassName, TEXT("Window Sugar"), WS_OVERLAPPEDWINDOW
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
