#ifndef CHO_PLATFORM_H_9028734982734
#define CHO_PLATFORM_H_9028734982734

#if (defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(WIN64)) && !defined(CHO_WINDOWS)
#	define CHO_WINDOWS
#elif defined(__linux__) && !defined(CHO_LINUX)
#	define CHO_LINUX
#endif // CHO_WINDOWS

#if defined(CHO_WINDOWS) && !defined(WIN32_LEAN_AND_MEAN)
#	define WIN32_LEAN_AND_MEAN
#endif 

#define CHO_MTSUPPORT

#endif // CHO_PLATFORM_H_9028734982734