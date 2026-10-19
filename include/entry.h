#pragma once
#ifndef _ENTRY_H
#define _ENTRY_H

#if defined(MSWindows) || defined(_WIN32) || defined(WIN32) || defined(_WINDOWS) || defined(WIN_ENV)
	#ifndef DllExport
		#define DllExport   __declspec( dllexport )
	#endif
#elif defined(macintosh) || defined(__MACH__) || defined(__APPLE__)
	#ifndef DllExport
		#define DllExport   __attribute__((visibility("default")))
	#endif
#else
	#ifndef DllExport
		#define DllExport   __attribute__((visibility("default")))
	#endif
#endif

#endif // _ENTRY_H
