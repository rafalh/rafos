#ifndef _VFS_H_
#define	_VFS_H_

#include "device.h"
#include "time.h"
#include "main.h"

struct volume_t;
typedef struct dir_t
{
	void *data;
	unsigned i;
	unsigned c;
} dir_t;

#define FS_ROOT_DIR NULL
#define FS_DIR 1

typedef struct fileinfo_t
{
	char name[MAX_PATH];
	unsigned size;
	time_t create_time;
	time_t last_access_time;
	time_t modify_time;
	unsigned attributes;
} fileinfo_t;

typedef struct filesystem_t
{
	int (*init)( struct volume_t *vol );
	int (*destroy)( struct volume_t *vol );
	int (*getinfo)( struct volume_t *vol, UINT32 *serial_nr, char *label, unsigned labelsize, unsigned *max_comp_len );
	int (*opendir)( struct volume_t *vol, dir_t *dir, const char *path, dir_t **result );
	int (*closedir)( struct volume_t *vol, dir_t *dir );
	int (*readdir)( struct volume_t *vol, dir_t *dir, fileinfo_t *fi );
} filesystem_t;

typedef struct volume_t
{
	io_dev_t *dev;
	UINT64 start_sector;
	UINT64 count;
	filesystem_t *fs;
	LPARAM param;
} volume_t;

extern volume_t lpVolumes[24];
extern filesystem_t lpFilesys[24];
extern unsigned cFilesys;

int LoadFS( struct filesystem_t *fs );
int MountVolume( unsigned char i, struct volume_t *volume );

#endif /* _VFS_H_ */
