#ifndef BMP_H
#define BMP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>


typedef struct __attribute__((__packed__)) BITMAPINFOHEADER {
	uint32_t size; // size of header
	int32_t width;
	int32_t height;
	uint16_t planes; // must be 1
	uint16_t bpp; // bits per pixel or colour depth
	uint32_t compression;
	uint32_t raw_size; // size of raw pixel data, sometimes will be zero for rgb no compression
	int32_t h_res; // pixel per metre
	int32_t v_res; // pixel per metre
	uint32_t palette;
	uint32_t important; // number of important colours
} BITMAPINFOHEADER;

typedef struct __attribute__((__packed__)) header {
	uint16_t type;
	uint32_t size; // full image size
	uint32_t padding; // reserved
	uint32_t offset; // pixel data offset
} header;

typedef struct __attribute__((__packed__)) rgb {
	uint8_t b;
	uint8_t g;
	uint8_t r;
} rgb;


#endif
