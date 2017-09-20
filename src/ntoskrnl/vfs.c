#include "vfs.h"

volume_t lpVolumes[24]={NULL};
filesystem_t lpFilesys[24];
unsigned cFilesys=0;

int LoadFS( struct filesystem_t *fs )
{
	if( cFilesys>=24 )
		return -1;
	memcpy( &lpFilesys[cFilesys], fs, sizeof(*fs) );
	++cFilesys;
	return 0;
}

int MountVolume( unsigned char i, struct volume_t *volume )
{
	for(; i<24; ++i)
		if(!lpVolumes[i].dev)
		{
			memcpy( &lpVolumes[i], volume, sizeof(*volume) );
			return i;
		}
	return -1;
}
