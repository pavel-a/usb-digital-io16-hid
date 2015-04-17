#ifdef _MSC_VER

#pragma once

// Target Windows XP or later.
// Note: for compat. with VS < 2010 not using sdkddkver.h
#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#if _MSC_VER < 1900 /* before VC++ 2015 */
#define _CRT_SECURE_NO_WARNINGS
#endif

#endif /*MSC_VER*/
