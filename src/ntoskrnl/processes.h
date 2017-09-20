#ifndef _PROCESSES_H_
#define	_PROCESSES_H_

#include "memory.h"

typedef struct process_t
{
	unsigned pid;
	char name[32];
	PGDIR pg_dir;
} process_t;



#endif /* _PROCESSES_H_ */
