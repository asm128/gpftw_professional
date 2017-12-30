// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "cho_ascii_display.h"
#include "cho_frameinfo.h"
#include "cho_timer.h"
#include "cho_bitmap_target.h"
#include "cho_array.h"
#include "cho_input.h"
#include "cho_display.h"
#include "cho_app_impl.h"

static constexpr	const uint32_t																	BMP_SCREEN_WIDTH							= 512;
static constexpr	const uint32_t																	BMP_SCREEN_HEIGHT							= uint32_t(::BMP_SCREEN_WIDTH * (9.0 / 16.0));
static constexpr	const uint32_t																	ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t																	ASCII_SCREEN_HEIGHT							= 50	;

struct SApplication {
						::cho::SDisplay																	MainWindow									= {};
						::cho::array_pod<::cho::SColorBGRA>												BitmapOffsceen								= {};
						::cho::SRuntimeValues															RuntimeValues								= {};
						::cho::SInput																	SystemInput									= {};
						::cho::STimer																	Timer										= {};
						::cho::SFrameInfo																FrameInfo									= {};
						::cho::SASCIITarget																ASCIIRenderTarget							= {};
						::cho::SPalette																	Palette										= 
		{	(uint32_t)::cho::ASCII_COLOR_INDEX_0		
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_1 	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_2 	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_3 	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_4 	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_5 	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_6 	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_7 	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_8 	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_9 	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_10	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_11	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_12	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_13	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_14	
		,	(uint32_t)::cho::ASCII_COLOR_INDEX_15	
		};

																										SApplication								(::cho::SRuntimeValues& runtimeValues)											: RuntimeValues(runtimeValues) {}
};

					::cho::error_t																	setup										(::SApplication& applicationInstance);
					::cho::error_t																	cleanup										(::SApplication& applicationInstance);
					::cho::error_t																	update										(::SApplication& applicationInstance);
					::cho::error_t																	draw										(::SApplication& applicationInstance);

CHO_DEFINE_APPLICATION_ENTRY_POINT(::SApplication);	

static				::SApplication																	* g_ApplicationInstance						= 0;

struct SOffscreenPlatformDetail {
	uint32_t																							BitmapInfoSize								= 0;
	::BITMAPINFO																						* BitmapInfo								= 0;
	::HDC																								IntermediateDeviceContext					= 0;    // <- note, we're creating, so it needs to be destroyed
	::HBITMAP																							IntermediateBitmap							= 0;

																										~SOffscreenPlatformDetail					()																				{
		if(BitmapInfo					) ::free			(BitmapInfo					); 
		if(IntermediateBitmap			) ::DeleteObject	(IntermediateBitmap			); 
		if(IntermediateDeviceContext	) ::DeleteDC		(IntermediateDeviceContext	); 
	}
};
	
		::cho::error_t																				drawBuffer									(::HDC hdc, int width, int height, ::cho::SColorBGRA *colorArray)				{
	const uint32_t																							bytesToCopy									= sizeof(::RGBQUAD) * width * height;
	::SOffscreenPlatformDetail																				offscreenDetail								= {};
	offscreenDetail.BitmapInfoSize																		= sizeof(::BITMAPINFO) + bytesToCopy;
	offscreenDetail.BitmapInfo																			= (::BITMAPINFO*)::malloc(offscreenDetail.BitmapInfoSize);
	for(uint32_t iPixel = 0, pixelCount = width * height; iPixel < pixelCount; ++iPixel)
		((::RGBQUAD*)offscreenDetail.BitmapInfo->bmiColors)[iPixel]											= {colorArray[iPixel].b, colorArray[iPixel].g, colorArray[iPixel].r, 0xFF};

	offscreenDetail.BitmapInfo->bmiHeader.biSize														= sizeof(::BITMAPINFO);
	offscreenDetail.BitmapInfo->bmiHeader.biWidth														= width;
	offscreenDetail.BitmapInfo->bmiHeader.biHeight														= height;
	offscreenDetail.BitmapInfo->bmiHeader.biPlanes														= 1;
	offscreenDetail.BitmapInfo->bmiHeader.biBitCount													= 32;
	offscreenDetail.BitmapInfo->bmiHeader.biCompression													= BI_RGB;
	offscreenDetail.BitmapInfo->bmiHeader.biSizeImage													= width * height * sizeof(::RGBQUAD);
	offscreenDetail.BitmapInfo->bmiHeader.biXPelsPerMeter												= 0x0ec4; // Paint and PSP use these values.
	offscreenDetail.BitmapInfo->bmiHeader.biYPelsPerMeter												= 0x0ec4; // Paint and PSP use these values.
	offscreenDetail.BitmapInfo->bmiHeader.biClrUsed														= 0; 
	offscreenDetail.BitmapInfo->bmiHeader.biClrImportant												= 0;

	offscreenDetail.IntermediateDeviceContext															= ::CreateCompatibleDC(hdc);    // <- note, we're creating, so it needs to be destroyed
	char																									* ppvBits									= 0;
	offscreenDetail.IntermediateBitmap																	= ::CreateDIBSection(offscreenDetail.IntermediateDeviceContext, offscreenDetail.BitmapInfo, DIB_RGB_COLORS, (void**) &ppvBits, NULL, 0);
	reterr_error_if(0 == ::SetDIBits(NULL, offscreenDetail.IntermediateBitmap, 0, height, offscreenDetail.BitmapInfo->bmiColors, offscreenDetail.BitmapInfo, DIB_RGB_COLORS), "Cannot copy bits into dib section.\n");
	::HBITMAP																								hBmpOld										= (::HBITMAP)::SelectObject(offscreenDetail.IntermediateDeviceContext, offscreenDetail.IntermediateBitmap);    // <- altering state
	error_if(FALSE == ::BitBlt(hdc, 0, 0, width, height, offscreenDetail.IntermediateDeviceContext, 0, 0, SRCCOPY), "Not sure why would this happen but probably due to mismanagement I've had it failing when I acquired the device too much and never released it.");

	::SelectObject(hdc, hBmpOld);	// put the old bitmap back in the DC (restore state)
	return 0;
}

		void																						updateOffscreen								(::SApplication& applicationInstance)											{ 
	::cho::array_pod<::cho::SColorBGRA>																		& bmpOffscreen								= applicationInstance.BitmapOffsceen;
	uint32_t																								linearScreenSize							= applicationInstance.MainWindow.Size.x * applicationInstance.MainWindow.Size.y;
	if(bmpOffscreen.size() < linearScreenSize)
		bmpOffscreen.resize(linearScreenSize);
}

		::cho::error_t																				presentTarget								(::SApplication& applicationInstance)											{ 
	::HWND																									windowHandle								= applicationInstance.MainWindow.PlatformDetail.WindowHandle;
	retwarn_warn_if(0 == windowHandle, "presentTarget called without a valid window handle set for the main window.");
	::HDC																									dc											= ::GetDC(windowHandle);
	::cho::array_pod<::cho::SColorBGRA>																		& bmpOffscreen								= applicationInstance.BitmapOffsceen;
	::drawBuffer(dc, applicationInstance.MainWindow.Size.x, applicationInstance.MainWindow.Size.y, &bmpOffscreen[0]);
	::ReleaseDC(windowHandle, dc);
	return 0;
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
				::draw				(applicationInstance);
				::presentTarget		(applicationInstance);
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

		void																						updateMainWindow							(::SApplication& applicationInstance)											{ 
	::MSG																									msg											= {};
	while(::PeekMessage(&msg, applicationInstance.MainWindow.PlatformDetail.WindowHandle, 0, 0, PM_REMOVE)) {
		::TranslateMessage	(&msg);
		::DispatchMessage	(&msg);
		if(msg.message == WM_QUIT)
			break;
	}
}

// --- Cleanup application resources.
					::cho::error_t																	cleanup										(::SApplication& applicationInstance)											{ 
	::cho::SDisplayPlatformDetail																			& displayDetail								= applicationInstance.MainWindow.PlatformDetail;
	if(displayDetail.WindowHandle) {
		error_if(0 == ::DestroyWindow(displayDetail.WindowHandle), "Not sure why would this fail.");
		::updateMainWindow(applicationInstance);
	}
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	error_if(errored(::cho::asciiDisplayDestroy	()), "Failed to close ASCII display!");
	error_if(errored(::cho::asciiTargetDestroy	(applicationInstance.ASCIIRenderTarget)), "No known reasons for this to fail. It may mean a mismamagement of the target array pointers!");

	g_ApplicationInstance																				= 0;
	error_printf("Error message test. Press F5 if the debugger breaks execution at this point.");
	return 0;
}

// --- Initialize console.
					::cho::error_t																	setup										(::SApplication& applicationInstance)											{ 
	g_ApplicationInstance																				= &applicationInstance;
	error_if(errored(::cho::asciiTargetCreate	(applicationInstance.ASCIIRenderTarget, ::ASCII_SCREEN_WIDTH, ::ASCII_SCREEN_HEIGHT)			), "This should never happen and usually indicates memory corruption or lack of system resources.");
	error_if(errored(::cho::asciiDisplayCreate	(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height())	), "Not sure why this would fail at this point.");
	::cho::SDisplay																							& mainWindow								= applicationInstance.MainWindow;
	::cho::SDisplayPlatformDetail																			& displayDetail								= mainWindow.PlatformDetail;
	::initWndClass(applicationInstance.RuntimeValues.hInstance, displayDetail.WindowClassName, displayDetail.WindowClass);
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

					::cho::error_t																	update										(::SApplication& applicationInstance)											{ 
	error_if(errored(::cho::asciiDisplayPresent	(applicationInstance.ASCIIRenderTarget)), "Could this fail if the console isn't properly initialized?");
	error_if(errored(::cho::asciiTargetClear	(applicationInstance.ASCIIRenderTarget)), "This shouldn't fail either unless memory corruption happened.");
	applicationInstance.Timer		.Frame();
	applicationInstance.FrameInfo	.Frame(applicationInstance.Timer.LastTimeMicroseconds);
	::updateMainWindow	(applicationInstance);
	::updateOffscreen	(applicationInstance);
	retval_info_if(1, 0 == applicationInstance.MainWindow.PlatformDetail.WindowHandle, "Application exiting because the main window was closed.");
	error_if(errored(::presentTarget(applicationInstance)), "Unknown error.");
	char																									buffer		[256]							= {};
	const ::cho::SDisplay																					& mainWindow								= applicationInstance.MainWindow;
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
	::cho::drawRectangle	(bmpTarget, ::cho::SColorRGBA(applicationInstance.Palette[::cho::ASCII_COLOR_BLUE		]), geometry0);
	::cho::drawRectangle	(bmpTarget, ::cho::SColorRGBA(applicationInstance.Palette[::cho::ASCII_COLOR_BLUE		]), {geometry0.Offset + ::cho::SCoord2<int32_t>{1, 1}, geometry0.Size - ::cho::SCoord2<int32_t>{2, 2}});
	::cho::drawCircle		(bmpTarget, ::cho::SColorRGBA(applicationInstance.Palette[::cho::ASCII_COLOR_GREEN		]), geometry1);
	::cho::drawCircle		(bmpTarget, ::cho::SColorRGBA(applicationInstance.Palette[::cho::ASCII_COLOR_RED		]), {geometry1.Radius - 1, geometry1.Center});
	::cho::drawTriangle		(bmpTarget, ::cho::SColorRGBA(applicationInstance.Palette[::cho::ASCII_COLOR_YELLOW		]), geometry2);
	::cho::drawLine			(bmpTarget, ::cho::SColorRGBA(applicationInstance.Palette[::cho::ASCII_COLOR_MAGENTA	]), ::cho::SLine2D<int32_t>{geometry2.A, geometry2.B});
	::cho::drawLine			(bmpTarget, ::cho::SColorRGBA(applicationInstance.Palette[::cho::ASCII_COLOR_WHITE		]), ::cho::SLine2D<int32_t>{geometry2.B, geometry2.C});
	::cho::drawLine			(bmpTarget, ::cho::SColorRGBA(applicationInstance.Palette[::cho::ASCII_COLOR_LIGHTGREY	]), ::cho::SLine2D<int32_t>{geometry2.C, geometry2.A});
	return 0;
}
