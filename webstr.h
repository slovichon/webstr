#ifndef _WEBSTR_H
#define _WEBSTR_H

#include <stddef.h>

#ifndef HAS_BOOL_T
#define HAS_BOOL_T
typedef enum {
	FALSE = 0,
	TRUE  = 1,
} bool_t;
#endif

#define STR_NONE  (0)
#define STR_HTML  (1<<0)
#define STR_URL   (1<<1)
#define STR_EMAIL (1<<2)
#define STR_ALL (STR_HTML | STR_URL | STR_EMAIL)

struct webstr_ent_map {
	char ent;	/* entity */
	char *esc;	/* escape sequence */
};

struct webstr_allowed_html {
	char *tag;
	char **attrs;
};

struct webstr_prefs {
	size_t max_chars;
	bool_t auto_urls;
	bool_t auto_emails;
	char **auto_tlds;
	char **allowed_protos;
	char **attr_check_proto;
	struct webstr_allowed_html *allowed_html;
};


#ifndef HAS_ITOA
#define HAS_ITOA
char *itoa(int p);
#endif

#ifndef HAS_STRREV
#define HAS_STRREV
void strrev(char *);
#endif

char *webstr_encode_html(char *s);
// char *webstr__str_attr_clean(char *s);
// char *webstr__remove_css(char *s, char *selectors[]);
// bool_t webstr__check_proto(char *s, char *protos[]);
char *webstr_parse(char *s, int flags, struct webstr_prefs *);

#ifndef HAS_XALLOC
char *xstrdup(const char *s);
void *xmalloc(size_t len);
#endif

void bark(char *msg, ...);

#endif /* _WEBSTR_H */
