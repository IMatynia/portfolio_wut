#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "virtual_disc.h"

int main(int ac, char *av[]) {
    char *operation = av[1];
    if (ac >= 4 && strcmp(operation, "create") == 0) {
        printf("Creating disc %s\n", av[2]);
        create_disc(av[2], atoi(av[3]));
    } else if (ac >= 5 && strcmp(operation, "insert") == 0) {
        printf("Inserting %s into %s\n", av[3], av[4]);
        insert_into_disc(av[2], av[3], av[4]);
    } else if (ac >= 4 && strcmp(operation, "mkdir") == 0) {
        printf("Creating directory %s\n", av[3]);
        make_directory(av[2], av[3]);
    } else if (ac >= 5 && strcmp(operation, "extract") == 0) {
        printf("Extracting file %s\n", av[3]);
        extract_from_disc(av[2], av[3], av[4]);
    } else if (ac >= 4 && strcmp(operation, "ls") == 0) {
        list_dir(av[2], av[3]);
    } else if (ac >= 4 && strcmp(operation, "rm") == 0) {
        printf("Removing %s\n", av[3]);
        remove_file_or_folder(av[2], av[3]);
    } else if (ac >= 3 && strcmp(operation, "disc_remove") == 0) {
        remove_disc(av[2]);
        printf("Removed disc %s\n", av[2]);
    } else if (ac >= 3 && strcmp(operation, "disc_info") == 0) {
        print_disc_info(av[2]);
    } else {
        printf("USAGE: [operation]    [path_to_disc] (operation args...)\n");
        printf("        create        [path_to_disc] [size in bytes] -> creates a new virtual disc\n");
        printf("        insert        [path_to_disc] [source path (real)] [destination path (virtual)] -> inserts a file into the virtual drive\n");
        printf("        mkdir         [path_to_disc] [folder name] -> creates the folder in root (root/<name>)\n");
        printf("        extract       [path_to_disc] [source path (virtual)] [destination path (real)] -> extracts a file from the disc\n");
        printf("        ls            [path_to_disc] [folder name] -> lists given directory\n");
        printf("        disc_remove   [path_to_disc] -> deletes disc\n");
        printf("        disc_info     [path_to_disc] -> prints disc blocks info\n");
    }
    printf("\n");
    return 0;
}
