// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"
#include "cho_bitmap_target.h"
#include "cho_app_impl.h"

static constexpr	const uint32_t																	BMP_SCREEN_WIDTH							= 512;
static constexpr	const uint32_t																	BMP_SCREEN_HEIGHT							= uint32_t(::BMP_SCREEN_WIDTH * (9.0 / 16.0));
static constexpr	const uint32_t																	ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t																	ASCII_SCREEN_HEIGHT							= 50	;

CHO_DEFINE_APPLICATION_ENTRY_POINT(::SApplication);	

static				::SApplication																	* g_ApplicationInstance						= 0;

		void																						updateOffscreen								(::SApplication& applicationInstance)											{ 
	::cho::array_pod<::cho::SColorBGRA>																		& bmpOffscreen								= applicationInstance.BitmapOffsceen;
	uint32_t																								linearScreenSize							= applicationInstance.MainWindow.Size.x * applicationInstance.MainWindow.Size.y;
	if(bmpOffscreen.size() < linearScreenSize)
		bmpOffscreen.resize(linearScreenSize);
}

static	::RECT																						minClientRect								= {100, 100, 100 + 320, 100 + 200};

		LRESULT WINAPI																				mainWndProc									(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)							{
	::SApplication																							& applicationInstance						= *g_ApplicationInstance;
	static	const int																						adjustedMinRect								= ::AdjustWindowRectEx(&minClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	::cho::SDisplayPlatformDetail																			& displayDetail								= applicationInstance.MainWindow.PlatformDetail;
	switch(uMsg) {
	default: break;		
	case WM_CLOSE			: ::DestroyWindow(hWnd); return 0;
	case WM_GETMINMAXINFO	:	// Catch this message so to prevent the window from becoming too small.
		((::MINMAXINFO*)lParam)->ptMinTrackSize																= {minClientRect.right - minClientRect.left, minClientRect.bottom - minClientRect.top}; 
		return 0;
	case WM_SIZE			: 
		if(lParam) {
			::cho::SCoord2<int32_t>																					newMetrics									= ::cho::SCoord2<WORD>{LOWORD(lParam), HIWORD(lParam)}.Cast<int32_t>();
			if(newMetrics != applicationInstance.MainWindow.Size.Cast<int32_t>()) {
				applicationInstance.MainWindow.Size																	= newMetrics.Cast<uint32_t>();
				applicationInstance.MainWindow.Resized																= true;
				applicationInstance.MainWindow.Repaint																= true; 
				::updateOffscreen	(applicationInstance);
				error_if(errored(::draw							(applicationInstance)), "Not sure why these would fail.");
				error_if(errored(::cho::displayPresentTarget	(applicationInstance.MainWindow, applicationInstance.BitmapOffsceen)), "Not sure why these would fail.");
			}
		}
		if( wParam == SIZE_MINIMIZED ) {
			applicationInstance.MainWindow.MinOrMaxed = applicationInstance.MainWindow.NoDraw					= true;
		}
		else if( wParam == SIZE_MAXIMIZED ) {
			applicationInstance.MainWindow.Resized = applicationInstance.MainWindow.MinOrMaxed					= true;
			applicationInstance.MainWindow.NoDraw																= false;
		}
		else if( wParam == SIZE_RESTORED ) {
			applicationInstance.MainWindow.Resized																= true;
			applicationInstance.MainWindow.MinOrMaxed															= true;
			applicationInstance.MainWindow.NoDraw																= false;
		}
		else {
			//State.Resized									= true;	? 
			applicationInstance.MainWindow.MinOrMaxed = applicationInstance.MainWindow.NoDraw					= false;
		}
		break;
	case WM_PAINT			: break;
	case WM_DESTROY			: 
		::PostQuitMessage(0); 
		displayDetail.WindowHandle																			= 0;
		applicationInstance.MainWindow.Closed																= true;
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

		void																						initWndClass								(::HINSTANCE hInstance, const TCHAR* className, ::WNDCLASSEX& wndClassToInit)	{
	wndClassToInit																						= {sizeof(::WNDCLASSEX),};
	wndClassToInit.lpfnWndProc																			= ::mainWndProc;
	wndClassToInit.hInstance																			= hInstance;
	wndClassToInit.hCursor																				= LoadCursor(NULL, IDC_ARROW);
	wndClassToInit.hbrBackground																		= (::HBRUSH)(COLOR_3DFACE + 1);
	wndClassToInit.lpszClassName																		= className;
}

// --- Cleanup application resources.
					::cho::error_t																	cleanup										(::SApplication& applicationInstance)											{ 
	::cho::SDisplayPlatformDetail																			& displayDetail								= applicationInstance.MainWindow.PlatformDetail;
	if(displayDetail.WindowHandle) {
		error_if(0 == ::DestroyWindow(displayDetail.WindowHandle), "Not sure why would this fail.");
		::cho::displayUpdate(applicationInstance.MainWindow);
	}
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	error_if(errored(::cho::asciiDisplayDestroy	()), "Failed to close ASCII display!");
	error_if(errored(::cho::asciiTargetDestroy	(applicationInstance.ASCIIRenderTarget)), "No known reasons for this to fail. It may mean a mismamagement of the target array pointers!");

	g_ApplicationInstance																				= 0;
	//error_printf("Error message test. Press F5 if the debugger breaks execution at this point.");
	return 0;
}

// --- Initialize console.
					::cho::error_t																	setup										(::SApplication& applicationInstance)											{ 
	g_ApplicationInstance																				= &applicationInstance;
	error_if(errored(::cho::asciiTargetCreate	(applicationInstance.ASCIIRenderTarget, ::ASCII_SCREEN_WIDTH, ::ASCII_SCREEN_HEIGHT)			), "This should never happen and usually indicates memory corruption or lack of system resources.");
	error_if(errored(::cho::asciiDisplayCreate	(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height())	), "Not sure why this would fail at this point.");
	::cho::SDisplay																							& mainWindow								= applicationInstance.MainWindow;
	::cho::SDisplayPlatformDetail																			& displayDetail								= mainWindow.PlatformDetail;
	::initWndClass(applicationInstance.RuntimeValues.PlatformDetail.EntryPointArgs.hInstance, displayDetail.WindowClassName, displayDetail.WindowClass);
	::RegisterClassEx(&displayDetail.WindowClass);

	//::std::vector<::cho::SColorBGRA>																		& bmpOffscreen								= applicationInstance.BitmapOffsceen;
	applicationInstance.MainWindow.Size																	= {::BMP_SCREEN_WIDTH, ::BMP_SCREEN_HEIGHT};
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

					::cho::error_t																	update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	error_if(errored(::cho::asciiDisplayPresent	(applicationInstance.ASCIIRenderTarget)), "Could this fail if the console isn't properly initialized?");
	error_if(errored(::cho::asciiTargetClear	(applicationInstance.ASCIIRenderTarget)), "This shouldn't fail either unless memory corruption happened.");
	applicationInstance.Timer		.Frame();
	applicationInstance.FrameInfo	.Frame(applicationInstance.Timer.LastTimeMicroseconds);
	::cho::SDisplay																							& mainWindow								= applicationInstance.MainWindow;
	::cho::displayUpdate(mainWindow);
	::updateOffscreen	(applicationInstance);
	retval_info_if(1, 0 == mainWindow.PlatformDetail.WindowHandle, "Application exiting because the main window was closed.");
	error_if(errored(::cho::displayPresentTarget(mainWindow, applicationInstance.BitmapOffsceen)), "Unknown error.");
	char																									buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, applicationInstance.Timer.LastTimeSeconds);
	::HWND																									windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

					::cho::error_t																	draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::cho::SDisplay																					& mainWindow								= applicationInstance.MainWindow;
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
	return 0;
}
