#include "webstr.h"
#include <string.h>

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
