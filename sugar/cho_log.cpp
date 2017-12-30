#include "cho_log.h"

#include <Windows.h>

#include <stdio.h>

static	::cho::error_t												getSystemErrorAsString						(const uint64_t lastError, char* buffer, uint32_t bufferSize)			{	// Get the error message, if any.
#if defined CHO_WINDOWS
	if(nullptr == buffer)
		return -1;
	if(0 == lastError) 
		return 0;
	const DWORD																size										= ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)(lastError & 0xFFFFFFFF), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, bufferSize, NULL);
	return (::cho::error_t)size;
#endif
}

		void														cho::_cho_print_system_errors				(const char* prefix, uint32_t prefixLen)			{
	char																	bufferError[4096]							= {};
	int64_t																	lastSystemError								= ::GetLastError() & 0xFFFFFFFFFFFFFFFFLL;
	if(lastSystemError) {
		::cho::error_t															stringLength								= ::getSystemErrorAsString((uint64_t)lastSystemError, bufferError, ::cho::size(bufferError));
		base_debug_print(prefix, prefixLen);
		base_debug_print(bufferError, stringLength);
		base_debug_print("\n", 1);
	}
	lastSystemError														= errno;
	if(lastSystemError) {
		::strerror_s(bufferError, (int)lastSystemError);
		size_t																	stringLength								= ::sprintf_s(bufferError, "Last system error: 0x%llX '%s'.", lastSystemError, bufferError);
		base_debug_print(prefix, prefixLen);
		base_debug_print(bufferError, stringLength);
		base_debug_print("\n", 1);
	}
}
