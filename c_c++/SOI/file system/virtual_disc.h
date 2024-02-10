#ifndef LAB6_1_VIRTUAL_DISC_H
#define LAB6_1_VIRTUAL_DISC_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define MAX_NAME_LENGTH     64

/* Block types (2-bit) */
#define BTYPE_NULL          0x0
#define BTYPE_FILE_HEADER   0x1
#define BTYPE_DIR_HEADER    0x2
#define BTYPE_DATA          0x3

#define BTYPE_MASK          0xc0000000
#define BADDR_MASK          0x3fffffff
#define N_BADDR_BITS        30

#define NULL_BLOCK          0x0

typedef unsigned block_addr; /* last 30 bits used */
typedef unsigned block_type; /* last 2 bits used */
typedef unsigned block_id; /* Has type and address */
typedef unsigned blocks;

struct FieldHeader {
    block_id next_block; /* last block if NULL_BLOCK, first 2 bits are type of current block! */
    size_t actual_size; /* in Bytes */
    blocks block_size; /* in blocks */
    char name[MAX_NAME_LENGTH];
};
typedef struct FieldHeader field_header_t;

/* How much data do blocks hold */
#define BLOCK_SIZE          4096
#define FIELD_BLOCK_CAP     (BLOCK_SIZE - sizeof(field_header_t))
#define DATA_BLOCK_CAP      (BLOCK_SIZE - sizeof(block_id))
#define BITMAP_HEADER       (sizeof(size_t) + sizeof(blocks))

/* --- User level --- */

/* creates disc with all needed headers */
void create_disc(char *disc_path, size_t disc_size);

/* deletes virtual disc */
void remove_disc(char *disc_path);

/* puts a file on the disc */
void insert_into_disc(char *disc_path, char *path_source, char *path_dest_virtual);

/* extracts a file from the disc and puts it in path_dest */
void extract_from_disc(char *disc_path, char *path_source_virtual, char *path_dest);

/* lists given directory */
void list_dir(char *disc_path, char *path);

/* creates a new directory in root (root/<name>) */
void make_directory(char *disc_path, char *name);

/* removes given file or folder but only when it is empty */
void remove_file_or_folder(char *disc_path, char *path);

/* prints disc blocks information */
void print_disc_info(char *disc_path);

/* --- Back-end --- */
block_id write_file_into_system(char *path_source, char *name, char *hole_bitmap, blocks n_of_blocks, FILE *vdisk_fp,
                                size_t bitmap_size);

block_id invalidate_field(block_id field_id, char *hole_bitmap, blocks n_of_blocks, FILE *vdisk_fp,
                          size_t bitmap_size);

field_header_t create_field_header(char *name, size_t actual_size, block_addr next_block_id);

block_addr get_block_address(block_addr block);

block_type get_block_type(block_id block);

block_addr get_block_id(block_type type, block_addr address);

block_addr bytes_to_blocks(size_t bytes);

void validate_name(char *name);

/* returns block_adddr of found field, NULL_BLOCK otherwise */
block_id
find_in_folder_by_name(const char *name, const block_id *dir_items, size_t n_of_items, size_t bitmap_size, FILE *fp);

int get_bit_from_bitmap(const char *hole_bitmap, block_addr pos);

void set_bit_on_bitmap(char *hole_bitmap, block_addr pos);

void unset_bit_on_bitmap(char *hole_bitmap, block_addr pos);

void parse_path(char *path, char **file, char **folder);

/* loads bitmap into memory */
void load_hole_bitmap(FILE *fp, char **hole_bitmap, blocks *n_of_blocks, size_t *bitmap_size);

/* initializes current directory !! FP must already point at the root directory !! */
void get_current_directory(const char *folder, FILE *fp, size_t bitmap_size, block_id *dir_items,
                           field_header_t *current_dir, size_t *n_of_dir_items, block_addr *current_dir_address);

size_t min(size_t a, size_t b);

#endif
