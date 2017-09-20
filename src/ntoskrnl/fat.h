#ifndef _FAT_H_
#define _FAT_H_

#include "vfs.h"

extern struct filesystem_t fat_fs;
extern struct filesystem_t fat16_fs;
extern struct filesystem_t fat32_fs;

#pragma pack(push, 1)

#define FAT_READ_ONLY 0x01
#define FAT_HIDDEN 0x02
#define FAT_SYSTEM 0x04
#define FAT_VOLUME_ID 0x08
#define FAT_DIRECTORY 0x10
#define FAT_ARCHIVE 0x20

typedef struct fat_bs_t
{
    unsigned char   bootjmp[3];
    unsigned char   oem_name[8];
    unsigned short  bytes_per_sector;
    unsigned char   sectors_per_cluster;
    unsigned short  reserved_sector_count;
    unsigned char   table_count;
    unsigned short  root_entry_count;
    unsigned short  total_sectors_16;
    unsigned char   media_type;
    unsigned short  table_size_16;
    unsigned short  sectors_per_track;
    unsigned short  head_side_count;
    unsigned long   hidden_sector_count;
    unsigned long   total_sectors_32;
    
    //this will be cast to it's specific type once the driver actually knows what type of FAT this is.
    unsigned char   extended_section[512 - 36]; /* reszta sektora */
    
} fat_bs_t;

typedef struct fat_extbs_t
{
    //extended fat12 and fat16 stuff
    unsigned char       bios_drive_num;
    unsigned char       reserved1;
    unsigned char       signature;
    unsigned long       volume_id;
    unsigned char       volume_label[11];
    unsigned char       fat_type_label[8];
    
} fat_extbs_t;

typedef struct fat_extbs_32_t
{
    //extended fat32 stuff
    unsigned long       table_size_32;
    unsigned short      extended_flags;
    unsigned short      fat_version;
    unsigned long       root_cluster;
    unsigned short      fat_info;
    unsigned short      backup_BS_sector;
    unsigned char       reserved_0[12];
    unsigned char       drive_number;
    unsigned char       reserved_1;
    unsigned char       boot_signature;
    unsigned long       volume_id;
    unsigned char       volume_label[11];
    unsigned char       fat_type_label[8];
    
} fat_extbs_32_t;

typedef struct fat_dir_entry_t
{
    char            name[11];
    unsigned char   attributes;
    unsigned char   reserved;
    unsigned char   creation_time_second_tenths;
    unsigned short  creation_time;
    unsigned short  creation_date;
    unsigned short  last_access_date;
    unsigned short  first_cluster_hi;
    unsigned short  modification_time;
    unsigned short  modification_date;
    unsigned short  first_cluster_lo;
    unsigned long   file_size;
} fat_dir_entry_t;

typedef struct fat_data_t
{
    fat_bs_t bs;
    BYTE *fat;
} fat_data_t;

int fat_init(struct volume_t *vol);
int fat_destroy(struct volume_t *vol);
int fat_getinfo(struct volume_t *vol, UINT32 *serial_nr, char *label, unsigned labelsize, unsigned *max_comp_len);
int fat_opendir(struct volume_t *vol, dir_t *dir, const char *path, dir_t **result);
int fat_closedir(struct volume_t *vol, dir_t *dir);
int fat_readdir(struct volume_t *vol, dir_t *dir, fileinfo_t *fi);
time_t fat_time(unsigned short date, unsigned short time);

#pragma pack(pop)

#endif /* _FAT_H_ */
