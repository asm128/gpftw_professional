/// Copyright 2010-2018 - asm128
#ifndef CHO_PLATFORM_GLOBALS_H_298365465465
#define CHO_PLATFORM_GLOBALS_H_298365465465

#if (defined(DEBUG) || defined(_DEBUG)) && (!defined(CHO_DISABLE_DEBUG)) && !defined(CHO_DEBUG_ENABLED) 
#	define CHO_DEBUG_ENABLED
#	if !defined(USE_DEBUG_BREAK_ON_ERROR_LOG)
#		define USE_DEBUG_BREAK_ON_ERROR_LOG
#	endif
#endif	// _DEBUG

#if (!defined(CHO_MTSUPPORT)) && !defined(CHO_DISABLE_MTSUPPORT)
#	define CHO_MTSUPPORT
#endif

#if (!defined(CHO_ANDROID)) && defined(ANDROID) 
#	define CHO_ANDROID
#elif (!defined(CHO_LINUX)) && defined(__linux__)
#	define CHO_LINUX
#elif (!defined(CHO_WINDOWS)) && (defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64))
#	define CHO_WINDOWS
#else
#	error "Unsupported platform."
#	define CHO_UNKNOWN
#endif

#ifdef __GNUC__
#	define UNUSED __attribute__((__unused__))
#elif defined(CHO_WINDOWS)
#	ifdef _DEBUG
#		define UNUSED 
#	else
#		define UNUSED 
#	endif
#else
#	define UNUSED
#endif

#if defined(CHO_WINDOWS)
//#	pragma warning(disable : 4592)		// We needed this because of a bug in the compiler causing our static const variables to throw a warning as if its constructor was defined as constexpr.
#	pragma warning(disable : 6326)		// Bogus potential comparison of a constant with another constant for static template functionality.
#	pragma warning(disable : 4706)		// Enable assignment within conditional expression. We do this constantly inside our macros in a completely valid way.
// GSL / Guide for Super Lammers who want to Ban C
#	pragma warning(disable : 26461)		// Enable not marking static arrays as const because they're already const.
#	pragma warning(disable : 26481)		// Enable pointer arithmetic because our library needs to be able to do pointer arithmetic in some places.
#	pragma warning(disable : 26482)		// Enable indexing arrays without constexpr x.X suck a dick souprot & sucker.
#	pragma warning(disable : 26485)		// Enable no pointer decay because being forced to make it decay makes absolutely no sense.
#	pragma warning(disable : 26493)		// Enable C-style casts because they are safe and straightforward.
#	pragma warning(disable : 26495)		// Not always initialize a member variable because we initialize plenty of structs in the C-style because it's faster and safe.
#	pragma warning(disable : 26496)		// Enable non-const for() boundaries because we can control scope better of our counter variables.
#endif

#if defined(CHO_WINDOWS)
#	if !defined(NOMINMAX)
#		define NOMINMAX
#	endif 
#	if !defined(WIN32_LEAN_AND_MEAN)
#		define WIN32_LEAN_AND_MEAN
#	endif
#	define CHO_DYNAMIC_LIBRARY_EXTENSION "dll"
#else
#	define CHO_DYNAMIC_LIBRARY_EXTENSION "os"
#endif

#endif // CHO_PLATFORM_GLOBALS_H_298365465465
