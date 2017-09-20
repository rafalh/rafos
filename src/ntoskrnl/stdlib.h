#ifndef _STDLIB_H_
#define	_STDLIB_H_

#include <sysdef.h>

#define abs(n) (((n)<0)?(-(n)):(n))

typedef unsigned errno_t;

//int abs( int n );
long int labs( long int n );
char *itoa( int value, char *str, int base );
int atoi( const char * str );
long int atol( const char * str );
long int strtol( const char *str, const char **endptr, int base );
unsigned long int strtoul( const char *str, const char **endptr, int base );

void *bsearch( const void *key, const void *base, size_t num, size_t size, int ( *comparator )( const void *, const void * ) );

errno_t _itoa_s( int value, char *buffer, size_t sizeInCharacters, int radix );

#endif /* _STDLIB_H_ */
