/*
	general-memory.c

	Memory allocation

*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "inc/bob.h"

#define CHUNK   (32750)         // just under 32K

void* vmalloc(int size) {

/*

	static char             *ptr = NULL;
	void                    *tmp;
	static unsigned int     amount = 0;

	if (size > CHUNK) {         // need a big one
		tmp = malloc(size);
		if (tmp) {
			MemAllocated += size;
		}
		return tmp;             // may be NULL if not available
	} 
	
	else if (size <= (int)amount) {
		tmp = ptr;
		ptr += size;
		amount -= size;
		MemAllocated += size;
		return tmp;
	} 
	
	else {
		ptr = malloc(CHUNK);
		if (!ptr) {      // nothing big left so fill in the holes
			tmp = malloc(size);
			if (tmp) {
				MemAllocated += size;
			}
			return tmp;
		} else {
			amount = CHUNK;
		}
		return vmalloc(size);
	}
*/
    MemAllocated += size;
    void* p = malloc(size);
    memset(p, 0, size);
    return p;
}


