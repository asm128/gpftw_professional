#include "cho_array.h"
#include "cho_color.h"
#include "cho_input.h"
#include <string>

#ifndef CHO_GUI_H_2903874982374
#define CHO_GUI_H_2903874982374

namespace cho
{
#pragma pack(push, 1)
	enum CONTROL_STATE	: int8_t
		{ CONTROL_STATE_Idle
		, CONTROL_STATE_Hover		
		, CONTROL_STATE_Pressed		
		, CONTROL_STATE_Selected	
		, CONTROL_STATE_Execute
		, CONTROL_STATE_Disabled	
		, CONTROL_STATE_Unused		
		, CONTROL_STATE_COUNT		
		, CONTROL_STATE_INVALID			= (int8_t)-1
		};

	struct SControlState {
							bool												Hover									: 1;
							bool												Pressed									: 1;
							bool												Selected								: 1;
							bool												Execute									: 1;
							bool												Disabled								: 1;
							bool												Unused									: 1;
	};

	struct SControlPropertyIndices {
							int16_t												Text									= -1;
							int16_t												Area									= -1;
							int16_t												AreaRealigned							= -1;
							int16_t												Center									= -1;
							int16_t												ColorFill								= -1;
							int16_t												ColorBorder								= -1;
							int16_t												SizeBorder								= -1;
							int16_t												SizeMargin								= -1;
							int16_t												SizePadding								= -1;
	};

	struct SGUIColorSurface {
							::cho::SColorBGRA									Fill									= {};
							::cho::SColorBGRA									Border									= {};
	};

	struct SGUIColorText {
							::cho::SColorBGRA									Fill									= {};
							::cho::SColorBGRA									Border									= {};
							::cho::SColorBGRA									Background								= {};
	};
#pragma pack(pop)

	struct SControlProperties {
							::std::string										Text									= {};
							::std::string										Tooltip									= {};
							::cho::SRectangle2D<int32_t>						Area									= {};
							::cho::SRectangle2D<int32_t>						AreaRealigned							= {};
							::cho::SCoord2<int32_t>								Center									= {};
							::cho::SGUIColorSurface								ColorSurface							= {};
							::cho::SGUIColorText								ColorText								= {};
							::cho::SCoord2<uint32_t>							SizeBorder								= {};
							::cho::SCoord2<uint32_t>							SizeMargin								= {};
							::cho::ALIGN										AlignArea								= {};
							::cho::ALIGN										AlignText								= {};
	};

	struct SControlPropertyTable {
							::cho::array_pod<::std::string					>	Text									= {};
							::cho::array_pod<::std::string					>	Tooltip									= {};
							::cho::array_pod<::cho::SRectangle2D<int32_t>	>	Area									= {};
							::cho::array_pod<::cho::SRectangle2D<int32_t>	>	AreaRealigned							= {};
							::cho::array_pod<::cho::SCoord2<int32_t>		>	Center									= {};
							::cho::array_pod<::cho::SGUIColorSurface		>	ColorSurface							= {};
							::cho::array_pod<::cho::SGUIColorText			>	ColorText								= {};
							::cho::array_pod<::cho::SCoord2<uint32_t>		>	SizeBorder								= {};
							::cho::array_pod<::cho::SCoord2<uint32_t>		>	SizeMargin								= {};
							::cho::array_pod<::cho::ALIGN					>	AlignArea								= {};
							::cho::array_pod<::cho::ALIGN					>	AlignText								= {};

							// -------------
							void												Clear									()						{
								::cho::clear
									(	Text				
									,	Area				
									,	AreaRealigned		
									,	ColorSurface		
									,	ColorText			
									,	SizeBorder			
									,	SizeMargin			
									,	AlignArea			
									,	AlignText			
									);
							}

							void												Resize									(uint32_t newSize)		{
								::cho::resize
									(	newSize
									,	Text				
									,	Area				
									,	AreaRealigned		
									,	ColorSurface		
									,	ColorText			
									,	SizeBorder			
									,	SizeMargin			
									,	AlignArea			
									,	AlignText			
									);
							}

	};

	struct SGUI {	// 
							::cho::SControlPropertyTable						Properties		[CONTROL_STATE_COUNT]	= {};
							::cho::array_pod<::cho::SControlState>				Controls								= {};
							::cho::SCoord2<uint32_t>							TargetSize								= {};
							::cho::SCoord2<int32_t>								MousePosition							= {};
	};

						::cho::error_t										createControl							(::cho::SGUI& guiSystem, const ::cho::SControlProperties& controlProperties);
						::cho::error_t										updateGUI								(::cho::SGUI& GUISystem, const ::cho::SInput& inputSystem);
} // namespace

#endif // CHO_GUI_H_2903874982374

