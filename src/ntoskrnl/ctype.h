#ifndef _CTYPE_H_
#define _CTYPE_H_

#define tolower(c) (((c)>='A'&&(c)<='Z')?((c)-'A'+'a'):(c))
#define toupper(c) (((c)>='a'&&(c)<='z')?((c)-'a'+'A'):(c))

#define isdigit(c) (((c)>='0'&&(c)<='9')?1:0)
#define isspace(c) (((c)==' '||(c)=='\t'||(c)=='\n'||(c)=='\v'||(c)=='\f'||(c)=='\f')?1:0)

#endif /* _CTYPE_H_ */
