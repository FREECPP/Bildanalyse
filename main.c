
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(void) {
    printf("Hello, World!\n");
    int width, height, channels;
    unsigned char* data = stbi_load("/Users/felixrehm/CLionProjects/Bildanalyse/Rubiks.png", &width, &height, &channels, 0);
    if(data == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);

    // Convert the image to gray
    size_t img_size = width * height * channels;
    int gray_channels = channels == 4 ? 2 : 1;
    size_t img_gray_size = width * height * gray_channels;

    unsigned char *gray_img = malloc(img_gray_size);
    if (gray_img == NULL) {
        printf("Unable to allocate memory for the gray image\n");
        exit(1);
    }

    for (unsigned char *p = data, *pg = gray_img;p != data + img_size; p += channels, pg += gray_channels) {
        *pg = (u_int8_t)((*p + *(p + 1) + *(p + 2))/ 3.0);
        if (channels ==4) {
            *(pg + 1) = *(p + 3);
        }
    }
    stbi_write_jpg("/Users/felixrehm/CLionProjects/Bildanalyse/sky_gray.jpg", width, height, gray_channels, gray_img, 100);


    free(gray_img);
    return 0;
}