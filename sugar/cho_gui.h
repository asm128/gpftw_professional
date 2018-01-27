#include "cho_array.h"
#include <string>

#ifndef CHO_GUI_H_2903874982374
#define CHO_GUI_H_2903874982374

namespace cho
{
	enum CONTROL_STATE	: int8_t
		{	CONTROL_STATE_Idle
		,	CONTROL_STATE_Hover		
		,	CONTROL_STATE_Pressed		
		,	CONTROL_STATE_Selected	
		,	CONTROL_STATE_Disabled	
		,	CONTROL_STATE_Unused		
		,	CONTROL_STATE_COUNT		
		,	CONTROL_STATE_INVALID	= (int8_t)-1
		};

	struct SControlState {
		bool	Hover		: 1;
		bool	Pressed		: 1;
		bool	Selected	: 1;
		bool	Disabled	: 1;
		bool	Execute		: 1;
		bool	Unused		: 1;
	};

	struct SControlPropertyIndices {
		bool	Text;
		bool	Area;
		bool	Color;
	};

	struct SGUIState {
							::cho::array_pod<::std::string>				Text;
	};

	// 
	struct SGUI {
							SGUIState									States[CONTROL_STATE_COUNT];
	};
} // namespace

#endif // CHO_GUI_H_2903874982374

