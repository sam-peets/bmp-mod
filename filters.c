#include "filters.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

#define PIXEL_OFFSET_D (rgb*)&d[h->offset + y*bmih->width*(bmih->bpp/8) + x*(bmih->bpp/8)];
#define PIXEL_OFFSET_R (rgb*)&ref[h->offset + y*bmih->width*(bmih->bpp/8) + x*(bmih->bpp/8)];

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

uint8_t avg_col(rgb* p) {
	return (uint8_t)(((float)p->b + (float)p->g + (float)p->r)/3.0);
}

void grayscale(char* d) {
	header* h = (header*)(d);
	BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)(d+sizeof(header));
	for (int y = 0; y < bmih->height; y++) {
		for (int x = 0; x < bmih->width; x++) {
			rgb* p = PIXEL_OFFSET_D;
			uint8_t c = avg_col(p);

			p->r = c;
			p->g = c;
			p->b = c;
		}
	}

}

void random_dither(char* d, char* ref) {
	header* h = (header*)(d);
	BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)(d+sizeof(header));
	for (int y = 0; y < bmih->height; y++) {
		for (int x = 0; x < bmih->width; x++) {
			rgb* p = PIXEL_OFFSET_D;
			rgb* refp = PIXEL_OFFSET_R;
			
			uint8_t c = avg_col(refp);
			if (rand()%256 < c) {
				c = 255;
			} else {
				c = 0;
			}
			p->r = c;
			p->g = c;
			p->b = c;
		}
	}
}


uint8_t posterize_bound(uint8_t c, int levels) {
	levels--;
	float sep = 255.0/(float)levels;
	return (uint8_t)(round((float)c/sep)*sep);
}
void posterize(char* d, int levels) {
	header* h = (header*)(d);
	BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)(d+sizeof(header));
	for (int y = 0; y < bmih->height; y++) {
		for (int x = 0; x < bmih->width; x++) {
			rgb* p = PIXEL_OFFSET_D;	

			p->r = posterize_bound(p->r, levels);
			p->g = posterize_bound(p->g, levels);
			p->b = posterize_bound(p->b, levels);
		}
	}
}

char* resize(char* d, int32_t nw, int32_t nh) {
	header* h = (header*)(d);
	BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)(d+sizeof(header));

	size_t pixel_data = nw*nh*bmih->bpp/3;
	
	d = realloc(d, h->offset + pixel_data);

	if (d == NULL) {
		fprintf(stderr, "resize couldnt realloc\n");
		exit(1);
	}
	return d;
}

char truncate(int c) {
	if (c > 255) {
		return 255;
	} else if (c < 0) {
		return 0;
	}
	return c;
}

void brightness(char* d, int amount) {
	header* h = (header*)(d);
        BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)(d+sizeof(header));
        for (int y = 0; y < bmih->height; y++) {
                for (int x = 0; x < bmih->width; x++) {
                        rgb* p = PIXEL_OFFSET_D;

                        p->r = truncate(p->r + amount);
                        p->g = truncate(p->g + amount);
                        p->b = truncate(p->b + amount);
                }
        }
}

void flip_vert(char* d) {
	header* h = (header*)(d);
        BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)(d+sizeof(header));

	char* temp = malloc(h->size);
        for (int y = 0; y < bmih->height; y++) {
                for (int x = 0; x < bmih->width; x++) {
                        rgb* p = PIXEL_OFFSET_D;
			rgb* target = (rgb*)&temp[h->offset + (bmih->height-y-1)*bmih->width*(bmih->bpp/8) + x*(bmih->bpp/8)];

                        target->r = p->r;
                        target->g = p->g;
                        target->b = p->b;
                }
        }
	memcpy(d+h->offset,temp+h->offset,h->size-h->offset);
	free(temp);
}

void flip_horiz(char* d) {
 	header* h = (header*)(d);
        BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)(d+sizeof(header));

        char* temp = malloc(h->size);
        for (int y = 0; y < bmih->height; y++) {
                for (int x = 0; x < bmih->width; x++) {
                        rgb* p = PIXEL_OFFSET_D;
                        rgb* target = (rgb*)&temp[h->offset + y*bmih->width*(bmih->bpp/8) + (bmih->width-x-1)*(bmih->bpp/8)];

                        target->r = p->r;
                        target->g = p->g;
                        target->b = p->b;
                }
        }
        memcpy(d+h->offset,temp+h->offset,h->size-h->offset);
        free(temp);	
}

void invert(char* d) {
	header* h = (header*)(d);
        BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)(d+sizeof(header));
        for (int y = 0; y < bmih->height; y++) {
                for (int x = 0; x < bmih->width; x++) {
                        rgb* p = PIXEL_OFFSET_D;

                        p->r = 255-p->r;
                        p->g = 255-p->g;
                        p->b = 255-p->b;
                }
       } 	
}

char* scale_nn(char* d, int32_t nwidth, int32_t nheight) {
	header* h = (header*)(d);
	BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)(d+sizeof(header));

	
	char* nd = malloc(h->offset + nwidth*nheight*bmih->bpp/3);
	memcpy(nd, d, sizeof(header)+sizeof(BITMAPINFOHEADER));

	header* nh = (header*)(nd);
	BITMAPINFOHEADER* nbmih = (BITMAPINFOHEADER*)(nd+sizeof(header));

	nh->size = h->offset + nwidth*nheight*bmih->bpp/3;
	nbmih->width = nwidth;
	nbmih->height = nheight;
	nbmih->raw_size = nwidth*nheight*bmih->bpp/3;


	for (int y = 0; y < nbmih->height; y++) {
		for (int x = 0; x < nbmih->width; x++) {
			
			rgb* p = (rgb*)&nd[nh->offset + y*nbmih->width*(nbmih->bpp/8) + x*(nbmih->bpp/8)];

			float sx = (float)x/(float)nwidth;
			float sy = (float)y/(float)nheight;

			int32_t old_x = (int32_t)round(sx*(float)bmih->width);
			int32_t old_y = (int32_t)round(sy*(float)bmih->height);

			rgb* old_p = (rgb*)&d[h->offset + old_y*bmih->width*(bmih->bpp/8) + old_x*(bmih->bpp/8)];

			p->r = old_p->r;
			p->g = old_p->g;
			p->b = old_p->b;
		}
	}

	free(d);
	return nd;
}

char* scale_bilinear(char* d, int32_t nwidth, int32_t nheight) {
	header* h = (header*)(d);
        BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)(d+sizeof(header));


        char* nd = malloc(h->offset + nwidth*nheight*bmih->bpp/3);
        memcpy(nd, d, sizeof(header)+sizeof(BITMAPINFOHEADER));

        header* nh = (header*)(nd);
        BITMAPINFOHEADER* nbmih = (BITMAPINFOHEADER*)(nd+sizeof(header));

        nh->size = h->offset + nwidth*nheight*bmih->bpp/3;
        nbmih->width = nwidth;
        nbmih->height = nheight;
        nbmih->raw_size = nwidth*nheight*bmih->bpp/3;	

	for (int y = 0; y < nbmih->height; y++) {
                for (int x = 0; x < nbmih->width; x++) {

                        rgb* p = (rgb*)&nd[nh->offset + y*nbmih->width*(nbmih->bpp/8) + x*(nbmih->bpp/8)];

                        float sx = (float)x/(float)nwidth;
                        float sy = (float)y/(float)nheight;
			
			float ox = sx*(float)bmih->width;
			float oy = sy*(float)bmih->height;

                        float old_x1 = (float)floor(ox);
                        float old_y1 = (float)floor(oy);
			float old_x2 = (float)floor(ox+1);
                        float old_y2 = (float)floor(oy+1);

			rgb* p_11 = (rgb*)&d[h->offset + (int)old_y1*bmih->width*3 + (int)old_x1*3];
			rgb* p_21 = (rgb*)&d[h->offset + (int)old_y1*bmih->width*3 + (int)old_x2*3];
			rgb* p_12 = (rgb*)&d[h->offset + (int)old_y2*bmih->width*3 + (int)old_x1*3];
			rgb* p_22 = (rgb*)&d[h->offset + (int)old_y2*bmih->width*3 + (int)old_x2*3];

			float dx1 = ((old_x2-ox)/(old_x2-old_x1));
			float dx2 = ((ox-old_x1)/(old_x2-old_x1));


			float dy1 = ((old_y2-oy)/(old_y2-old_y1));
			float dy2 = ((oy-old_y1)/(old_y2-old_y1));

			uint8_t new_r = (uint8_t)(dy1*(dx1*(float)p_11->r + dx2*(float)p_21->r) + dy2*(dx1*(float)p_12->r + dx2*(float)p_22->r));
			uint8_t new_g = (uint8_t)(dy1*(dx1*(float)p_11->g + dx2*(float)p_21->g) + dy2*(dx1*(float)p_12->g + dx2*(float)p_22->g));
			uint8_t new_b = (uint8_t)(dy1*(dx1*(float)p_11->b + dx2*(float)p_21->b) + dy2*(dx1*(float)p_12->b + dx2*(float)p_22->b));

                        p->r = new_r;
                        p->g = new_g;
                        p->b = new_b;

                }
        }

        free(d);
        return nd;

}
