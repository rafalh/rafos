#include "fat.h"
#include "vfs.h"
#include "memory.h"
#include "string.h"

struct filesystem_t fat_fs =	{ fat_init,	fat_destroy,	fat_getinfo,	fat_opendir,	fat_closedir,	fat_readdir };
struct filesystem_t fat16_fs =	{ NULL,		NULL,			NULL,			NULL,			NULL,			NULL };
struct filesystem_t fat32_fs =	{ NULL,		NULL,			NULL,			NULL,			NULL,			NULL };

#define FatLookup12BitEntry(FAT, INDEX, ENTRY) {                            \
                                                                            \
    (ENTRY) = *(UINT16*)(((BYTE*)(FAT))+(INDEX)*3/2);                        \
                                                                            \
    (ENTRY) = (0xfff & (((INDEX) & 1) ? ((ENTRY) >> 4) :                    \
                                        (ENTRY) ));                         \
}

void mem_dump( void *ptr, unsigned len );

int fat_readfile( struct volume_t *vol, char *buf, unsigned c, unsigned cluster )
{
	unsigned c2, offset, data_offset, cluster2;
	if(cluster==0)
	{
		offset = (((fat_bs_t*)vol->param)->reserved_sector_count + (((fat_bs_t*)vol->param)->table_count * ((fat_bs_t*)vol->param)->table_size_16))*((fat_bs_t*)vol->param)->bytes_per_sector;
		if( vol->dev->read( vol->dev, buf, offset, ((fat_bs_t*)vol->param)->root_entry_count*sizeof(fat_dir_entry_t) ) )
			return -1;
		return 0;
	}
	data_offset = (((fat_bs_t*)vol->param)->reserved_sector_count + ((fat_bs_t*)vol->param)->table_count*((fat_bs_t*)vol->param)->table_size_16 + (((fat_bs_t*)vol->param)->root_entry_count*sizeof(fat_dir_entry_t)+((fat_bs_t*)vol->param)->bytes_per_sector-1)/((fat_bs_t*)vol->param)->bytes_per_sector)*((fat_bs_t*)vol->param)->bytes_per_sector;
	do
	{
		c2 = min( c, ((fat_bs_t*)vol->param)->sectors_per_cluster*((fat_bs_t*)vol->param)->bytes_per_sector );
		offset = ((cluster-2)*((fat_bs_t*)vol->param)->sectors_per_cluster*((fat_bs_t*)vol->param)->bytes_per_sector+data_offset);
		if( vol->dev->read( vol->dev, buf, offset, c2 ) )
			return -1;

		FatLookup12BitEntry( ((fat_data_t*)vol->param)->fat, cluster, cluster2 );
		cluster = cluster2;
		//printf("fat[%x] = %x\n", cluster, cluster2);
		c -= c2;
	} while( cluster<0xff7 && c );
	if( cluster==0xff7 )
		return -1;
	return 0;
}

int fat_init( struct volume_t *vol )
{
    puts("fat_init");
	fat_data_t *fat_data;
	unsigned total_clusters, root_dir_sectors, data_sectors;
	int i;

	if( !(fat_data = (fat_data_t*)malloc( sizeof(*fat_data) )) )
		return -1;
	if( vol->dev->read( vol->dev, (BYTE*)&fat_data->bs, 0, 512 ) )
		goto e_free_fat_data;
	if( !(fat_data->bs.bytes_per_sector) || !(fat_data->bs.sectors_per_cluster) )
		goto e_free_fat_data;
    puts("tu");
	root_dir_sectors = ((fat_data->bs.root_entry_count * 32) + (fat_data->bs.bytes_per_sector - 1)) / (fat_data->bs.bytes_per_sector);
	data_sectors = fat_data->bs.total_sectors_16 - (fat_data->bs.reserved_sector_count + (fat_data->bs.table_count * fat_data->bs.table_size_16) + root_dir_sectors);
	total_clusters = data_sectors / fat_data->bs.sectors_per_cluster;
	puts("tu2");
	if(total_clusters > 4084 && !(((fat_extbs_t*)fat_data->bs.extended_section)->signature==0x28 || ((fat_extbs_t*)fat_data->bs.extended_section)->signature==0x29)) /* Nie FAT12 */
		goto e_free_fat_data;
	if( !(fat_data->fat = (BYTE*)malloc( fat_data->bs.table_size_16*fat_data->bs.bytes_per_sector )) ) /* File Allocation Table */
		goto e_free_fat_data;
	if( vol->dev->read( vol->dev, fat_data->fat, fat_data->bs.reserved_sector_count*fat_data->bs.bytes_per_sector, fat_data->bs.table_size_16*fat_data->bs.bytes_per_sector ) )
		{ free( fat_data->fat ); goto e_free_fat_data; }

	vol->param = (LPARAM)fat_data;
	return 0;
  e_free_fat_data: /* error */
	free( fat_data );
	return -1;
}

int fat_destroy( struct volume_t *vol )
{
	free( ((fat_data_t*)vol->param)->fat );
	free( (void*)vol->param );
	return 0;
}

int fat_getinfo( struct volume_t *vol, UINT32 *serial_nr, char *label, unsigned labelsize, unsigned *max_comp_len )
{
	if( serial_nr )
		*serial_nr = ((fat_extbs_t*)((fat_data_t*)vol->param)->bs.extended_section)->volume_id;
	if( label )
	{
		int i;
		i = min( labelsize, 11 );
		strncpy( label, ((fat_extbs_t*)((fat_data_t*)vol->param)->bs.extended_section)->volume_label, i );
		for( ; i>=0 && label[i]==' '; --i );
		label[i+1] = '\0';
	}
	if( max_comp_len )
		*max_comp_len = 11;
}

int fat_opendir( struct volume_t *vol, dir_t *dir, const char *path, dir_t **result )
{
	unsigned len, i, dir_offset, data_offset;
	fat_dir_entry_t *entry;

	*result = (dir_t*)malloc( sizeof(*dir) );
	(*result)->i = 0;
	data_offset = (((fat_bs_t*)vol->param)->reserved_sector_count + ((fat_bs_t*)vol->param)->table_count*((fat_bs_t*)vol->param)->table_size_16 + (((fat_bs_t*)vol->param)->root_entry_count*sizeof(fat_dir_entry_t)+((fat_bs_t*)vol->param)->bytes_per_sector-1)/((fat_bs_t*)vol->param)->bytes_per_sector)*((fat_bs_t*)vol->param)->bytes_per_sector;

	if(dir==FS_ROOT_DIR)
	{
		dir_offset = (((fat_bs_t*)vol->param)->reserved_sector_count + (((fat_bs_t*)vol->param)->table_count * ((fat_bs_t*)vol->param)->table_size_16))*((fat_bs_t*)vol->param)->bytes_per_sector;
		(*result)->c = ((fat_bs_t*)vol->param)->root_entry_count;
		(*result)->data = malloc( (*result)->c*sizeof(fat_dir_entry_t) );
		if( vol->dev->read( vol->dev, (BYTE*)(*result)->data, dir_offset, (*result)->c*sizeof(fat_dir_entry_t) ) )
			return -1;
	} else {
		(*result)->c = dir->c;
		(*result)->data = malloc( (*result)->c*sizeof(fat_dir_entry_t) );
		memcpy( (*result)->data, dir->data, (*result)->c*sizeof(fat_dir_entry_t) );
	}

	for( ; path[0]; )
	{
		unsigned j;
		for( len=0; path[len] && path[len]!='\\' && path[len]!='/'; ++len );
		if( len>8 )
			return -1;
		for( i=0; i<((*result)->c); ++i )
		{
			entry = &((fat_dir_entry_t*)(*result)->data)[i];
			for( j=8; j && entry->name[j-1]==' '; --j )
				entry->name[j-1] = '\0';
			if(*((BYTE*)entry) && *((BYTE*)entry)!=0xE5 && (entry->attributes&FAT_DIRECTORY) && j==len && !strnicmp( path, entry->name, len ) )
			{
				path += len;
				if( path[0] )
					++path;

				fat_readfile( vol, (*result)->data, 0xffffffff, entry->first_cluster_lo );
				/*if (entry->first_cluster_lo==0 )
				{
					dir_offset = (((fat_bs_t*)vol->param)->reserved_sector_count + (((fat_bs_t*)vol->param)->table_count * ((fat_bs_t*)vol->param)->table_size_16))*((fat_bs_t*)vol->param)->bytes_per_sector;
					(*result)->c = ((fat_bs_t*)vol->param)->root_entry_count;
				} else {
					dir_offset = (entry->first_cluster_lo-2)*((fat_bs_t*)vol->param)->sectors_per_cluster*((fat_bs_t*)vol->param)->bytes_per_sector+data_offset;
					(*result)->c = ((fat_bs_t*)vol->param)->sectors_per_cluster*((fat_bs_t*)vol->param)->bytes_per_sector/sizeof(fat_dir_entry_t);
				}*/

				//if( vol->dev->read( vol->dev, (BYTE*)(*result)->data, dir_offset, (*result)->c*sizeof(fat_dir_entry_t) ) )
					//break;
				i = 0xffffffff;
				break;
			}
		}
		if(i!=0xffffffff)
			return -1;
	}

	return 0;
}

int fat_closedir( struct volume_t *vol, dir_t *dir )
{
	if(dir==FS_ROOT_DIR)
		return 0;
	free(dir->data);
	free(dir);
	return 0;
}

time_t fat_time( unsigned short date, unsigned short time )
{
	struct tm tm;
	tm.tm_sec = (time&0x1F)<<1; //???
	tm.tm_min = (time>>5)&0x3F;
	tm.tm_hour = (time>>11)&0x1F;
	tm.tm_mday = (date)&0x1F;
	tm.tm_mon = ((date>>5)&0xF)-1;
	tm.tm_year = ((date>>9)&0x7F)+80;
	return mktime( &tm );
}

int fat_readdir( struct volume_t *vol, dir_t *dir, fileinfo_t *fi )
{
	unsigned i, len;
	fat_dir_entry_t *entry;
	struct tm tm;
	if(!dir || dir==FS_ROOT_DIR)
		return -1;
	for( ; (dir->i)<(dir->c); ++(dir->i) )
	{
		entry = &((fat_dir_entry_t*)dir->data)[dir->i];
		if( !(*((BYTE*)entry)) || (*((BYTE*)entry))==0xE5 )
			continue;
		for( len=8; len>0; --len)
			if( entry->name[len-1]!=' ' )
				break;
		//printf("fat_readdir - i: %u, c: %u\n", dir->i, dir->c);
		strncpy( fi->name, entry->name, len );
		fi->name[len] = '\0';
		if( !(entry->attributes&FAT_DIRECTORY) )
		{
			if( entry->name[10]==' ' )
			{
				entry->name[10] = '\0';
				if( entry->name[9]==' ' )
				{
					entry->name[9] = '\0';
					if( entry->name[8]==' ' )
						entry->name[8] = '\0';
				}
			}

			if( entry->name[8] )
			{
				fi->name[len] = '.';
				strncpy( fi->name+len+1, entry->name+8, 3 );
				fi->name[len+4] = '\0';
			}
			fi->attributes = 0;
		} else fi->attributes = FS_DIR;
		fi->size = entry->file_size;
		fi->create_time = fat_time( entry->creation_date, entry->creation_time );
		fi->modify_time = fat_time( entry->modification_date, entry->modification_time );
		fi->last_access_time = fat_time( entry->last_access_date, 0 );
		++(dir->i);
		return 0;
	}
	return -1;
}
