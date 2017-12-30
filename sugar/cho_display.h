#include "cho_coord.h"

#if defined(FTW_WINDOWS)
#	include <windows.h>
#endif 

#ifndef FTW_DISPLAY_H_293874928374
#define FTW_DISPLAY_H_293874928374

namespace cho 
{
	struct SDisplayPlatformDetail {
	#if defined(FTW_WINDOWS)
							::HWND																			WindowHandle								= {};
							::WNDCLASSEX																	WindowClass									= {};

		static constexpr	const char																		WindowClassName	[256]						= "FTWL_WINDOW";
	#endif
	};

	struct SDisplay {
							::cho::SDisplayPlatformDetail													PlatformDetail								= {};
							::cho::SCoord2<uint32_t>														Size										= {};
							bool																			Resized										: 1;
							bool																			Repaint										: 1;
							bool																			NoDraw										: 1;
							bool																			MinOrMaxed									: 1;
							bool																			Closed										: 1;

																											SDisplay									()
			: Resized		(false)
			, Repaint		(false)
			, NoDraw		(false)
			, MinOrMaxed	(false)
			, Closed		(false)
		{}
	};
} // namespace

#endif // FTW_DISPLAY_H_293874928374
