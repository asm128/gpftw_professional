#include "cho_ascii_display.h"
#include "cho_frameinfo.h"
#include "cho_timer.h"
#include "cho_array.h"
#include "cho_input.h"
#include "cho_display.h"
#include "cho_runtime.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SApplication {
						::cho::SDisplay																	MainWindow									= {};
						::cho::array_pod<::cho::SColorBGRA>												BitmapOffsceen								= {};
						::cho::SRuntimeValues															RuntimeValues								= {};
						::cho::SInput																	SystemInput									= {};
						::cho::STimer																	Timer										= {};
						::cho::SFrameInfo																FrameInfo									= {};
						::cho::SASCIITarget																ASCIIRenderTarget							= {};
						::cho::SPalette																	ASCIIPalette								= 
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


#endif // APPLICATION_H_098273498237423
