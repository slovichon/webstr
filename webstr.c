#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "webstr.h"
#include "buffer.h"

static bool in_vec(char *needle, char **hay)
{
	char **p;
bark("checking for vec presence");
	for (p = hay; *p; p++) {
bark("comparing %s\n to %s", needle, *p);
		if (strcmp(needle, *p) == 0) {
bark("found!");
			return TRUE;
		}
	}
	return FALSE;
}

static bool in_a_href(char *needle, char *hay)
{
	bool found = FALSE;
	return found;
}

static bool in_a(char *needle, char *hay)
{
	bool found = FALSE;
	return found;
}

char *webstr_encode_html(char *s)
{
	Buffer *p;
	char *q, *seq;
	int len, i, j;
	bool found;

	struct webstr_ent_map map[] = {
		{ '<', "&lt;" },
		{ '>', "&gt;" },
		{ '&', "&amp;" },
		{ '"', "&quot;" },
		{ '\0', NULL }
	};
	len = strlen(s);
	/* encoding all invalid characters may expand it some */
	p = Buffer_init((len*5)/4);
	for (i = 0; i < len; i++) {
		found = FALSE;
		for (j = 0; map[j].ent; j++)
			if (map[j].ent == s[i]) {
				/* use entity mapping */
				Buffer_cat(p, map[j].esc);
				found = TRUE;
				break;
			}

		if (!found) {
			if (strchr("abcdefghijklmnopqrstuvwxyz"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"0123456789"
				"!@#$^*()`~-_=+{}[] \t\r\n\\|;:',./?", s[i]) != NULL)
				/* valid char, pass it through */
				Buffer_addch(p, s[i]);
			else {
				/* else add &#keycode; */
				Buffer_addch(p, '&');
				Buffer_addch(p, '#');
				seq = itoa(s[i]);
				Buffer_cat(p, seq);
				free(seq);
				Buffer_addch(p, ';');
			}
		}
	}
	q = Buffer_get(p);
	Buffer_long_free(&p, TRUE);
	return q;
}

/*
	remove_html_attr
	strip_html_attr
*/

char *webstr__attr_clean(char *s)
{
	Buffer *q;
	char *p;
	int len = strlen(s);
	q = Buffer_init(len);
	/* decode_html */
	/* remove css */
	p = xstrdup(Buffer_get(q));
	Buffer_free(&q);
	return p;
}

/*
	in a CSS string:
		
		foo:bar; foo1:bar2; foo3:bar4;
	
	and arguments

		"foo", "foo3"
	
	return

		foo1:bar2;
	
	things to watch out for:
		case-sensitivity
		whitespace
		optional trailing `;'
*/
char *webstr__remove_css(char *s, char *selectors[])
{
	Buffer *q;
	char *p;
	int len = strlen(s);
	q = Buffer_init(len);
	p = xstrdup(Buffer_get(q));
	Buffer_free(&q);
	return p;
}

char *webstr_parse(char *s, int flags, struct webstr_prefs *prefs)
{
	Buffer *p;
	char *r;

	r = webstr_encode_html(s);

	if (flags & STR_HTML) {
bark("performing HTML parsing");
	}

	if ((flags & STR_URL) && prefs->auto_urls) {
		char *q = r;
		char *t;
		Buffer *proto, *domain, *url;
		char *pos;
		p = Buffer_init(strlen(r));
bark("performing auto URL parsing");
		for (; *q; q++) {
bark("Examining %c", *q);
			if (q != s && isalnum(q[-1])) {
bark("Skipping");
				Buffer_addch(p, *q);
				continue;
			}
			proto = Buffer_init(5);		/* protocol */
			domain = Buffer_init(15);	/* domain name */
			url = Buffer_init(30);		/* location */
			/* protocol */
			for (t = q; *t && isalpha(*t); t++) {
bark("pushing %c", *t);
				Buffer_addch(proto, *t);
			}
			if (*t == ':') {
				t++;
				Buffer_addch(proto, ':');
			} else {
bark("proto being freed (%s)", Buffer_get(proto));
				/* reset protocol */
				Buffer_free(&proto);
			}
bark("proto: %s", Buffer_is_set(proto) ? Buffer_get(proto) : "(null)");
			if (Buffer_is_set(proto) &&
				((strcmp(Buffer_get(proto), "http:") == 0) ||
				(strcmp(Buffer_get(proto), "https:") == 0))) {
bark("looking for http://");
				/* http and https require `//' (might be /// or /) */
				if ((*t == '/') && (t[1] == '/')) {
					t += 2;
					Buffer_cat(proto, "//");
				} else if ((*t == '/') && (t[1] == '/') &&
					(t[2] == '/')) {
					t += 3;
					Buffer_cat(proto, "//");
				} else if (*t == '/') {
					t++;
					Buffer_cat(proto, "//");
				} else {
bark("// not found");
					Buffer_free(&proto);
				}
			}
			/* restart if protocol wasn't found */
			if (!Buffer_is_set(proto))
				t = q;
			/* www. */
			if ((*t == 'w') && (t[1] == 'w') &&
				(t[2] == 'w') && (t[3] == '.')) {
				t += 4;
bark("found www.");
				Buffer_set(domain, "www.");
			}
			/* rest of domain */
			if (isalnum(*t) || (*t == '-')) {
				while (isalnum(*t) || (*t == '-') ||
					(*t == '.'))
						Buffer_addch(domain, *t++);
bark("domain: %s", Buffer_get(domain));
			}
			if (Buffer_is_set(domain) &&
				((pos = strrchr(Buffer_get(domain), '.')) != NULL) &&
				/* skip '.' and check TLD */
				pos[1] && in_vec(pos+1, prefs->auto_tlds)) {
bark("domain looks valid, checking TLD completer");
				/* valid TLD completer */
				if (*t == '/') {
					/* location */
					Buffer_addch(url, '/');
					while (*++t && !isspace(*t) ||
						/*
						 * we should instead match any
						 * amount of punctuation followed
						 * by whitespace
						 */
						(ispunct(*t) && t[1] && !isspace(t[1]))) {
						Buffer_addch(url, *t);
					}
				}
			} else {
				Buffer_free(&domain);
			}
bark("url?");
			if (domain && !in_a_href(s, q) && !in_a(s, q) &&
				/* if the protocol was found, it must be allowed */
				((Buffer_is_set(proto) &&
				in_vec(Buffer_get(proto), prefs->allowed_protos)) ||
				!Buffer_is_set(proto))) {
bark("found url [proto: %s] [domain: %s] [loc: %s]",
	Buffer_is_set(proto) ? Buffer_get(proto) : "DEFAULT",
	Buffer_get(domain), Buffer_is_set(url) ? Buffer_get(url) : "NONE");
				/* we found a URL, now reconstruct it */
				Buffer_cat(p, "<a href=\"");
				if (Buffer_is_set(proto))
					Buffer_append(p, proto);
				else
					Buffer_cat(p, "http://");
				
				Buffer_append(p, domain);
				Buffer_append(p, url);
				
				Buffer_cat(p, "\">");
				Buffer_cat_range(p, q, t);
				Buffer_cat(p, "</a>");

				q = t;
			}
			Buffer_addch(p, *q);
		}
		free(r);
		r = Buffer_get(p);
		Buffer_long_free(&p, TRUE);
	}

	/* auto e-mail */

	/* fix newlines */
{
	int len, i;
	len = strlen(r);
	p = Buffer_init(len);
	for (i = 0; i < len; i++)
		if (r[i] == '\n') {
			Buffer_cat(p, "<br />");
		} else if ((r[i] == '\r') && (i+1 < len)
				&& (r[i+1] == '\n')) {
			Buffer_cat(p, "<br />");
			i++;
		/* try this last */
		} else if (r[i] == '\r')
			Buffer_cat(p, "<br />");
		else
			Buffer_addch(p, r[i]);
bark("buf: %s", Buffer_get(p));
	free(r);
	r = Buffer_get(p);
	Buffer_long_free(&p, TRUE);
}

bark("newlines fixed, doing run-ons now");

	/* fix run-ons */
{
	int count = 0, len, i, lastpos = 0;
//	bool found;
	len = strlen(r);
	p = Buffer_init(len);
	for (i = 0; i < len; i++)
	{
		/* a problem is (?:&#nbsp;)* */
		if (strchr("&<()> \t/!?|.,", r[i]) != NULL)
			count = 0;
		else
			count++;

		if (count > prefs->max_chars) {
#if 0
			/* find previous character acceptable to break at */
			found = FALSE;
			for (j = i; j > lastpos; j++)
				if (strchr("<(", r[j]) != NULL) {
					/* insert before char */
					Buffer_addch(p, ' ');
					Buffer_addch(p, r[i]);
					found = TRUE;
					break;
				} else if (strchr(">)", r[j]) != NULL) {
					/* insert after char */
					Buffer_addch(p, r[i]);
					Buffer_addch(p, ' ');
					found = TRUE;
					break;
				}

			if (!found) {
				/* must not have found a char, force break */
#endif
				Buffer_addch(p, ' ');
				Buffer_addch(p, r[i]);
//			}
			/* next char counts */
			count = 1;
			/* future's last replacement is here */
			lastpos = i;
		} else {
			Buffer_addch(p, r[i]);
		}
	}

	free(r);
	r = Buffer_get(p);
	Buffer_long_free(&p, TRUE);
}

	return r;
}
