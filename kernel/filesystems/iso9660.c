#include <kernel/filesystems/vfs.h>
#include <kernel/mem/kheap.h>
#include <string.h>
typedef struct
{
    uint8_t years;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t offset;
} __attribute__((packed)) directory_datetime_t ;

typedef struct
{
    uint8_t     length                        ;
    uint8_t     attribute                     ;
    uint32_t    location_lba_lsb             ;
    uint32_t    location_lba_msb             ;
    uint32_t    size_lsb                     ;
    uint32_t    size_msb                     ;
    directory_datetime_t creation_date     ;  
    uint8_t     flags                         ;
    uint8_t     interleaved_unit_size         ;
    uint8_t     interleave_gap_size           ;
    uint16_t    volume_sequence_number_lsb   ;
    uint16_t    volume_sequence_number_msb   ;
    uint8_t     length_file_identifier        ;
    uint8_t     file_identifier[]               ;
} __attribute__((packed)) directory_t;

typedef struct {
    uint8_t     type_code                              ;
    uint8_t     standard_identifier[5]                 ;
    uint8_t     version                                ;
    uint8_t     unused_1                               ;
    uint8_t     system_identifier[32]                  ;
    uint8_t     volume_identifier[32]                  ;
    uint8_t     unused_2[8]                            ;
    uint32_t    volume_space_size_lsb                 ;
    uint32_t    volume_space_size_msb                 ;
    uint8_t     unused_3[32]                           ;
    uint16_t    volume_set_size_lsb                   ;
    uint16_t    volume_set_size_msb                   ;
    uint16_t    volume_sequence_number_lsb            ;
    uint16_t    volume_sequence_number_msb            ;
    uint16_t    logical_block_size_lsb                ;
    uint16_t    logical_block_size_msb                ;
    uint32_t    path_table_size_lsb                   ;
    uint32_t    path_table_size_msb                   ;
    uint32_t    path_table_location_lsb               ;
    uint32_t    optional_path_table_location_lsb      ;
    uint32_t    path_table_location_msb               ;
    uint32_t    optional_path_table_location_msb      ;
    directory_t root_directory                   ;
} __attribute__((packed)) PVD_t;

typedef struct iso9660_fs
{
    uint32_t sector_size;
    PVD_t * pvd;
    vfs_node_t * device;
} iso9660_fs_t;

void read_disk_block(iso9660_fs_t * fs,uint32_t block,char * buffer){
    vfs_read(fs->device,fs->sector_size * block,fs->sector_size,buffer);
}
void iso9660_install(string_t device,string_t mount_point){
    iso9660_fs_t * isofs = (iso9660_fs_t *)kmalloc(sizeof(iso9660_fs_t));
    isofs->device = file_open(device,0);
    isofs->sector_size = 2048;
    isofs->pvd = (PVD_t *)kmalloc(isofs->sector_size);
    read_disk_block(isofs,0x10,(void *)isofs->pvd);
    isofs->sector_size = isofs->pvd->logical_block_size_lsb;
    vfs_node_t * iso9660_vfs_node = kmalloc(sizeof(vfs_node_t));
    strcpy(iso9660_vfs_node->name,"/");
    iso9660_vfs_node->device = (uint32_t)isofs;
    iso9660_vfs_node->address = (uint32_t)isofs->pvd->root_directory.location_lba_lsb;
    iso9660_vfs_node->size = (uint32_t)isofs->pvd->root_directory.size_lsb;
    iso9660_vfs_node->flags = FS_DIRECTORY;
    vfs_mount(mount_point,iso9660_vfs_node);
}