/* $Id$ */

#include <math.h>

#include "webstr.h"
#include "buffer.h"

char *itoa(int p)
{
	Buffer *s;
	char *r;
	s = Buffer_init(1+(int)ceil(log10((double)p)));
	while (p) {
		Buffer_addch(s, '0' + (p % 10));
		p /= 10;
	}
	r = xstrdup(Buffer_get(s));
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
