#include "ftw_runtime.h"

#define FTW_DEFINE_APPLICATION_ENTRY_POINT(_mainClass)																																													\
		int																							rtMain										(::ftw::SRuntimeValues& runtimeValues)												{	\
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);																																										\
	_mainClass																								* applicationInstance						= new _mainClass(runtimeValues);												\
	if(0 == applicationInstance)																																																		\
		return -1;																																																						\
																																																										\
	::setup(*applicationInstance);																																																		\
																																																										\
	while(true) {																																																						\
		::ftw::error_t																						updateResult								= ::update(*applicationInstance);												\
		break_info_if(1 == updateResult, "application requested termination");																																							\
		break_error_if(errored(updateResult), "update() returned error.");																																								\
		error_if(::draw(*applicationInstance), "Why would this ever happen?");																																							\
	}																																																									\
																																																										\
	::cleanup(*applicationInstance);																																																	\
	delete(applicationInstance);																																																		\
	return 0;																																																							\
}																																																										\
																																																										\
		int																							main										()																					{	\
	::ftw::SRuntimeValues																					runtimeValues								= {::GetModuleHandle(NULL), 0, 0, SW_SHOW};										\
	return ::ftw::failed(::rtMain(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;																																						\
}																																																										\
																																																										\
		int	WINAPI																					WinMain																																\
	(	_In_		::HINSTANCE		hInstance																																															\
	,	_In_opt_	::HINSTANCE		hPrevInstance																																														\
	,	_In_		::LPSTR			lpCmdLine																																															\
	,	_In_		::INT			nShowCmd																																															\
	)																																																									\
{																																																										\
	::ftw::SRuntimeValues																					runtimeValues								= 																				\
		{	hInstance																																																					\
		,	hPrevInstance																																																				\
		,	lpCmdLine																																																					\
		,	nShowCmd																																																					\
		};																																																								\
	return ::ftw::failed(::rtMain(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;																																						\
}
