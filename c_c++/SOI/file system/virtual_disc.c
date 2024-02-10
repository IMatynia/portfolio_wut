#include "virtual_disc.h"

/* --- User level --- */

void create_disc(char *disc_path, size_t disc_size) {
    /* evaluate how many blocks are needed + hole bitmap + root folder description */
    blocks n_blocks = bytes_to_blocks(disc_size) + 1;
    size_t bitmap_size = n_blocks / 8 + (n_blocks % 8 != 0);
    bitmap_size += BITMAP_HEADER;

    if (access(disc_path, F_OK) == 0) {
        perror("Disc already exists!");
        exit(1);
    }

    FILE *fp = fopen(disc_path, "w");

    /* write empty bitmap */
    fwrite(&bitmap_size, sizeof bitmap_size, 1, fp);
    fwrite(&n_blocks, sizeof n_blocks, 1, fp);

    for (int i = 0; i < bitmap_size - BITMAP_HEADER; i++) {
        if (i == 0) {
            fputc(1, fp);
        } else {
            fputc(0, fp);
        }

    }
    /* write root folder header */
    field_header_t root_folder = create_field_header("root", 0, get_block_id(BTYPE_DIR_HEADER, NULL_BLOCK));
    fwrite(&root_folder, sizeof(field_header_t), 1, fp);

    /* clear disc contents */
    for (int i = 0; i < BLOCK_SIZE * n_blocks + FIELD_BLOCK_CAP; i++) {
        fputc(0, fp);
    }
    fclose(fp);
}

void remove_disc(char *disc_path) {
    remove(disc_path);
}

void insert_into_disc(char *disc_path, char *path_source, char *path_dest_virtual) {
    char *file = NULL;
    char *folder = NULL;
    char buffer[BLOCK_SIZE];
    FILE *fp = NULL;

    char *hole_bitmap;
    blocks n_of_blocks; /* total number of blocks */
    size_t bitmap_size; /* total size of the bitmap segment */

    block_id dir_items[FIELD_BLOCK_CAP / sizeof(block_id)];

    field_header_t current_dir;
    size_t n_of_dir_items;
    block_addr current_dir_address = 0;

    /* parse path */
    parse_path(path_dest_virtual, &file, &folder);

    fp = fopen(disc_path, "r+");
    if (fp == NULL) {
        perror("Couldnt open virtual disc!");
        exit(1);
    }

    /* get file bitmap */
    load_hole_bitmap(fp, &hole_bitmap, &n_of_blocks, &bitmap_size);

    /* get root folder header and its items */
    get_current_directory(folder, fp, bitmap_size, dir_items, &current_dir, &n_of_dir_items, &current_dir_address);

    /* check if folder can fit this */
    if (current_dir.actual_size + sizeof(block_id) > FIELD_BLOCK_CAP) {
        fclose(fp);
        perror("Maximum number of files in this folder has been reached!");
        exit(1);
    }


    /* check uniqueness */
    if (find_in_folder_by_name(file, dir_items, n_of_dir_items, bitmap_size, fp) != NULL_BLOCK) {
        fclose(fp);
        perror("This name is not unique!");
        exit(1);
    }

    /* create the file */
    block_id new_file_id = write_file_into_system(path_source, file, hole_bitmap, n_of_blocks, fp, bitmap_size);

    /* add it to the folder */
    dir_items[n_of_dir_items] = new_file_id;
    current_dir.actual_size += sizeof new_file_id;
    n_of_dir_items++;

    /* update bitmap and folder */
    fseek(fp, BITMAP_HEADER, SEEK_SET);
    fwrite(hole_bitmap, 1, bitmap_size - BITMAP_HEADER, fp);
    fseek(fp, bitmap_size + BLOCK_SIZE * current_dir_address, SEEK_SET);
    fwrite(&current_dir, sizeof(field_header_t), 1, fp);
    fwrite(dir_items, sizeof(block_addr), n_of_dir_items, fp);
    fclose(fp);
}

void extract_from_disc(char *disc_path, char *path_source_virtual, char *path_dest) {
    char *file = NULL;
    char *folder = NULL;
    char buffer[BLOCK_SIZE];
    FILE *fp = NULL;

    char *hole_bitmap;
    blocks n_of_blocks; /* total number of blocks */
    size_t bitmap_size; /* total size of the bitmap segment */

    block_id dir_items[FIELD_BLOCK_CAP / sizeof(block_id)];

    field_header_t current_dir;
    size_t n_of_dir_items;
    block_addr current_dir_address = 0;

    /* parse path */
    parse_path(path_source_virtual, &file, &folder);

    fp = fopen(disc_path, "r+");
    if (fp == NULL) {
        perror("Couldnt open virtual disc!");
        exit(1);
    }

    /* get file bitmap */
    load_hole_bitmap(fp, &hole_bitmap, &n_of_blocks, &bitmap_size);

    /* get root folder header and its items */
    get_current_directory(folder, fp, bitmap_size, dir_items, &current_dir, &n_of_dir_items, &current_dir_address);

    /* grab block_id of the file */
    block_id file_to_extract = find_in_folder_by_name(file, dir_items, n_of_dir_items, bitmap_size, fp);
    block_addr adress_to_extract = get_block_address(file_to_extract);

    /* make sure its a file... */
    if (get_block_type(file_to_extract) != BTYPE_FILE_HEADER || adress_to_extract == NULL_BLOCK) {
        fclose(fp);
        perror("This is not a file!");
        exit(1);
    }

    /* open the external file */
    FILE *dest_fp = fopen(path_dest, "w");
    if (dest_fp == NULL) {
        fclose(fp);
        perror("Couldnt open destination file!");
        exit(1);
    }

    block_id *next_block_id;
    field_header_t *extracted_file_header;
    size_t bytes_remaining;

    /* carry it over to the external location */
    fseek(fp, bitmap_size + BLOCK_SIZE * adress_to_extract, SEEK_SET);
    fread(buffer, 1, BLOCK_SIZE, fp);

    extracted_file_header = (field_header_t *) buffer;
    next_block_id = (block_id *) buffer;
    bytes_remaining = extracted_file_header->actual_size;

    size_t bytest_to_extract_now = min(bytes_remaining, FIELD_BLOCK_CAP);
    bytes_remaining -= bytest_to_extract_now;
    fwrite(buffer + sizeof(field_header_t), 1, bytest_to_extract_now, dest_fp);

    while (bytes_remaining > 0 && get_block_address(*next_block_id) != NULL_BLOCK) {
        fseek(fp, bitmap_size + BLOCK_SIZE * get_block_address(*next_block_id), SEEK_SET);
        fread(buffer, 1, BLOCK_SIZE, fp);

        next_block_id = (block_id *) buffer;
        bytest_to_extract_now = min(bytes_remaining, DATA_BLOCK_CAP);
        bytes_remaining -= bytest_to_extract_now;
        fwrite(buffer + sizeof(block_id), 1, bytest_to_extract_now, dest_fp);
    }

    /* close files */
    fclose(fp);
    fclose(dest_fp);
}

void list_dir(char *disc_path, char *path) {
    char *file = NULL;
    char *folder = NULL;
    char buffer[BLOCK_SIZE];
    FILE *fp = NULL;

    char *hole_bitmap;
    blocks n_of_blocks; /* total number of blocks */
    size_t bitmap_size; /* total size of the bitmap segment */

    block_id dir_items[FIELD_BLOCK_CAP / sizeof(block_id)];

    field_header_t current_dir;
    size_t n_of_dir_items;
    block_addr current_dir_address = 0;

    /* parse path */
    parse_path(path, &file, &folder);
    folder = file;

    fp = fopen(disc_path, "r+");
    if (fp == NULL) {
        perror("Couldnt open virtual disc!");
        exit(1);
    }
    /* get file bitmap */
    load_hole_bitmap(fp, &hole_bitmap, &n_of_blocks, &bitmap_size);

    /* get root folder header and its items */
    get_current_directory(folder, fp, bitmap_size, dir_items, &current_dir, &n_of_dir_items, &current_dir_address);

    if (folder == NULL) folder = "root";
    printf("Listing directory %s:\n\nType\t| Real size \t| Disc size \t| Name\n", folder);
    /* iterate on items of the folder */
    block_id temp_id;
    block_addr temp_addr;
    block_type temp_type;
    char type_char;

    field_header_t temp_header;
    for (int i = 0; i < n_of_dir_items; i++) {
        temp_id = *(dir_items + i);
        temp_addr = get_block_address(temp_id);
        temp_type = get_block_type(temp_id);

        fseek(fp, BLOCK_SIZE * temp_addr + bitmap_size, SEEK_SET);
        fread(&temp_header, sizeof(field_header_t), 1, fp);

        switch (temp_type) {
            case BTYPE_FILE_HEADER:
                type_char = 'F';
                break;
            case BTYPE_DIR_HEADER:
                type_char = 'D';
                break;
            default:
                type_char = 'X';
                break;
        }

        printf("%c       | %12dB | %12dB | %s\n", type_char, (int) temp_header.actual_size,
               temp_header.block_size * BLOCK_SIZE,
               temp_header.name);
    }
    fclose(fp);
}

void make_directory(char *disc_path, char *name) {
    char *file = NULL;
    char *folder = NULL;
    char buffer[BLOCK_SIZE];
    FILE *fp = NULL;

    char *hole_bitmap;
    blocks n_of_blocks; /* total number of blocks */
    size_t bitmap_size; /* total size of the bitmap segment */

    block_id dir_items[FIELD_BLOCK_CAP / sizeof(block_id)];

    field_header_t current_dir;
    size_t n_of_dir_items;
    block_addr current_dir_address = 0;

    /* parse path */
    folder = NULL;
    file = name;

    fp = fopen(disc_path, "r+");
    if (fp == NULL) {
        perror("Couldnt open virtual disc!");
        exit(1);
    }

    /* get file bitmap */
    load_hole_bitmap(fp, &hole_bitmap, &n_of_blocks, &bitmap_size);

    /* get root folder header and its items */
    get_current_directory(folder, fp, bitmap_size, dir_items, &current_dir, &n_of_dir_items, &current_dir_address);

    /* check if folder can fit this */
    if (current_dir.actual_size + sizeof(block_id) > FIELD_BLOCK_CAP) {
        fclose(fp);
        perror("Maximum number of files in this folder has been reached!");
        exit(1);
    }

    /* check uniqueness */
    if (find_in_folder_by_name(file, dir_items, n_of_dir_items, bitmap_size, fp) != NULL_BLOCK) {
        fclose(fp);
        perror("This name is not unique!");
        exit(1);
    }

    /* find a free hole */
    block_addr folder_address = 1;
    while (folder_address < n_of_blocks && get_bit_from_bitmap(hole_bitmap, folder_address)) {
        folder_address++;
    }

    field_header_t new_folder = create_field_header(name, 0, get_block_id(BTYPE_DIR_HEADER, NULL_BLOCK));
    fseek(fp, bitmap_size + BLOCK_SIZE * folder_address, SEEK_SET);
    fwrite(&new_folder, sizeof new_folder, 1, fp);

    dir_items[n_of_dir_items] = get_block_id(BTYPE_DIR_HEADER, folder_address);
    n_of_dir_items++;
    current_dir.actual_size += sizeof folder_address;
    set_bit_on_bitmap(hole_bitmap, folder_address);

    /* update bitmap and folder */
    fseek(fp, BITMAP_HEADER, SEEK_SET);
    fwrite(hole_bitmap, 1, bitmap_size - BITMAP_HEADER, fp);
    fseek(fp, bitmap_size + BLOCK_SIZE * current_dir_address, SEEK_SET);
    fwrite(&current_dir, sizeof(field_header_t), 1, fp);
    fwrite(dir_items, sizeof(block_addr), n_of_dir_items, fp);
    fclose(fp);
}

void remove_file_or_folder(char *disc_path, char *path) {
    char *file = NULL;
    char *folder = NULL;
    char buffer[BLOCK_SIZE];
    FILE *fp = NULL;

    char *hole_bitmap;
    blocks n_of_blocks; /* total number of blocks */
    size_t bitmap_size; /* total size of the bitmap segment */

    block_id dir_items[FIELD_BLOCK_CAP / sizeof(block_id)];

    field_header_t current_dir;
    field_header_t removed_dir;
    size_t n_of_dir_items;
    block_addr current_dir_address = 0;

    /* parse path */
    parse_path(path, &file, &folder);

    fp = fopen(disc_path, "r+");
    if (fp == NULL) {
        perror("Couldnt open virtual disc!");
        exit(1);
    }

    /* get file bitmap */
    load_hole_bitmap(fp, &hole_bitmap, &n_of_blocks, &bitmap_size);

    /* get root folder header and its items */
    get_current_directory(folder, fp, bitmap_size, dir_items, &current_dir, &n_of_dir_items, &current_dir_address);

    block_id field_to_remove = find_in_folder_by_name(file, dir_items, n_of_dir_items, bitmap_size, fp);
    if (field_to_remove == NULL_BLOCK) {
        fclose(fp);
        perror("This field does not exist!");
        exit(1);
    } else if (get_block_type(field_to_remove) == BTYPE_DIR_HEADER) {
        fseek(fp, bitmap_size + BLOCK_SIZE * get_block_address(field_to_remove), SEEK_SET);
        fread(&removed_dir, sizeof removed_dir, 1, fp);
        if (removed_dir.actual_size > 0) {
            fclose(fp);
            perror("You can only remove an empty folder");
            exit(1);
        }
    }

    /* invalidate the field in the bitmap */
    invalidate_field(field_to_remove, hole_bitmap, n_of_blocks, fp, bitmap_size);

    /* update folder contents */
    int shift_to_left = 0;
    block_id current_dir_item;
    for (int i = 0; i < n_of_dir_items; ++i) {
        current_dir_item = *(dir_items + i);
        if (shift_to_left == 0 && current_dir_item == field_to_remove) {
            shift_to_left = 1;
        } else if (shift_to_left == 1) {
            *(dir_items + i - 1) = *(dir_items + i);
        }
    }
    if (shift_to_left == 1) {
        current_dir.actual_size -= sizeof current_dir_item;
        n_of_dir_items--;
    }

    /* update bitmap and folder */
    fseek(fp, BITMAP_HEADER, SEEK_SET);
    fwrite(hole_bitmap, 1, bitmap_size - BITMAP_HEADER, fp);
    fseek(fp, bitmap_size + BLOCK_SIZE * current_dir_address, SEEK_SET);
    fwrite(&current_dir, sizeof(field_header_t), 1, fp);
    fwrite(dir_items, sizeof(block_addr), n_of_dir_items, fp);
    fclose(fp);
}

void print_disc_info(char *disc_path) {
    char buffer[BLOCK_SIZE];
    FILE *fp = NULL;

    char *hole_bitmap;
    blocks n_of_blocks; /* total number of blocks */
    size_t bitmap_size; /* total size of the bitmap segment */

    fp = fopen(disc_path, "r+");
    if (fp == NULL) {
        perror("Couldnt open virtual disc!");
        exit(1);
    }
    /* get file bitmap */
    load_hole_bitmap(fp, &hole_bitmap, &n_of_blocks, &bitmap_size);

    printf("Block states:\nD - directory header, F - file header, x - data block, . - empty block\n");

    block_id current_block_id;
    block_type curr_block_type;
    int total_occupied = 0;
    int n_of_files = 0;
    int n_of_folders = 0;
    for (int i = 0; i < n_of_blocks; ++i) {
        if ((i & 0xF) == 0 && i > 0) printf("\n");
        fread(&current_block_id, sizeof current_block_id, 1, fp);
        fseek(fp, DATA_BLOCK_CAP, SEEK_CUR);
        curr_block_type = get_block_type(current_block_id) * get_bit_from_bitmap(hole_bitmap, i);
        total_occupied += (curr_block_type != BTYPE_NULL);

        switch (curr_block_type) {
            case BTYPE_DIR_HEADER:
                n_of_folders++;
                printf("D");
                break;
            case BTYPE_FILE_HEADER:
                n_of_files++;
                printf("F");
                break;
            case BTYPE_DATA:
                printf("x");
                break;
            case BTYPE_NULL:
                printf(".");
                break;
            default:
                printf("?");
                break;
        }
    }
    printf("\nBlocks occupied: %d/%d (%0.1fkB/%0.1fkB %0.2f%%)\n", total_occupied, n_of_blocks,
           (float) (total_occupied * BLOCK_SIZE) / 1024.0f,
           (float) (n_of_blocks * BLOCK_SIZE) / 1024.0f, (float) total_occupied / (float) n_of_blocks * 100.0f);
    printf("%d files in %d folders in total\n", n_of_files, n_of_folders);
}

/* --- Back-end --- */

void get_current_directory(const char *folder, FILE *fp, size_t bitmap_size, block_id *dir_items,
                           field_header_t *current_dir, size_t *n_of_dir_items, block_addr *current_dir_address) {
    fread(current_dir, sizeof(field_header_t), 1, fp);
    (*n_of_dir_items) = (*current_dir).actual_size / sizeof(block_addr);
    fread(dir_items, sizeof(block_id), (*n_of_dir_items), fp);

    block_id current_dir_id;
    if (folder != NULL) {
        current_dir_id = find_in_folder_by_name(folder, dir_items, (*n_of_dir_items), bitmap_size, fp);
        if (get_block_type(current_dir_id) != BTYPE_DIR_HEADER) {
            fclose(fp);
            perror("This is not a folder!");
            exit(1);
        }
        (*current_dir_address) = get_block_address(current_dir_id);
        if ((*current_dir_address) == NULL_BLOCK) {
            fclose(fp);
            perror("This folder does not exist!");
            exit(1);
        }
        fseek(fp, bitmap_size + (*current_dir_address) * BLOCK_SIZE, SEEK_SET);
        fread(current_dir, sizeof(*current_dir), 1, fp);
        (*n_of_dir_items) = (*current_dir).actual_size / sizeof(block_addr);
        fread(dir_items, sizeof(block_id), (*n_of_dir_items), fp);
    }
}

void load_hole_bitmap(FILE *fp, char **hole_bitmap, blocks *n_of_blocks, size_t *bitmap_size) {
    fread(bitmap_size, sizeof(*bitmap_size), 1, fp);
    fread(n_of_blocks, sizeof(*n_of_blocks), 1, fp);

    (*hole_bitmap) = malloc((*bitmap_size) - BITMAP_HEADER);
    fread((*hole_bitmap), 1, (*bitmap_size) - BITMAP_HEADER, fp);
}

block_id
find_in_folder_by_name(const char *name, const block_id *dir_items, size_t n_of_items, size_t bitmap_size, FILE *fp) {
    block_id temp_id;
    block_addr temp_addr;
    block_type temp_type;

    field_header_t temp_header;
    for (int i = 0; i < n_of_items; i++) {
        temp_id = *(dir_items + i);
        temp_addr = get_block_address(temp_id);
        temp_type = get_block_type(temp_id);

        fseek(fp, BLOCK_SIZE * temp_addr + bitmap_size, SEEK_SET);
        fread(&temp_header, sizeof(field_header_t), 1, fp);

        if (strcmp(name, temp_header.name) == 0) {
            return temp_id;
        }
    }
    return NULL_BLOCK;
}

size_t min(size_t a, size_t b) {
    if (a > b) return b;
    else return a;
}

void parse_path(char *path, char **file, char **folder) {/* parse path */
    strsep(&path, "/"); /* root */
    *folder = strsep(&path, "/"); /* folder or file */
    *file = strsep(&path, "/"); /* null or file */

    if (*file == NULL) {
        *file = *folder;
        *folder = NULL;
    }
}

field_header_t create_field_header(char *name, size_t actual_size, block_addr next_block_id) {
    field_header_t result = {next_block_id, actual_size, bytes_to_blocks(actual_size)};
    validate_name(name);
    strcpy(result.name, name);
    return result;
}

block_id write_file_into_system(char *path_source, char *name, char *hole_bitmap, blocks n_of_blocks, FILE *vdisk_fp,
                                size_t bitmap_size) {
    /* get source file */
    FILE *src_fp = fopen(path_source, "r");
    if (src_fp == NULL) {
        perror("Couldnt open source file!");
        exit(1);
    }

    fseek(src_fp, 0, SEEK_END);
    size_t actual_size = ftell(src_fp);
    size_t remaining_bytes_of_source = actual_size;
    size_t bytes_read_from_file;
    rewind(src_fp);

    char buffer[BLOCK_SIZE] = {};

    block_addr current_block = 1;
    block_id current_block_id;
    blocks blocks_so_far = 0;
    block_addr previous_block = NULL_BLOCK;
    block_addr header_address = NULL_BLOCK;

    int finished = 0;

    while (current_block < n_of_blocks) {
        if (get_bit_from_bitmap(hole_bitmap, current_block) == 0) {
            /*free block*/
            if (blocks_so_far == 1) {
                /* header block */
                bytes_read_from_file = fread(buffer, 1, FIELD_BLOCK_CAP, src_fp);
                remaining_bytes_of_source -= bytes_read_from_file;
                header_address = previous_block;

                if (remaining_bytes_of_source > 0) {
                    field_header_t new_file_header = create_field_header(name, actual_size,
                                                                         get_block_id(BTYPE_FILE_HEADER,
                                                                                      current_block));

                    /* go to the block, write header, read from source, insert from buffer */
                    fseek(vdisk_fp, bitmap_size + BLOCK_SIZE * header_address, SEEK_SET);
                    fwrite(&new_file_header, sizeof new_file_header, 1, vdisk_fp);
                    fwrite(buffer, 1, FIELD_BLOCK_CAP, vdisk_fp);
                } else {
                    /* all data has been insterted, break after you finish */
                    field_header_t new_file_header = create_field_header(name, actual_size,
                                                                         get_block_id(BTYPE_FILE_HEADER, NULL_BLOCK));

                    /* go to the block, write header, read from source, insert from buffer */
                    fseek(vdisk_fp, bitmap_size + BLOCK_SIZE * header_address, SEEK_SET);
                    fwrite(&new_file_header, sizeof new_file_header, 1, vdisk_fp);
                    fwrite(buffer, 1, FIELD_BLOCK_CAP, vdisk_fp);
                    finished = 1;
                    break;
                }
            } else if (blocks_so_far > 1) {
                /*data block*/
                bytes_read_from_file = fread(buffer, 1, DATA_BLOCK_CAP, src_fp);
                remaining_bytes_of_source -= bytes_read_from_file;

                /* go to the block, write simple header, read from source, insert from buffer */
                fseek(vdisk_fp, bitmap_size + BLOCK_SIZE * previous_block, SEEK_SET);
                if (remaining_bytes_of_source > 0) {
                    current_block_id = get_block_id(BTYPE_DATA, current_block);
                    fwrite(&current_block_id, sizeof current_block_id, 1, vdisk_fp);
                    fwrite(buffer, 1, DATA_BLOCK_CAP, vdisk_fp);
                } else {
                    /* all data has been insterted, break after you finish */
                    current_block_id = get_block_id(BTYPE_DATA, NULL_BLOCK);
                    fwrite(&current_block_id, sizeof current_block, 1, vdisk_fp);
                    fwrite(buffer, 1, DATA_BLOCK_CAP, vdisk_fp);
                    finished = 1;
                    break;
                }
            }
            set_bit_on_bitmap(hole_bitmap, current_block);
            blocks_so_far++;
            previous_block = current_block;
        }
        current_block++;
    }
    fclose(src_fp);
    if (finished == 1) {
        return get_block_id(BTYPE_FILE_HEADER, header_address);
    } else {
        fclose(vdisk_fp);
        perror("Unable to allocate memory!");
        exit(1);
    }
}

block_id
invalidate_field(block_id field_id, char *hole_bitmap, blocks n_of_blocks, FILE *vdisk_fp, size_t bitmap_size) {
    block_addr current_addr = get_block_address(field_id);

    while (current_addr != NULL_BLOCK) {
        unset_bit_on_bitmap(hole_bitmap, current_addr);
        fseek(vdisk_fp, current_addr * BLOCK_SIZE + bitmap_size, SEEK_SET);
        fread(&current_addr, sizeof current_addr, 1, vdisk_fp);
        current_addr = get_block_address(current_addr);
    }
}

block_addr get_block_address(block_addr block) {
    return (block_addr) (block << (32 - N_BADDR_BITS) >> (32 - N_BADDR_BITS));
}

block_type get_block_type(block_id block) { return (block_type) (block) >> N_BADDR_BITS; }

block_addr get_block_id(block_type type, block_addr address) {
    return (block_addr) ((type << N_BADDR_BITS) + address);
}

block_addr bytes_to_blocks(size_t bytes) {
    block_addr n_blocks = 1;
    if (bytes > FIELD_BLOCK_CAP) {
        n_blocks += (bytes - FIELD_BLOCK_CAP) / DATA_BLOCK_CAP;
        if ((bytes - FIELD_BLOCK_CAP) % DATA_BLOCK_CAP != 0) {
            /* additional block for remaining data */
            n_blocks += 1;
        }
    }
    return n_blocks;
}

void validate_name(char *name) {
    if (strlen(name) >= MAX_NAME_LENGTH) {
        perror("Name too long!");
        exit(1);
    }
}

int get_bit_from_bitmap(const char *hole_bitmap, block_addr pos) {
    return (hole_bitmap[pos / 8] >> (pos & 7)) & 1;
}

void set_bit_on_bitmap(char *hole_bitmap, block_addr pos) {
    hole_bitmap[pos / 8] = hole_bitmap[pos / 8] | 1 << (pos & 7);
}

void unset_bit_on_bitmap(char *hole_bitmap, block_addr pos) {
    hole_bitmap[pos / 8] = hole_bitmap[pos / 8] & (((1 << 8) - 1) ^ (1 << (pos & 7)));
}

