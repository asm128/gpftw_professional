// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
//
#include "ftw_ascii_display.h"
#include "ftw_frameinfo.h"
#include "ftw_timer.h"
#include "ftw_bitmap_target.h"
#include "ftw_array.h"
#include "ftw_app_impl.h"

static constexpr	const uint32_t																	BMP_SCREEN_WIDTH							= 512;
static constexpr	const uint32_t																	BMP_SCREEN_HEIGHT							= uint32_t(::BMP_SCREEN_WIDTH * (9.0 / 16.0));
static constexpr	const uint32_t																	ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t																	ASCII_SCREEN_HEIGHT							= 50	;

struct STriangleMesh {

};

struct SInput {
						uint8_t																			KeyboardPrevious	[256]					= {};
						uint8_t																			KeyboardCurrent		[256]					= {};
};


struct SDisplayPlatformDetail {
						::HWND																			WindowHandle								= {};
						::WNDCLASSEX																	WindowClass									= {};

	static constexpr	const char																		WindowClassName	[256]						= "FTWL_WINDOW";
};

struct SDisplay {
						::SDisplayPlatformDetail														PlatformDetail								= {};
						::ftw::SCoord2<uint32_t>														Size										= {};
						bool																			Resized										= {};
						bool																			Repaint										= {};
						bool																			NoDraw										= {};
						bool																			MinOrMaxed									= {};
};

struct SApplication {
						::SDisplay																		MainWindow									= {};
						::ftw::array_pod<::ftw::SColorBGRA>												BitmapOffsceen								= {};
						::ftw::SRuntimeValues															RuntimeValues								= {};
						::SInput																		SystemInput									= {};
						::ftw::STimer																	Timer										= {};
						::ftw::SFrameInfo																FrameInfo									= {};
						::ftw::SASCIITarget																ASCIIRenderTarget							= {};
						::ftw::SBitmapTargetBGRA														BitmapRenderTarget							= {};
						::ftw::SPalette																	Palette										= 
		{	(uint32_t)::ftw::ASCII_COLOR_INDEX_0		
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_1 	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_2 	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_3 	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_4 	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_5 	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_6 	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_7 	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_8 	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_9 	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_10	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_11	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_12	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_13	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_14	
		,	(uint32_t)::ftw::ASCII_COLOR_INDEX_15	
		};

																										SApplication								(::ftw::SRuntimeValues& runtimeValues)											: RuntimeValues(runtimeValues) {}
};

					::ftw::error_t																	setup										(::SApplication& applicationInstance);
					::ftw::error_t																	cleanup										(::SApplication& applicationInstance);
					::ftw::error_t																	update										(::SApplication& applicationInstance);
					::ftw::error_t																	draw										(::SApplication& applicationInstance);

FTW_DEFINE_APPLICATION_ENTRY_POINT(::SApplication);	

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
	
		::ftw::error_t																				drawBuffer									(::HDC hdc, int width, int height, ::ftw::SColorBGRA *colorArray)				{
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
	::ftw::array_pod<::ftw::SColorBGRA>																		& bmpOffscreen								= applicationInstance.BitmapOffsceen;
	uint32_t																								linearScreenSize							= applicationInstance.MainWindow.Size.x * applicationInstance.MainWindow.Size.y;
	if(bmpOffscreen.size() < linearScreenSize) {
		bmpOffscreen.resize(linearScreenSize);
		//applicationInstance.BitmapRenderTarget.Colors														= {&bmpOffscreen[0], applicationInstance.MainWindow.Size.x, applicationInstance.MainWindow.Size.y};
	}
	if(applicationInstance.MainWindow.Resized) 
		applicationInstance.BitmapRenderTarget.Colors														= {&bmpOffscreen[0], applicationInstance.MainWindow.Size.x, applicationInstance.MainWindow.Size.y};
}

		::ftw::error_t																				presentTarget								(::SApplication& applicationInstance)											{ 
	::HWND																									windowHandle								= applicationInstance.MainWindow.PlatformDetail.WindowHandle;
	retwarn_warn_if(0 == windowHandle, "presentTarget called without a valid window handle set for the main window.");
	::HDC																									dc											= ::GetDC(windowHandle);
	::ftw::array_pod<::ftw::SColorBGRA>																		& bmpOffscreen								= applicationInstance.BitmapOffsceen;
	::drawBuffer(dc, applicationInstance.MainWindow.Size.x, applicationInstance.MainWindow.Size.y, &bmpOffscreen[0]);
	::ReleaseDC(windowHandle, dc);
	return 0;
}

static	::RECT																						minClientRect								= {100, 100, 100 + 320, 100 + 200};

		LRESULT WINAPI																				mainWndProc									(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)							{
	::SApplication																							& applicationInstance						= *g_ApplicationInstance;
	static	const int																						adjustedMinRect								= ::AdjustWindowRectEx(&minClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	::SDisplayPlatformDetail																				& displayDetail								= applicationInstance.MainWindow.PlatformDetail;
	switch(uMsg) {
	default: break;		
	case WM_CLOSE			: ::DestroyWindow(hWnd); return 0;
	case WM_GETMINMAXINFO	:	// Catch this message so to prevent the window from becoming too small.
		((::MINMAXINFO*)lParam)->ptMinTrackSize																= {minClientRect.right - minClientRect.left, minClientRect.bottom - minClientRect.top}; 
		return 0;
	case WM_SIZE			: 
		if(lParam) {
			::ftw::SCoord2<int32_t>																					newMetrics									= ::ftw::SCoord2<WORD>{LOWORD(lParam), HIWORD(lParam)}.Cast<int32_t>();
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
		::TranslateMessage(&msg);
		::DispatchMessage	(&msg);
		if(msg.message == WM_QUIT) {
			applicationInstance.MainWindow.PlatformDetail.WindowHandle											= 0;
			break;
		}
		break;
	}
}

// --- Cleanup application resources.
					::ftw::error_t																	cleanup										(::SApplication& applicationInstance)											{ 
	::SDisplayPlatformDetail																				& displayDetail								= applicationInstance.MainWindow.PlatformDetail;
	if(displayDetail.WindowHandle) {
		::DestroyWindow(displayDetail.WindowHandle);
		::updateMainWindow(applicationInstance);
	}
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	error_if(errored(::ftw::asciiDisplayDestroy	()), "Failed to close ASCII display!");
	error_if(errored(::ftw::asciiTargetDestroy	(applicationInstance.ASCIIRenderTarget)), "No known reasons for this to fail. It may mean a mismamagement of the target array pointers!");
	g_ApplicationInstance																				= 0;
	return 0;
}

// --- Initialize console.
					::ftw::error_t																	setup										(::SApplication& applicationInstance)											{ 
	g_ApplicationInstance																				= &applicationInstance;
	::ftw::asciiTargetCreate	(applicationInstance.ASCIIRenderTarget, ::ASCII_SCREEN_WIDTH, ::ASCII_SCREEN_HEIGHT);
	::ftw::asciiDisplayCreate	(applicationInstance.ASCIIRenderTarget.Width(), applicationInstance.ASCIIRenderTarget.Height());
	::SDisplay																								& mainWindow								= applicationInstance.MainWindow;
	::SDisplayPlatformDetail																				& displayDetail								= mainWindow.PlatformDetail;
	::initWndClass(applicationInstance.RuntimeValues.hInstance, displayDetail.WindowClassName, displayDetail.WindowClass);
	::RegisterClassEx(&displayDetail.WindowClass);

	//::std::vector<::ftw::SColorBGRA>																		& bmpOffscreen								= applicationInstance.BitmapOffsceen;
	applicationInstance.MainWindow.Size																	= {::BMP_SCREEN_WIDTH, ::BMP_SCREEN_HEIGHT};
	::RECT																									finalClientRect								= {100, 100, 100 + (LONG)mainWindow.Size.x, 100 + (LONG)mainWindow.Size.y};
	::AdjustWindowRectEx(&finalClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
	mainWindow.PlatformDetail.WindowHandle																= ::CreateWindowEx(0, displayDetail.WindowClassName, TEXT("Window FTW"), WS_OVERLAPPEDWINDOW
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

					::ftw::error_t																	update										(::SApplication& applicationInstance)											{ 
	error_if(errored(::ftw::asciiDisplayPresent	(applicationInstance.ASCIIRenderTarget)), "Could this fail if the console isn't properly initialized?");
	error_if(errored(::ftw::asciiTargetClear	(applicationInstance.ASCIIRenderTarget)), "This shouldn't fail either unless memory corruption happened.");
	applicationInstance.Timer		.Frame();
	applicationInstance.FrameInfo	.Frame(applicationInstance.Timer.LastTimeMicroseconds);
	::updateOffscreen	(applicationInstance);
	::updateMainWindow	(applicationInstance);
	retval_info_if(1, 0 == applicationInstance.MainWindow.PlatformDetail.WindowHandle, "Application exiting because the main window was closed.");
	error_if(errored(::presentTarget(applicationInstance)), "Unknown error.");
	char																									buffer		[256]							= {};
	const ::SDisplay																						& mainWindow								= applicationInstance.MainWindow;
	sprintf_s(buffer, "[%u x %u]. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, applicationInstance.Timer.LastTimeSeconds);
	::HWND																									windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

					::ftw::error_t																	draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::SDisplay																						& mainWindow								= applicationInstance.MainWindow;
	::memset(&applicationInstance.BitmapRenderTarget.Colors[0][0], 0, sizeof(::ftw::SColorBGRA) * applicationInstance.BitmapRenderTarget.Colors.size());
	::ftw::array_pod<::ftw::SColorBGRA>																		& bmpOffscreen								= applicationInstance.BitmapOffsceen;
	::ftw::SCoord2		<int32_t>																			screenCenter								= {(int32_t)mainWindow.Size.x / 2, (int32_t)mainWindow.Size.y / 2};
	::ftw::SRectangle2D<int32_t>																			geometry0									= {{2, 2}, {(int32_t)((applicationInstance.FrameInfo.FrameNumber / 2) % (mainWindow.Size.x - 2)), 5}};
	::ftw::SCircle2D	<int32_t>																			geometry1									= {5.0 + (applicationInstance.FrameInfo.FrameNumber / 5) % 5, screenCenter};	
	::ftw::STriangle2D	<int32_t>																			geometry2									= {{0, 0}, {15, 15}, {-5, 10}};	
	geometry2.A																							+= screenCenter + ::ftw::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.B																							+= screenCenter + ::ftw::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	geometry2.C																							+= screenCenter + ::ftw::SCoord2<int32_t>{(int32_t)geometry1.Radius, (int32_t)geometry1.Radius};
	::ftw::SBitmapTargetBGRA																				bmpTarget									= {{&bmpOffscreen[0], mainWindow.Size.x, mainWindow.Size.y},};
	::ftw::drawRectangle	(bmpTarget, ::ftw::SColorRGBA(applicationInstance.Palette[::ftw::ASCII_COLOR_BLUE		]), geometry0);
	::ftw::drawRectangle	(bmpTarget, ::ftw::SColorRGBA(applicationInstance.Palette[::ftw::ASCII_COLOR_BLUE		]), {geometry0.Offset + ::ftw::SCoord2<int32_t>{1, 1}, geometry0.Size - ::ftw::SCoord2<int32_t>{2, 2}});
	::ftw::drawCircle		(bmpTarget, ::ftw::SColorRGBA(applicationInstance.Palette[::ftw::ASCII_COLOR_GREEN		]), geometry1);
	::ftw::drawCircle		(bmpTarget, ::ftw::SColorRGBA(applicationInstance.Palette[::ftw::ASCII_COLOR_RED		]), {geometry1.Radius - 1, geometry1.Center});
	::ftw::drawTriangle		(bmpTarget, ::ftw::SColorRGBA(applicationInstance.Palette[::ftw::ASCII_COLOR_YELLOW		]), geometry2);
	::ftw::drawLine			(bmpTarget, ::ftw::SColorRGBA(applicationInstance.Palette[::ftw::ASCII_COLOR_MAGENTA	]), ::ftw::SLine2D<int32_t>{geometry2.A, geometry2.B});
	::ftw::drawLine			(bmpTarget, ::ftw::SColorRGBA(applicationInstance.Palette[::ftw::ASCII_COLOR_WHITE		]), ::ftw::SLine2D<int32_t>{geometry2.B, geometry2.C});
	::ftw::drawLine			(bmpTarget, ::ftw::SColorRGBA(applicationInstance.Palette[::ftw::ASCII_COLOR_LIGHTGREY	]), ::ftw::SLine2D<int32_t>{geometry2.C, geometry2.A});
	return 0;
}
