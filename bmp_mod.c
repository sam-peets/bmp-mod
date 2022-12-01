#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

#include "bmp.h"
#include "filters.h"
#include "emalloc.h"



int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s <infile.bmp> <outfile.bmp>\n", argv[0]);
		exit(1);
	}
	
	FILE* fr = fopen(argv[1], "rb");
	if (fr == NULL) {
		fprintf(stderr, "file not found %s\n", argv[1]);
		exit(1);
	}

	FILE* fw = fopen(argv[2], "wb");
	if (fw == NULL) {
		fprintf(stderr, "cant open %s for writing\n", argv[2]);
		exit(1);
	}

	srand(time(0));

	header* h = emalloc(sizeof(header));
	fread(h, sizeof(header), 1, fr);

	BITMAPINFOHEADER* bmih = emalloc(sizeof(BITMAPINFOHEADER));
	fread(bmih, sizeof(BITMAPINFOHEADER), 1, fr);

	char* d = emalloc(h->size);
	memcpy(d, h, sizeof(header));

	memcpy(d + sizeof(header), bmih, sizeof(BITMAPINFOHEADER));

	size_t pixel_offset = sizeof(header)+sizeof(BITMAPINFOHEADER);
	fread(d + h->offset, h->size,1,fr);
	
	char* edit = emalloc(h->size);
	memcpy(edit,d,h->size);

	edit = scale_nn(edit, 101,101);
	edit = scale_bilinear(edit, 202,202);

	brightness(edit,10);
	grayscale(edit);
	posterize(edit,2);
	flip_horiz(edit);

	fwrite(edit, h->size, 1, fw);
	free(h);
	free(bmih);
	free(d);
	fclose(fw);
	fclose(fr);
	return 0;
}
