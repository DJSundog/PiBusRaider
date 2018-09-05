
#pragma once

#include "globaldefs.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void W32(unsigned int addr, unsigned int val);
extern unsigned int R32(unsigned int addr);

extern int timer_isTimeout(unsigned long curTime, unsigned long lastTime, unsigned long maxDuration);

extern void system_init();

extern char* strncpy(char* dest, const char* src, size_t num);
extern int strncmp(const char* str1, const char* str2, size_t num);
extern int stricmp(const char* str1, const char* str2);
extern const char *strstr(const char* string, const char* substring);
extern char rdtolower(char c);
extern char rdisupper (unsigned char c);
extern int rdisdigit(int c);
extern char rdisspace (unsigned char c);
extern int rdisalpha(int c);

extern bool jsonGetValueForKey(const char* srchKey, const char* jsonStr, char* pOutStr, int outStrMaxLen);

extern long strtol(const char *nptr, char **endptr, register int base);

extern void * memcpy (void *dest, const void *src, size_t len);

// extern void __aeabi_unwind_cpp_pr0(void);
// extern void __cxa_end_cleanup(void);

// Error handler for pure virtual functions
extern void __cxa_pure_virtual();

#ifdef __cplusplus
}
#endif