#pragma once
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef _WIN32
  #ifndef PF_STRCPY
    #define PF_STRCPY(dst, src) strcpy_s((dst), sizeof(dst), (src))
  #endif
  #ifndef PF_STRCAT
    #define PF_STRCAT(dst, src) strcat_s((dst), sizeof(dst), (src))
  #endif
  #ifndef PF_SPRINTF
    #define PF_SPRINTF sprintf_s
  #endif
#else
  #ifndef PF_STRCPY
    #define PF_STRCPY(dst, src) strncpy((dst), (src), sizeof(dst) - 1)
  #endif
  #ifndef PF_STRCAT
    #define PF_STRCAT(dst, src) strncat((dst), (src), sizeof(dst) - strlen(dst) - 1)
  #endif
  #ifndef PF_SPRINTF
    #define PF_SPRINTF snprintf
  #endif
#endif

#endif // STRING_UTILS_H
