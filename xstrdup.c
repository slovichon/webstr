#include "webstr.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

char *xstrdup(const char *s)
{
	char *p;
	assert((p = strdup(s)) != NULL);
	return p;
}
