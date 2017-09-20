#ifndef _EXCEPTIONS_H_
#define	_EXCEPTIONS_H_

void exception_proc( unsigned n, unsigned eflags, unsigned ds, unsigned es, unsigned fs, unsigned gs, unsigned edi, unsigned esi, unsigned ebp, unsigned esp, unsigned ebx, unsigned edx, unsigned ecx, unsigned eax, unsigned error_code, unsigned addr );

#endif /* _EXCEPTIONS_H_ */
