#ifndef __MULTIOS_H__
#define __MULTIOS_H__

#ifdef _WIN32
/* Win32 */
#define getcwd(buf, size) _getcwd(buf, size)
#define timeb _timeb
#define ftime(t) _ftime(t)

#else
/* pthreads */

#endif

#endif /*__MULTIOS_H__*/