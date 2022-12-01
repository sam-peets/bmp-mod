#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "emalloc.h"

void* emalloc(size_t size) {
	void* p = malloc(size);
	if (p == NULL) {
		fprintf(stderr, "couldnt allocate %lx bytes\n", size);
		exit(1);
	}
	return p;
}

