
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void linear_scaling(size_t img_size, unsigned char* data,int channels) {
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

        *p     = (unsigned char) fmin(255, fmax(0, ((*p     - *ptr_darkest_pixl) * scale)));
        *(p+1) = (unsigned char) fmin(255, fmax(0, ((*(p+1) - *ptr_darkest_pixl) * scale)));
        *(p+2) = (unsigned char) fmin(255, fmax(0, ((*(p+2) - *ptr_darkest_pixl) * scale)));

    }

}

void brighter_colors(unsigned char *data, size_t img_size, int channels) {

    for (unsigned char *p = data; p < data + img_size; p += channels) {
        //organge check
        if (*p >= 171 && *(p+2) <= 85) {
            //orange possible

            //red-part
           if (*p <=127) {
               *p = 0;
           }else {
               *p = 255;
           }
            //green-part
            if (*(p+1)<=85) {
                *(p+1) = 0;
            }
            else if (*(p+1) >= 86 && *(p+1) <= 170) {
                *(p+1) = 127;
            }
            else {
                *(p+1) = 255;
            }
            //blue-part
            if (*(p+2)<=127) {
                *(p+2) = 0;
            }
            else {
                *(p+2) = 255;
            }
        }
        else {
            //orange not possible

            //red-part
            if (*p <=127) {
                *p = 0;
            }else {
                *p = 255;
            }
            //green-part
            if (*(p+1)<=127) {
                *(p+1) = 0;
            }
            else {
                *(p+1) = 255;
            }
            //blue-part
            if (*(p+2)<=127) {
                *(p+2) = 0;
            }
            else {
                *(p+2) = 255;
            }

        }

    }
}

int main(void) {
    printf("Hello, World!\n");
    int width, height, channels;
    unsigned char* data = stbi_load("/Users/felixrehm/CLionProjects/Bildanalyse/Cube.jpeg", &width, &height, &channels, 0);
    if(data == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);

    // Convert the image to gray
    size_t img_size = width * height * channels;


    //erhöhe den Kontrast
    brighter_colors(data,img_size,channels);
    stbi_write_jpg("/Users/felixrehm/CLionProjects/Bildanalyse/high_contrast.jpg", width,height,channels,data,100);


    return 0;
}