#include "cho_ascii_target.h"

				::cho::error_t									cho::asciiTargetCreate							(::cho::SASCIITarget& console, uint32_t width, uint32_t height)		{ 
	console.Characters												= {(uint8_t *)::malloc(sizeof(uint8_t ) * width * height), width, height}; 
	console.Colors													= {(uint16_t*)::malloc(sizeof(uint16_t) * width * height), width, height}; 
	return 0; 
}

				::cho::error_t									cho::asciiTargetDestroy						(::cho::SASCIITarget& console)											{ 
	if(console.Characters	.begin()) ::free(console.Characters	.begin());
	if(console.Colors		.begin()) ::free(console.Colors		.begin());
	return 0; 
}

				::cho::error_t									cho::asciiTargetClear							(::cho::SASCIITarget& console, uint8_t character, uint16_t color)		{ 
	uint8_t																* characters									= console.Characters	.begin();
	uint16_t															* colors										= console.Colors		.begin();
	for(uint32_t iCell = 0; iCell < console.Colors.size(); ++iCell) {
		characters	[iCell]												= character;
		colors		[iCell]												= color;
	}
	return 0; 
}
