#include "string.h"
#include "main.h"
#include "ctype.h"

size_t strlen( const char * str )
{
	unsigned i;
	for( i=0u; str[i]; ++i );
	return i;
}

int strcmp( const char *str1, const char *str2 )
{
	for( ; (*str1==*str2) && *str1; ++str1, ++str2 );
	return *str1-*str2;
}

int stricmp( const char *str1, const char *str2 )
{
	for( ; (tolower(*str1)==tolower(*str2)) && *str1; ++str1,++str2 );
	return tolower(*str1)-tolower(*str2);
}

int strncmp( const char *str1, const char *str2, size_t num )
{
	unsigned i;
	for( i=0; i<num && (str1[i]==str2[i]) && *str1; ++i );
	if(i==num)
		return 0;
	return str1[i]-str2[i];
}

int strnicmp(const char *str1, const char *str2, size_t num)
{
	unsigned i;
	for( i=0; i<num && (tolower(str1[i])==tolower(str2[i])) && str1[i]; ++i );
	if(i==num)
		return 0;
	return tolower(str1[i])-tolower(str2[i]);
}

char *strcpy( char *destination, const char *source )
{
	unsigned i;
	for( i=0u; source[i]; ++i )
		destination[i] = source[i];
	destination[i] = '\0';
	return destination;
}

char *strncpy( char *destination, const char *source, size_t num )
{
	unsigned i;
	for( i=0u; source[i] && i<num; ++i )
		destination[i] = source[i];
	for( ; i<num ; ++i )
		destination[i] = '\0';
	return destination;
}

void *memcpy( void *destination, const void *source, size_t num )
{
	unsigned i;
	for( i=0u; i<(num/sizeof(unsigned)); ++i )
		((unsigned*)destination)[i] = ((unsigned*)source)[i];
	i *= sizeof( unsigned );
	for( ; i<num; ++i )
		((char*)destination)[i] = ((char*)source)[i];
	return destination;
}

int memcmp( const void * ptr1, const void * ptr2, size_t num )
{
	unsigned i;
	for( i=0u; i<num && ((BYTE*)ptr1)[i]==((BYTE*)ptr2)[i]; ++i );
	if( i==num )
		return 0;
	return ((BYTE*)ptr1)[i]-((BYTE*)ptr2)[i];
}

void *memset( void * ptr, int value, size_t num )
{
	unsigned i;
	for( i=0u; i<num; ++i )
		((BYTE*)ptr)[i] = value;
	return ptr;
}

void *memmove( void *dest, const void *src, size_t count )
{
	unsigned i;
	if( !count )
		return dest;
	if( src<dest )
	{
		for( i=count; i%sizeof(unsigned); --i )
			((char*)dest)[i-1] = ((char*)src)[i-1];
		i /= sizeof( unsigned );
		for( ; i>0; --i )
		((unsigned*)dest)[i-1] = ((unsigned*)src)[i-1];
	} else {
		for( i=0u; i<(count/sizeof(unsigned)); ++i )
		((unsigned*)dest)[i] = ((unsigned*)src)[i];
		i *= sizeof( unsigned );
		for( ; i<count; ++i )
			((char*)dest)[i] = ((char*)src)[i];
	}
	return dest;
}
