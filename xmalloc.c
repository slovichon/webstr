#include "webstr.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void *xmalloc(size_t len)
{
	void *t;
	assert((t = malloc(len)) != NULL);
	return t;
}
