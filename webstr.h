#ifndef _WEBSTR_H
#define _WEBSTR_H

#include <stddef.h>

#if !defined(HAS_BOOL)
#define HAS_BOOL
typedef enum {FALSE=0, TRUE=1} bool;
#endif

/*
#ifdef TRUE
# if !TRUE
#  undef TRUE
#  define TRUE 1
# endif
#else
# define TRUE 1
#endif

#ifdef FALSE
# if FALSE
#  undef FALSE
#  define FALSE 0
# endif
#else
# define FALSE 0
#endif
*/

#define STR_NONE 0
#define STR_HTML 1
#define STR_URL  2
#define STR_ALL (STR_HTML | STR_URL)

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
	bool auto_urls;
	char **auto_tlds;
	char **allowed_protos;
	struct webstr_allowed_html *allowed_html;
};


#if !defined(HAS_ITOA)
char *itoa(int p);
#endif
#if !defined(HAS_STRREV)
void strrev(char *);
#endif
char *webstr_encode_html(char *s);
//char *webstr__str_attr_clean(char *s);
//char *webstr__remove_css(char *s, char *selectors[]);
//bool webstr__check_proto(char *s, char *protos[]);
char *webstr_parse(char *s, int flags, struct webstr_prefs *);
#if !defined(HAS_XSTRDUP)
char *xstrdup(const char *s);
#endif
#if !defined(HAS_XMALLOC)
void *xmalloc(size_t len);
#endif

void bark(char *msg, ...);

#endif /* _WEBSTR_H */
