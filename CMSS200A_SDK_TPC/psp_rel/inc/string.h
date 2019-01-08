#ifndef _STRING_H_
#define _STRING_H_

#include "typeext.h"

#ifndef PC
#ifdef __C251__
#pragma SAVE

#pragma ROM(huge)

extern int8 *strcat (int8 *s1, int8 *s2) reentrant;

extern int8 *strncat (int8 *s1, int8 *s2, uint16 n) reentrant;

extern int8 strcmp (int8 *s1, int8 *s2) reentrant;

extern int8 strncmp (int8 *s1, int8 *s2, uint16 n) reentrant;

extern int8 *strcpy (int8 *s1, int8 *s2) reentrant;

extern int8 *strncpy (int8 *s1, int8 *s2, uint16 n) reentrant;

extern uint16 strlen (int8 *) reentrant;

extern int8 *strchr (const int8 *s, int8 c) reentrant;

extern uint16 strpos (const int8 *s, int8 c) reentrant;

extern int8 *strrchr (const int8 *s, int8 c) reentrant;

extern uint16 strrpos (const int8 *s, int8 c) reentrant;

extern uint16 strspn (int8 *s, int8 *set) reentrant;

extern uint16 strcspn (int8 *s, int8 *set) reentrant;

extern int8 *strpbrk (int8 *s, int8 *set) reentrant;

extern int8 *strrpbrk (int8 *s, int8 *set) reentrant;

extern int8 memcmp (void *s1, void *s2, uint16 n) reentrant;

extern void *memcpy (void *s1, void *s2, uint16 n) reentrant;

extern void *memchr (void *s, int8 val, uint16 n) reentrant;

extern void *memccpy (void *s1, void *s2, int8 val, uint16 n) reentrant;

extern void *memmove (void *s1, void *s2, uint16 n) reentrant;

extern void *memset  (void *s, int8 val, uint16 n) reentrant;

extern void far   *fmemset  (void far *s,   int8 val, uint16 n) reentrant;

extern void xdata *xmemset  (void xdata *s, int8 val, uint16 n) reentrant;

extern void far   *fmemcpy  (void far *s1,  void far *s2, uint16 n) reentrant;

extern void xdata *xmemcpy  (void xdata *s1, void xdata *s2, uint16 n) reentrant;

extern void uprintf(const int8 *fmt, ...);
extern void prints(const int8 *str);
extern void printh(const int16 hexdata);
extern void printd(const int16 decdata);
#pragma ROM(huge)



#pragma RESTORE
#endif /*  __C251__ */
#else

extern int8 *strcat (int8 *s1, int8 *s2);
extern int8 *strncat (int8 *s1, int8 *s2, uint16 n);
extern int8 strcmp (int8 *s1, int8 *s2);
extern int8 strncmp (int8 *s1, int8 *s2, uint16 n);
extern int8 *strcpy (int8 *s1, int8 *s2);
extern int8 *strncpy (int8 *s1, int8 *s2, uint16 n);
extern uint16 strlen (int8 *);
extern int8 *strchr (const int8 *s, int8 c);
extern uint16 strpos (const int8 *s, int8 c);
extern int8 *strrchr (const int8 *s, int8 c);
extern uint16 strrpos (const int8 *s, int8 c);
extern uint16 strspn (int8 *s, int8 *set);
extern uint16 strcspn (int8 *s, int8 *set);
extern int8 *strpbrk (int8 *s, int8 *set);
extern int8 *strrpbrk (int8 *s, int8 *set);
extern int8 memcmp (void *s1, void *s2, uint16 n);
extern void *memcpy (void *s1, void *s2, uint16 n);
extern void *memchr (void *s, int8 val, uint16 n);
extern void *memccpy (void *s1, void *s2, int8 val, uint16 n);
extern void *memmove (void *s1, void *s2, uint16 n);
extern void *memset  (void *s, int8 val, uint16 n);
extern void far   *fmemset  (void far *s,   int8 val, uint16 n);
extern void xdata *xmemset  (void xdata *s, int8 val, uint16 n);
extern void far   *fmemcpy  (void far *s1,  void far *s2, uint16 n);
extern void xdata *xmemcpy  (void xdata *s1, void xdata *s2, uint16 n);
extern void uprintf(const int8 *fmt, ...);
extern void prints(const int8 *str);
extern void printh(const int16 hexdata);
extern void printd(const int16 decdata);

// UNICODE ×Ö·û´®³¤¶È
int16 *strlenuni (int8 *s1);
// UNICODE ×Ö·û´®¿½±´
int8 *strcpyuni (int8 *s1, int8 *s2);

#endif /* #ifndef PC */
#endif /* _STRING_H_ */

