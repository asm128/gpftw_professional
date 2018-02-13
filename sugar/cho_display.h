#include "cho_coord.h"
#include "cho_color.h"
#include "cho_grid_view.h"

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

#if defined(CHO_WINDOWS)
	static inline		void													initWndClass								(::HINSTANCE hInstance, const TCHAR* className, ::WNDCLASSEX& wndClassToInit, WNDPROC wndProc)	{
		wndClassToInit																= {sizeof(::WNDCLASSEX),};
		wndClassToInit.lpfnWndProc													= wndProc;
		wndClassToInit.hInstance													= hInstance;
		wndClassToInit.hCursor														= LoadCursor(NULL, IDC_ARROW);
		wndClassToInit.hbrBackground												= (::HBRUSH)(COLOR_3DFACE + 1);
		wndClassToInit.lpszClassName												= className;
	}
#endif

	struct SDisplay {
							::cho::SDisplayPlatformDetail							PlatformDetail								= {};
							::cho::SCoord2<uint32_t>								PreviousSize								= {};
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

		::cho::error_t																displayPresentTarget						(::cho::SDisplay& displayInstance, const ::cho::grid_view<::cho::SColorBGRA>& targetToPresent);
		::cho::error_t																displayUpdate								(::cho::SDisplay& applicationInstance);
} // namespace

#endif // CHO_DISPLAY_H_293874928374
