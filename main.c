
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
    u_int8_t high = 255;
    u_int8_t low = 0;

    for (unsigned char *p = data; p < data + img_size; p += channels) {
        //organge check
        if (*p >= 171 && *(p+2) <= 85) {
            //orange possible

            //red-part
           if (*p <=127) {
               *p = low;
           }else {
               *p = high;
           }
            //green-part
            if (*(p+1)<=85) {
                *(p+1) = low;
            }
            else if (*(p+1) >= 86 && *(p+1) <= 170) {
                *(p+1) = 127;
            }
            else {
                *(p+1) = high;
            }
            //blue-part
            if (*(p+2)<=127) {
                *(p+2) = low;
            }
            else {
                *(p+2) = high;
            }
        }
        else {
            //orange not possible

            //red-part
            if (*p <=127) {
                *p = low;
            }else {
                *p = high;
            }
            //green-part
            if (*(p+1)<=127) {
                *(p+1) = low;
            }
            else {
                *(p+1) = high;
            }
            //blue-part
            if (*(p+2)<=127) {
                *(p+2) = low;
            }
            else {
                *(p+2) = high;
            }

        }

    }
}
void make_mid_visible(int width, int height, unsigned char *data, size_t image_size, int channels) {
    u_int8_t high = 255;
    u_int8_t low = 0;
    //vertical line
    int half_width_pos = width * channels / 2;
    for (unsigned char *p = data + half_width_pos; p < data + image_size; p += width*channels) {
        //vertical line in magenta
        *p =  high;
        *(p + 1) = low;
        *(p + 2) = high;
    }
    //horizontal line
    int half_height_pos =width * height * channels /2;
    for (unsigned char *p = data + half_height_pos; p < data + half_height_pos + width * channels; p += channels) {
        //vertical line in magenta
        *p =  high;
        *(p + 1) = low;
        *(p + 2) = high;
    }
}

unsigned char* find_upper_left_rubic(unsigned char *data, size_t img_size, int channels, int width, int pxl_tolerance,
    int black_pxl, int non_black_pxl) {
    u_int8_t high = 255;
    u_int8_t low = 0;
    unsigned char *lower_bound = data;
    unsigned char *upper_bound = data + img_size;
    unsigned char *ptr_upper_left = NULL;
    //iterate over whole picture
    for (unsigned char *p = data; p < data + img_size; p+=channels) {
        //check if actual pixel is black
        if (*p == low && *(p+1) == low && *(p +2) == low) {
            //look over the next n pixels(horizontal) | Check if they are also black
            while (1) {
                unsigned char *pxl_addr_horizontal = NULL;
                unsigned char *pxl_addr_vertical = NULL;
                unsigned char *pxl_color_upper_left_candidate_1 = NULL;
                unsigned char *pxl_color_upper_left_candidate_2 = NULL;
                int flag_break = 0;
                for (unsigned char *q = p+1; q < p + black_pxl; q+=(3*channels)) {
                    if (*q != low || *(q+1) != low || *(q +2) != low) {
                        flag_break = 1;
                        break;
                    }
                    //Save nth pixel address
                    pxl_addr_horizontal = q;
                }
                if (flag_break == 1) {
                    break;
                }
                //look over the next n pixels(vertical) | Check if they are also black

                for (unsigned char *q = p; q < p + (black_pxl * channels * width);q += (width * channels) ) {
                    if (*q != low || *(q+1) != low || *(q +2) != low) {
                        flag_break = 1;
                        break;
                    }
                    //Save nth pixel address
                    pxl_addr_vertical = q;
                }
                if (flag_break == 1) {
                    break;
                }
                //starting from last vertical pixel -> after a tolerance look for the next n pixels (horizontal)
                //if they are not black
                for (unsigned char *q = (pxl_addr_vertical + pxl_tolerance); q <(pxl_addr_vertical + pxl_tolerance + non_black_pxl); q += (3*channels)) {
                    if (*q == low && *(q+1) == low && *(q +2) == low) {
                        flag_break = 1;
                        break;
                    }
                    pxl_color_upper_left_candidate_1 = q;
                }


                if (flag_break == 1) {
                    break;
                }

                //starting from last horizontal pixel (above) -> after a tolerance, look for the next n pixels (vertical)
                //if they are not black
                for (unsigned char *q = (pxl_addr_horizontal + (pxl_tolerance *width*channels)); q < (pxl_addr_horizontal + (pxl_tolerance * width * channels) + (non_black_pxl * width * channels)); q+=(width * channels)) {
                    if (*q == low && *(q+1) == low && *(q +2) == low) {
                        flag_break = 1;
                        break;
                    }
                    pxl_color_upper_left_candidate_2 = q;
                }
                if (flag_break == 1) {
                    break;
                }
                // if we are at this point of the function we can be most certain that we have the address of the upper left
                // corner pixel
                unsigned char *left_upper_corner_color = NULL;
                if (*pxl_color_upper_left_candidate_1 == *pxl_color_upper_left_candidate_2 && *(pxl_color_upper_left_candidate_1 +1) == *(pxl_color_upper_left_candidate_2 + 1) && *(pxl_color_upper_left_candidate_1 +2) == *(pxl_color_upper_left_candidate_2 + 2)) {
                    left_upper_corner_color = pxl_color_upper_left_candidate_1;
                }

                *p = high;
                *(p + 1) = low;
                *(p + 2) = high;
                ptr_upper_left =p;
                break;
            }
        }
    }
    return ptr_upper_left;
}

void find_lower_right_rubic(unsigned char *data, size_t img_size, int channels, int width, int pxl_tolerance,
    int black_pxl, int non_black_pxl) {
    u_int8_t low = 0;
    u_int8_t high = 255;
    unsigned char *lower_bound = data;
    unsigned char *upper_bound = data + img_size;
    unsigned char *ptr_upper_left = NULL;
    //iterate over whole picture but backwards
    for (unsigned char *p = data + img_size; p > data; p-=channels) {
        //check if actual pixel is black
        if (*p == low && *(p+1) == low && *(p +2) == low) {
            while (1) {
                int flag_break = 0;
                unsigned char *pxl_addr_horizontal = NULL;
                unsigned char *pxl_addr_vertical = NULL;
                unsigned char *pxl_color_upper_left_candidate_1 = NULL;
                unsigned char *pxl_color_upper_left_candidate_2 = NULL;
                //look over the next n pixels(horizontal) | Check if they are also black

                //Check for an negative memory overflow
                if ((p-channels) < lower_bound || (p - black_pxl) < lower_bound) {
                    break;
                }
                for (unsigned char *q = p-channels; q < p - black_pxl; q-=(3*channels)) {
                    if (*q != low || *(q+1) != low || *(q +2) != low) {
                        flag_break = 1;
                        break;
                    }
                    //Save nth pixel address
                    pxl_addr_horizontal = q;
                }
                if (flag_break == 1) {
                    break;
                }
                //look over the next n pixels(vertical) | Check if they are also black

                //Check for an negative memory overflow
                //Here is code missing
                for (unsigned char *q = p; q > p - (black_pxl * channels * width);q -= (width * channels) ) {
                    if (*q != low || *(q+1) != low || *(q +2) != low) {
                        flag_break = 1;
                        break;
                    }
                    //Save nth pixel address
                    pxl_addr_vertical = q;
                }
                if (flag_break == 1) {
                    break;
                }
                //starting from last vertical pixel -> after a tolerance look for the next n pixels (horizontal)
                //if they are not black
                for (unsigned char *q = (pxl_addr_vertical - pxl_tolerance); q >(pxl_addr_vertical - pxl_tolerance - non_black_pxl); q -= (3*channels)) {
                    if (*q == low && *(q+1) == low && *(q +2) == low) {
                        flag_break = 1;
                        break;
                    }
                    pxl_color_upper_left_candidate_1 = q;
                }
                if (flag_break == 1) {
                    break;
                }
                printf("found plx_color1\n");

                //starting from last horizontal pixel (above) -> after a tolerance, look for the next n pixels (vertical)
                //if they are not black
                if (pxl_addr_horizontal < lower_bound) {
                    printf("Fehler bei pxl_addr_horizontal\n");
                    break;
                }
                if (pxl_addr_horizontal - (pxl_tolerance *width*channels) < lower_bound) {
                    continue;
                }
                for (unsigned char *q = (pxl_addr_horizontal - (pxl_tolerance *width*channels));q >= lower_bound && q > (pxl_addr_horizontal - (pxl_tolerance * width * channels) - (non_black_pxl * width * channels)); q-=(width * channels)) {
                    if (*q == low && *(q+1) == low && *(q +2) == low) {
                        flag_break = 1;
                        break;
                    }
                    printf("inside for\n");
                    pxl_color_upper_left_candidate_2 = q;
                }
                if (flag_break == 1) {
                    break;
                }
                // if we are at this point of the function we can be most certain that we have the address of the upper left
                // corner pixel
                printf("Found possible corner at %p\n",p);
                unsigned char *left_upper_corner_color = NULL;
                if (*pxl_color_upper_left_candidate_1 == *pxl_color_upper_left_candidate_2 && *(pxl_color_upper_left_candidate_1 +1) == *(pxl_color_upper_left_candidate_2 + 1) && *(pxl_color_upper_left_candidate_1 +2) == *(pxl_color_upper_left_candidate_2 + 2)) {
                    left_upper_corner_color = pxl_color_upper_left_candidate_1;
                }

                *p = high;
                *(p + 1) = low;
                *(p + 2) = high;
                ptr_upper_left =p;
                break;
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
    make_mid_visible(width,height,data, img_size, channels);
    unsigned char *ptr_upper_left = find_upper_left_rubic(data,img_size,channels,width,10,100,20);
    find_lower_right_rubic(data,img_size,channels,width,10,100,20);
    stbi_write_jpg("/Users/felixrehm/CLionProjects/Bildanalyse/high_contrast.jpg", width,height,channels,data,100);


    return 0;
}