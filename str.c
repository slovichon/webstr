/* $Id$ */

#include "webstr.h"
#include "buffer.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

char *itoa(int p)
{
	Buffer *s;
	char *r;
	s = Buffer_init(1+(int)ceil(log10((double)p)));
	while (p) {
		Buffer_addch(s, '0' + (p % 10));
		p /= 10;
	}
	assert((r = strdup(Buffer_get(s))) != NULL);
	strrev(r);
	return r;
}

void strrev(char *s)
{
	int i, j, len;
	char t;
	len = strlen(s)/2;
	for (i = 0, j = strlen(s)-1; i < len; i++, j--) {
		t = s[i];
		s[i] = s[j];
		s[j] = t;
	}	
}
