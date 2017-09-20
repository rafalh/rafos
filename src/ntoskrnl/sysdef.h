#ifndef _SYSDEF_H_
#define	_SYSDEF_H_

//#include <stddef.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef min
#define min(a, b) (((a)>(b))?(b):(a))
#endif

#ifndef max
#define max(a, b) (((a)>(b))?(a):(b))
#endif

#define HIWORD(a) ((a)>>16)
#define LOWORD(a) ((a)&0xffff)

#define MAX_PATH 256

#define IN
#define OUT

typedef unsigned char BYTE, *PBYTE, *LPBYTE;
typedef BYTE BOOL, BOOLEAN;
typedef unsigned short UINT16, USHORT;
typedef unsigned long UINT32, ULONG;
typedef unsigned long long UINT64, ULONGLONG;
typedef unsigned LPARAM;
typedef void VOID, *PVOID, *LPVOID;
typedef UINT32 DWORD, *PDWORD, *LPDWORD;
typedef UINT16 WORD, *PWORD, *LPWORD;
typedef unsigned size_t;
typedef size_t SIZE_T;
typedef LPVOID HINSTANCE, HANDLE;

# define DLLEXPORT __declspec (dllexport)

#endif /* _SYSDEF_H_ */
