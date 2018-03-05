#include "cho_runtime.h"
#include "cho_display.h"
#include "cho_input.h"
#include "cho_timer.h"
#include "cho_frameinfo.h"
#include "cho_texture.h"

#ifndef CHO_FRAMEWORK_H_20987347928
#define CHO_FRAMEWORK_H_20987347928

namespace cho
{
	struct SFramework {
		typedef				::cho::STexture<::cho::SColorBGRA>					TOffscreen;

							::cho::SRuntimeValues								RuntimeValues								= {};
							::cho::SDisplay										MainDisplay									= {};
							::cho::SInput										Input										= {};
							::cho::STimer										Timer										= {};
							::cho::SFrameInfo									FrameInfo									= {};
							TOffscreen											Offscreen									= {};

																				SFramework									(::cho::SRuntimeValues& runtimeValues)			noexcept	: RuntimeValues(runtimeValues) {}
	}; // struct

						::cho::error_t										updateFramework								(::cho::SFramework& framework);
} // namespace

#endif // CHO_FRAMEWORK_H_20987347928
