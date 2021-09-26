// Standard library, alei 29Jul97

// Strings are NULL-terminated.
// Some functions like _fmemcpy(), are optimized by MS Compiler to in-line functions.

#ifndef __SYNC_STANDARD_H
#define __SYNC_STANDARD_H

// string.h and memory.h from msvc
#include "string.h"
#include "memory.h"


#define memcpy(a, b, n) _fmemcpy(a, b, n)
#define memcmp(a, b, n) _fmemcmp(a, b, n)
#define memset(p, c, n) _fmemset(p, c, n)

#define strcpy(a, b) _fstrcpy(a, b)
#define strncpy(a, b, n) _fstrncpy(a, b, n)
#define strcat(a, b) _fstrcat(a, b)
#define strncat(a, b, n) _fstrncat(a, b, n)

#define strtok(a, b) _fstrtok(a, b)
#define strchr(s, c) _fstrchr(s, c)
#define strlen(a) _fstrlen(a)
#define strset(s, c) _fstrset(s, c)

#define strcmp(a, b) _fstrcmp(a, b)
#define stricmp(a, b) _fstricmp(a, b)
#define strncmp(a, b, n) _fstrncmp(a, b, n)
#define strnicmp(a, b, n) _fstrnicmp(a, b, n)

#define strspn(a, b) _fstrspn(a, b)

#endif
