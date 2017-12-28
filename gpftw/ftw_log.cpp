#include "ftw_log.h"

#include <Windows.h>

#include <stdio.h>

static	::ftw::error_t												getSystemErrorAsString						(const uint64_t lastError, char* buffer)			{	// Get the error message, if any.
#if defined FTW_WINDOWS
	if(nullptr == buffer)
		return -1;
	if(0 == lastError) 
		return -1;
	const DWORD																size										= ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)(lastError & 0xFFFFFFFF), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 0, NULL);
	return (::ftw::error_t)size;
#endif
}

		void														ftw::_ftw_print_system_errors				(const char* prefix, uint32_t prefixLen)			{
	char																	bufferError[4096]							= {};
	int64_t																	lastSystemError								= ::GetLastError() & 0xFFFFFFFFFFFFFFFFLL;
	if(lastSystemError) {
		::ftw::error_t															stringLength								= ::getSystemErrorAsString((uint64_t)lastSystemError, bufferError);
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
