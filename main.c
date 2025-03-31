
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void linear_scaling( unsigned char* high_contrast,size_t img_size, unsigned char* data,int channels) {
    printf("Hallo ich bin die Funktion und werde ausgeführt\n");

    u_int8_t smallest_brightness = 255, highest_brightness = 0;
    unsigned char *ptr_brightest_pxl = NULL, *ptr_darkest_pixl = NULL;

    for (unsigned char* p = data; *p != *data + img_size; p+= channels) {
        u_int8_t brightness = (u_int8_t)0.299 * *p + 0.587 * *(p + 1) + 0.114 * *(p +2);
        if (brightness > highest_brightness) {
            highest_brightness = brightness;
            ptr_brightest_pxl  = p;
        }
        if (brightness < smallest_brightness) {
            smallest_brightness = brightness;
            ptr_darkest_pixl = p;
        }
    }
    if (ptr_brightest_pxl == NULL || ptr_darkest_pixl == NULL ) {
        printf("Es wurde weder ein hellster Pixel noch ein dunkelster Pixel gefunden\n");
        exit(1);
    }
    else {
        printf("Es wurde ein hellster Pixel unter %p und ein dunkelster unter %p gefunden\n", ptr_brightest_pxl, ptr_darkest_pixl);
    }
    for (unsigned char* p = high_contrast; *p !=  *high_contrast + img_size; p += channels) {
        *p =(*p - *ptr_darkest_pixl) / (*ptr_brightest_pxl - *ptr_darkest_pixl) * 255;
        *(p+1) =(*(p+1) - *ptr_darkest_pixl) / (*ptr_brightest_pxl - *ptr_darkest_pixl) * 255;
        *(p+2) =(*(p+2) - *ptr_darkest_pixl) / (*ptr_brightest_pxl - *ptr_darkest_pixl) * 255;
    }



}

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
/*
    unsigned char *gray_img = malloc(img_gray_size);
    if (gray_img == NULL) {
        printf("Unable to allocate memory for the gray image\n");
        exit(1);
    }
    */
    unsigned char* high_contrast = malloc(img_size);
    if (high_contrast == NULL) {
        printf("Es konnte kein Speicher für high_contrast allokiert werden\n");
        exit(1);
    }
    else {
        printf("Es wurde Speicher für high_contrast unter %p allokiert\n", high_contrast);
    }
    //erhöhe den Kontrast
    linear_scaling(high_contrast,img_size,data,channels);
    stbi_write_jpg("/Users/felixrehm/CLionProjects/Bildanalyse/high_contrast.jpg", width,height,channels,high_contrast,100);
/*
    //mache das Bild schwarz weiß
    for (unsigned char *p = data, *pg = gray_img;p != data + img_size; p += channels, pg += gray_channels) {
        *pg = (u_int8_t)((*p + *(p + 1) + *(p + 2))/ 3.0);
        if (channels ==4) {
            *(pg + 1) = *(p + 3);
        }
    }
    stbi_write_jpg("/Users/felixrehm/CLionProjects/Bildanalyse/sky_gray.jpg", width, height, gray_channels, gray_img, 100);
*/
    free(high_contrast);
    high_contrast = NULL;
    //free(gray_img);
    //gray_img = NULL;
    return 0;
}