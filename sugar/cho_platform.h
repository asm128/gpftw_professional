#ifndef FTW_PLATFORM_H_9028734982734
#define FTW_PLATFORM_H_9028734982734

#if (defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(WIN64)) && !defined(FTW_WINDOWS)
#	define FTW_WINDOWS
#elif defined(__linux__) && !defined(FTW_LINUX)
#	define FTW_LINUX
#endif // FTW_WINDOWS

#if defined(FTW_WINDOWS) && !defined(WIN32_LEAN_AND_MEAN)
#	define WIN32_LEAN_AND_MEAN
#endif 

#define FTW_MTSUPPORT

#endif // FTW_PLATFORM_H_9028734982734