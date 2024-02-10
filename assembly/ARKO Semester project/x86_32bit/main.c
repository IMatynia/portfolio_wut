#include <stdio.h>
#include "image.h"
#include <string.h>

extern void mark_greatest_brigthness(ImageInfo *info, int width, int height);

int main(int ac, char *av[])
{
    // av[1] => input file
    // av[2] => width of the rectangle
    // av[3] => height of the rectangle

    if (ac < 3)
    {
        printf("Missing arguments!");
        return 0;
    }

    int width = atoi(av[2]), height = atoi(av[3]);
    ImageInfo *image = readBmp(av[1]);
    if (width > image->width || height > image->height || width < 1 || height < 1)
    {
        printf("Invalid rectangle size!\n");
        return 0;
    }

    printf("Processing begins...\n");
    mark_greatest_brigthness(image, width, height);
    printf("Processing done, saving file\n");

    char new_file[200];
    strcat(new_file, av[1]);
    strcat(new_file, "_result_");
    strcat(new_file, av[2]);
    strcat(new_file, "x");
    strcat(new_file, av[3]);
    strcat(new_file, ".bmp");

    printf("%s\n", new_file);
    saveBmp(new_file, image);
    return 0;
}