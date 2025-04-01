
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
    int i = 0;
    for (unsigned char* p = data; p < data + img_size; p+= channels) {
        u_int8_t brightness = (u_int8_t)(0.299 * (float)*p + 0.587 * (float)*(p + 1) + 0.114 * (float)*(p +2));
        if (brightness > highest_brightness) {
            highest_brightness = brightness;
            ptr_brightest_pxl  = p;
            printf("Hellster Pixel aktuell: %d\n", brightness);
        }
        if (brightness < smallest_brightness) {
            smallest_brightness = brightness;
            ptr_darkest_pixl = p;
        }
        if ( i%10 == 0) {
            printf("Loopcount = %d\n", i);
        }
        i++;
    }
    printf("Erste Schleife wurde verlassen \n");
    if (ptr_brightest_pxl == NULL || ptr_darkest_pixl == NULL ) {
        printf("Es wurde weder ein hellster Pixel noch ein dunkelster Pixel gefunden\n");
        exit(1);
    }
    else {
        printf("Es wurde ein hellster Pixel unter %p mit: %d und ein dunkelster unter %p mit: %d gefunden\n", ptr_brightest_pxl,*ptr_brightest_pxl, ptr_darkest_pixl,*ptr_darkest_pixl);
    }
    for (unsigned char* p = data; p <  data + img_size; p += channels) {
        float scale = 255.0f /(*ptr_brightest_pxl - *ptr_darkest_pixl);
        if (*ptr_darkest_pixl == *ptr_brightest_pxl) {
            scale = 1.0f;
        }
        //*p =(*p - *ptr_darkest_pixl) / (*ptr_brightest_pxl - *ptr_darkest_pixl) * 255;
        //*(p+1) =(*(p+1) - *ptr_darkest_pixl) / (*ptr_brightest_pxl - *ptr_darkest_pixl) * 255;
        //*(p+2) =(*(p+2) - *ptr_darkest_pixl) / (*ptr_brightest_pxl - *ptr_darkest_pixl) * 255;

        //next try
        /*
        *p = (unsigned char)(*p - *ptr_darkest_pixl) / scale;
        *(p+1) = (unsigned char)(*(p+1) - *ptr_darkest_pixl) / scale;
        *(p + 2) = (unsigned char)(*(p+2) - *ptr_darkest_pixl) / scale;
        */
        *p     = (unsigned char) fmin(255, fmax(0, ((*p     - *ptr_darkest_pixl) * scale)));
        *(p+1) = (unsigned char) fmin(255, fmax(0, ((*(p+1) - *ptr_darkest_pixl) * scale)));
        *(p+2) = (unsigned char) fmin(255, fmax(0, ((*(p+2) - *ptr_darkest_pixl) * scale)));

    }



}

int main(void) {
    printf("Hello, World!\n");
    int width, height, channels;
    unsigned char* data = stbi_load("/Users/felixrehm/CLionProjects/Bildanalyse/test.jpg", &width, &height, &channels, 0);
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
    stbi_write_jpg("/Users/felixrehm/CLionProjects/Bildanalyse/high_contrast.jpg", width,height,channels,data,100);

    free(high_contrast);
    high_contrast = NULL;
    //free(gray_img);
    //gray_img = NULL;
    return 0;
}