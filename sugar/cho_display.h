#include "cho_coord.h"

#if defined(CHO_WINDOWS)
#	include <windows.h>
#endif 

#ifndef CHO_DISPLAY_H_293874928374
#define CHO_DISPLAY_H_293874928374

namespace cho 
{
	struct SDisplayPlatformDetail {
#if defined(CHO_WINDOWS)
							::HWND													WindowHandle								= {};
							::WNDCLASSEX											WindowClass									= {};

		static constexpr	const char												WindowClassName	[256]						= "SUGAR_WINDOW";
#endif
	};

	struct SDisplay {
							::cho::SDisplayPlatformDetail							PlatformDetail								= {};
							::cho::SCoord2<uint32_t>								Size										= {};
							bool													Resized										: 1;
							bool													Repaint										: 1;
							bool													NoDraw										: 1;
							bool													MinOrMaxed									: 1;
							bool													Closed										: 1;

																					SDisplay									()
			: Resized		(false)
			, Repaint		(false)
			, NoDraw		(false)
			, MinOrMaxed	(false)
			, Closed		(false)
		{}
	};
} // namespace

#endif // CHO_DISPLAY_H_293874928374
