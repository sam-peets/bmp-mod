#ifndef FILTERS_H
#define FILTERS_H

#include "bmp.h"


void grayscale(char* d);
void random_dither(char* d, char* ref);
void posterize(char* d, int levels);
char* scale_nn(char* d, int32_t nw, int32_t nh);
char* scale_bilinear(char* d, int32_t nw, int32_t nh);
void brightness(char* d, int amount);
void flip_horiz(char* d);
void flip_vert(char* d);
void invert(char* d);

#endif
