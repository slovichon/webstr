/* $Id$ */

#include "webstr.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *xmalloc(size_t len)
{
	void *t;
	assert((t = malloc(len)) != NULL);
	return t;
}

char *xstrdup(const char *s)
{
	char *p;
	assert((p = strdup(s)) != NULL);
	return p;
}
