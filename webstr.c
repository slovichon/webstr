#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "webstr.h"
#include "buffer.h"

static bool in_vec(char *needle, char **hay, int len, bool sens)
{
	char **p;
	if (sens) {
		if (len) {
			for (p = hay; *p; p++)
				if (strncmp(needle, *p, len) == 0)
					return TRUE;
		} else {
			for (p = hay; *p; p++)
				if (strcmp(needle, *p) == 0)
					return TRUE;
		}
	} else {
		if (len) {
			for (p = hay; *p; p++)
				if (strncasecmp(needle, *p, len) == 0)
					return TRUE;
		} else {
			for (p = hay; *p; p++)
				if (strcasecmp(needle, *p) == 0)
					return TRUE;
		}
	}
	return FALSE;
}

#define __DECR(a) \
	{ \
		if ((a) == s) { \
			/* reached string boundary, couldn't have found it */ \
			bark("boundary reached"); \
			return FALSE; \
		} else \
			(a)--; \
	}

/*
	<a href="sdf">asdf</a>
*/
/*
 * Search for ``<a ...>URL''. `url' here points to somewhere
 * inside `s' and marks the beginning of the URL.
 *
 * I think this also inherently suppresses link spoofing:
 *
 *	<a href="url1">url2</a>
 */
static bool in_a_tag(char *url, char *s)
{
	char *q = url;

	/* locate last tag */
	do {
		__DECR(q);
	} while (*q != '>');

	if (q == s)
		return FALSE;

	/* find beginning of tag */
	while (q != s)
		if (*--q == '<') {
			if (((q[1] == 'a') || (q[1] == 'A')) &&
			    isspace(q[2]))
			    	/* it's a link */
				return TRUE;
			else
				/* end of tag, and the tag isn't an A */
				return FALSE;
		}
	return FALSE;
}

/*
 * Search for ``<a ... href="URL''. `url' here points to
 * somewhere in `s' and marks the beginning of the target
 * URL.
 *
 * Our str_parse() should have well-formed the HTML so we
 * shouldn't need to do any strange parsing (such as
 * optional quotes or whitespace, etc.).
 */
static bool in_a_href(char *url, char *s)
{
	char *q = url;
	__DECR(q);
	if (*q != '"')
		return FALSE;
	__DECR(q);
	if (*q != '=')
		return FALSE;
	/* href */
	__DECR(q);
	__DECR(q);
	__DECR(q);
	__DECR(q);
	if (strncasecmp(q, "href", 4) != 0)
		return FALSE;
	while (q != s)
		if (*--q == '<') {
			if (((q[1] == 'a') || (q[1] == 'A')) &&
			    isspace(q[2]))
				return TRUE;
			else
				return FALSE;
		}
	return FALSE;
}

static struct webstr_ent_map map[] = {
	{ '<', "lt" },
	{ '>', "gt" },
	{ '&', "amp" },
	{ '"', "quot" },
//	{ '\'', "apos"},
	{ '\0', NULL }
};

char *webstr_decode_html(char *s)
{
	Buffer *p;
	char *q, *t, *u;
	int ch, len;
	struct webstr_ent_map *iter;

	p = Buffer_init(strlen(s));

	for (t = s; *t != '\0'; t++) {
		if (*t == '&') {
			if (isalpha(t[1])) {
				/* look in the map */
				for (iter = map; iter->ent != '\0'; iter++) {
					len = strlen(iter->esc);
					if ((strncmp(t, iter->esc, len) == 0) &&
					    (t[len] == ';')) {
						/* found a match */
						Buffer_addch(p, iter->ent);
						t += len;
						continue;
					}
				}
				/* not found, skip */
			} else if ((t[1] == '#') && isdigit(t[2])) {
				ch = 0;
				for (u = t+2; isdigit(*u); u++)
					ch = ch*10 + (*u - '0');
				if (*u == ';') {
					/* should we check ch here for a valid range? */
					Buffer_addch(p, ch);
					t = ++u;
					continue;
				}
			} else {
				/* invalid ('\0'?) */

			}
		}
		Buffer_addch(p, *t);
	}

	q = Buffer_get(p);
	Buffer_long_free(&p, TRUE);

	return q;
}

char *webstr_encode_html(char *s)
{
	Buffer *p;
	char *q, *seq;
	int len, i, j;
	bool found;

	/* encoding all invalid characters may expand it some */
	len = strlen(s);
	p = Buffer_init((len*5)/4);
	for (i = 0; i < len; i++) {
		found = FALSE;
		for (j = 0; map[j].ent; j++)
			if (map[j].ent == s[i]) {
				/* use entity mapping */
				Buffer_addch(p, '&');
				Buffer_cat(p, map[j].esc);
				Buffer_addch(p, ';');
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

char *webstr__attr_clean(char *s)
{
	Buffer *q;
	char *p;
	int len = strlen(s);
	q = Buffer_init(len);
// decode_html
// remove css
	p = xstrdup(Buffer_get(q));
	Buffer_free(&q);
	return p;
}
*/

char *webstr_css_remove_expr(char *s)
{
	Buffer *p, *name, *val;
	char *q, *r;

	p = Buffer_init(strlen(s));

	for (q = s; *q != '\0'; q++) {
		if (isalpha(*q) || (*q == '-')) {
			r = q;
			name = Buffer_init(5);
			do {
				Buffer_addch(name, *r++);
			} while (isalnum(*r) || (*r == '-'));
			while (isspace(*r))
				r++;
			if (*r != ':')
				goto CLEANUP;
			while (isspace(*++r));
//bark("mid [name: %s] [tail: %s]", Buffer_get(name), r);
			val = Buffer_init(8);
			switch (*r) {
				case '"':
					while (isspace(*++r));
					while ((*r != '\0') && (*r != '"'))
						Buffer_addch(val, *r);
					while (isspace(*r))
						r++;
					if (*r != ';') {
						/* invalid */
						q = r;
						goto END;
					}
					r++;
					break;

				case '\'':
					while (isspace(*++r));
					while ((*r != '\0') && (*r != '\''))
						Buffer_addch(val, *r);
					while (isspace(*r))
						r++;
					if (*r != ';') {
						/* invalid */
						q = r;
						goto END;
					}
					r++;
					break;

				default:
					while ((*r != '\0') && (*r != ';'))
						Buffer_addch(val, *r++);
					break;
			}
			if (strncasecmp(Buffer_get(val), "expression", 10) != 0) {
				Buffer_append(p, name);
				Buffer_addch(p, ':');
				Buffer_append(p, val);
				Buffer_addch(p, ';');
			}
			q = r;
			goto END;
		}
CLEANUP:
		Buffer_addch(p, *q);
END:
		if (Buffer_is_set(name))
			Buffer_free(&name);
		if (Buffer_is_set(val))
			Buffer_free(&val);
	}

	q = Buffer_get(p);
	Buffer_long_free(&p, TRUE);
	return q;
}

char *webstr_parse(char *s, int flags, struct webstr_prefs *prefs)
{
	Buffer *p;		/* temp buf for each incremental parse */
	char *r, *t, *q;	/* temp strings r => new string,
					t => incremental parse completer,
					q => fail-back for incremental parse */
	int count;		/* counting chars in max run-on chars */
	int ignore;		/* whether to ignore chars in newline fixing */

	r = webstr_encode_html(s);

	if (flags & STR_HTML) {
		Buffer *tag, *val, *attrname, *attrval, *endtagbuf;
		VBuffer *attrnames, *attrvals;
		char *w, *x, *endtag, **allowedattr;
		int endtaglen;
		bool allowed;
		struct webstr_allowed_html *iter;

bark("performing HTML parsing on %s", r);
		p = Buffer_init(strlen(r));
		for (q = r; *q != '\0'; q++) {
			if ((*q == '&') && (strncmp(q, "&lt;", 4) == 0)) {
				/* tagname */
				tag = Buffer_init(3);
				for (t = q+4; isalpha(*t); t++)
					Buffer_addch(tag, tolower(*t));
//bark("found <%s>", Buffer_get(tag), q);
				/* attributes */
				attrnames = VBuffer_init();
				attrvals  = VBuffer_init();
				while ((*t != '\0') && (strncmp(t, "&gt;", 4) != 0)) {
//bark("'%c'", *t);
					/* attribute */
					if (isalpha(*t) || (*t == '-')) {
						attrname = Buffer_init(6);
						do {
							Buffer_addch(attrname, tolower(*t++));
						} while (isalpha(*t) || (*t == '-'));
						while (isspace(*t))
							t++;
						if ((*t != '=') || (*t == '\0'))
							goto H_CLEANUP;
						while (isspace(*++t));
						attrval = Buffer_init(15);
						if (*t == '\'') {
//bark("squot");
							/* single quote delimiters */
							t++;
							while ((*t != '\0') && (*t != '\''))
								Buffer_addch(attrval, *t++);
							if (*t != '\'')
								goto H_CLEANUP;
							t++;
						} else if (strncmp(t, "&quot;", 6) == 0) {
//bark("dquot");
							/* double quote delimiters (encode) */
							t += 6;
//bark("%s", t);
							while ((*t != '\0') &&
								(strncmp(t, "&quot;", 6) != 0))
								Buffer_addch(attrval, *t++);
							if (strncmp(t, "&quot;", 6) != 0)
								goto H_CLEANUP;
							t += 6;
//bark("%s", t);
						} else if (*t == '\0')
							goto H_CLEANUP;
						else {
//bark("ndelim");
							/* null attr val delimiters */
							while ((*t != '\0') &&
							       (strchr(" \t\r\n\f", *t) == NULL) &&
							       (strncmp(t, "&gt;", 4) != 0))
								Buffer_addch(attrval, *t++);
						}
bark("%s='%s'", Buffer_get(attrname), Buffer_get(attrval));
						VBuffer_add(attrnames, attrname);
						VBuffer_add(attrvals, attrval);
//						Buffer_free(&attrval);
//						Buffer_free(&attrname);
					} else
						t++;
				}
//bark("ended HTML tag");
				t += 4; /* &gt; */
				val = Buffer_init(20);

				/* &lt; / tag &gt; */
				endtaglen = 4*2 + strlen(Buffer_get(tag)) + 1;
				endtagbuf = Buffer_init(endtaglen);
				Buffer_set(endtagbuf, "&lt;/");
				Buffer_append(endtagbuf, tag);
				Buffer_cat(endtagbuf, "&gt;");
				endtag = Buffer_get(endtagbuf);
				Buffer_long_free(&endtagbuf, TRUE);

				while ((*t != '\0') && (strncmp(t, endtag, endtaglen) != 0))
					Buffer_addch(val, *t++);

				free(endtag);

				if (*t == '\0')
					goto H_CLEANUP;

//bark("val: %s", Buffer_get(val));
				/* check for allowed html tag */
				allowed = FALSE;
				for (iter = prefs->allowed_html; iter->tag != NULL; iter++) {
					if (strcmp(iter->tag, Buffer_get(tag)) == 0) {
						/* this tag is allowed*/
						allowed = TRUE;
						Buffer_addch(p, '<');
						Buffer_append(p, tag);

						/* check/add attributes */
						while (((attrname = VBuffer_remove(&attrnames)) != NULL) &&
							((attrval = VBuffer_remove(&attrvals)) != NULL)) {
							for (allowedattr = iter->attrs; *allowedattr != NULL; allowedattr++) {
								if (strcmp(Buffer_get(attrname), *allowedattr) == 0) {
									/*
									 * if attribute is targetted for protocol checking
									 * and doesn't pass, move to next attribute
									 */
									if (in_vec(Buffer_get(attrname), prefs->attr_check_proto, 0, FALSE) &&
									    ((x = strchr(Buffer_get(attrval), ':')) != NULL) &&
									    !in_vec(Buffer_get(attrval), prefs->allowed_protos,
										   x - Buffer_get(attrval), FALSE))
										goto NEXTATTR;
									Buffer_addch(p, ' ');
									Buffer_cat(p, *allowedattr);
									Buffer_cat(p, "=\"");
									x = webstr_decode_html(Buffer_get(attrval));
									if (strcasecmp(*allowedattr, "style") == 0) {
										w = webstr_css_remove_expr(x);
										free(x);
										x = w;
									}
									Buffer_cat(p, x);
									free(x);
									Buffer_addch(p, '"');

									goto NEXTATTR;
								}
							}

NEXTATTR:
							Buffer_free(&attrname);
							Buffer_free(&attrval);
						}

						Buffer_addch(p, '>');
						Buffer_append(p, val);
						Buffer_cat(p, "</");
						Buffer_append(p, tag);
						Buffer_addch(p, '>');
						break;
					}
				}

				/* tag isn't allowed; preserve value though */
				if (!allowed)
					Buffer_append(p, val);

				q = t + endtaglen - 1;
				goto H_END;

H_CLEANUP:
				Buffer_addch(p, *q);
H_END:

				if (Buffer_is_set(tag))
					Buffer_free(&tag);
				if (Buffer_is_set(attrname))
					Buffer_free(&attrname);
				if (Buffer_is_set(attrval))
					Buffer_free(&attrval);
			} else
				Buffer_addch(p, *q);
		}
		free(r);
		r = Buffer_get(p);
		Buffer_long_free(&p, TRUE);
	}

	if ((flags & STR_URL) && prefs->auto_urls) {
		char *pos;
		bool sawtag = FALSE;
		Buffer *proto, *domain, *url = NULL;
		p = Buffer_init(strlen(r));
bark("performing auto URL parsing");
		for (q = r; *q; q++) {
//bark("Examining %c", *q);
			if (*q == '<')
				sawtag = TRUE;
			if (((q != s) && isalnum(q[-1])) || sawtag) {
//bark("Skipping");
				if (*q == '>')
					sawtag = FALSE;
				Buffer_addch(p, *q);
				continue;
			}
			proto 	= Buffer_init(5);	/* protocol */
			domain 	= Buffer_init(15);	/* domain name */
//			url 	= Buffer_init(30);	/* location */
			/* protocol */
			for (t = q; *t && isalpha(*t); t++) {
//bark("pushing %c", *t);
				Buffer_addch(proto, *t);
			}
			if (*t == ':') {
				t++;
				Buffer_addch(proto, ':');
			} else {
//bark("proto being freed (%s)", Buffer_get(proto));
				/* reset protocol */
				Buffer_free(&proto);
				t = q;
			}
//bark("proto: %s", Buffer_is_set(proto) ? Buffer_get(proto) : "(null)");
			if (Buffer_is_set(proto)) {//&&
				//((strcmp(Buffer_get(proto), "http:") == 0) ||
				//(strcmp(Buffer_get(proto), "https:") == 0))) {
//bark("looking for //");
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
				} else if ((strcmp(Buffer_get(proto), "http:")  == 0) ||
					   (strcmp(Buffer_get(proto), "https:") == 0)) {
				//} else {
//bark("// not found");
					Buffer_free(&proto);
				}
			}
			/* restart if protocol wasn't found */
//			if (!Buffer_is_set(proto))
//				t = q;
			/* www. */
			if (strncmp(t, "www.", 4) == 0) {
				t += 4;
//bark("found www.");
				Buffer_set(domain, "www.");
			}
			/* rest of domain */
			if (isalnum(*t) || (*t == '-')) {
				while (isalnum(*t) || (*t == '-') ||
				       (*t == '.'))
					Buffer_addch(domain, *t++);
//bark("domain: %s", Buffer_get(domain));
//bark("domain looks valid, checking TLD completer");
				/* valid TLD completer */
				if (Buffer_is_set(domain) &&
				    ((pos = strrchr(Buffer_get(domain), '.')) != NULL) &&
				    /* skip '.' and check TLD */
				    pos[1] != '\0') {
					/*
					 * must match auto TLD if set to
					 * http:// or not set
					 * (inherent http link)
					 */
					if (((Buffer_is_set(proto) &&
					    ((strcmp(Buffer_get(proto), "http://") == 0) ||
					     (strcmp(Buffer_get(proto), "https://") == 0))) ||
					     !Buffer_is_set(proto)) &&
					     in_vec(pos+1, prefs->auto_tlds, 0, TRUE)) {
						if (*t == '/') {
							/* location */
							url = Buffer_init(10); /* default to average URL length */
							Buffer_addch(url, '/');
							while ((*++t != '\0') && !isspace(*t) &&
								/*
								 * we should instead match any
								 * amount of punctuation followed
								 * by whitespace
								 */
								((ispunct(*t) && (t[1] != '\0') && !isspace(t[1])) ||
								!ispunct(*t))) {
								Buffer_addch(url, *t);
							}
						}
					} else {
//bark("invalid tld (%s)", pos+1);
						goto CLEANUP;
					}
				} else {
					/* invalid: domain must contain '.' */
//bark("no . in domain, can't be url");
					goto CLEANUP;
				}
//bark("url?");
				/* should we lookup a DNS record for the domain? */
//				if (!in_a_tag(q, s) && !in_a_href(q, s) &&
				if (!in_a_tag(q, s) &&
				    /* if the protocol was found, the TLD must be allowed */
				    ((Buffer_is_set(proto) &&
//				    ((strcmp(Buffer_get(proto), "http://") == 0) ||
//				    (strcmp(Buffer_get(proto), "https://") == 0)) &&
				    in_vec(Buffer_get(proto), prefs->allowed_protos, 0, TRUE)) ||
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
					if (Buffer_is_set(url)) {
//bark("URL found: %s", Buffer_get(url));
						Buffer_append(p, url);
					} else if (((Buffer_is_set(proto) &&
						   ((strcmp(Buffer_get(proto), "http://")  == 0) ||
						    (strcmp(Buffer_get(proto), "https://") == 0))) ||
						   !Buffer_is_set(proto))) {
//bark("no URL found");
						Buffer_addch(p, '/');
					}

					Buffer_cat(p, "\">");
					Buffer_cat_range(p, q, t);
					Buffer_cat(p, "</a>");

					q = t;
				}
			}
CLEANUP:
			Buffer_addch(p, *q);

			if (Buffer_is_set(proto))
				Buffer_free(&proto);
			if (Buffer_is_set(domain))
				Buffer_free(&domain);
			if (Buffer_is_set(url))
				Buffer_free(&url);
		}
		free(r);
		r = Buffer_get(p);
		Buffer_long_free(&p, TRUE);
	}

	/* auto e-mail */

	/* fix newlines */
	p = Buffer_init(strlen(r));
	ignore = FALSE;
	for (q = r; *q != '\0'; q++) {
		if (*q == '<')
			ignore = TRUE;
		else if (*q == '>')
			ignore = FALSE;
		else if (!ignore) {
			if (*q == '\n') {
				Buffer_cat(p, "<br />");
				continue;
			} else if ((*q == '\r') && (q[1] == '\n')) {
				Buffer_cat(p, "<br />");
				q++;
				continue;
			/* try this last */
			} else if (*q == '\r') {
				Buffer_cat(p, "<br />");
				continue;
			}
		}
		Buffer_addch(p, *q);
	}
	free(r);
	r = Buffer_get(p);
	Buffer_long_free(&p, TRUE);

bark("newlines fixed, doing run-ons now");

	/* fix run-ons */
	p = Buffer_init(strlen(r));
	count = 0;
	for (q = r; *q != '\0'; q++) {
		/* a problem is (?:&#nbsp;)* */
		if (strchr("&<()> \t/!?|.,", *q) != NULL)
			count = 0;
		else
			count++;

		if (count > prefs->max_chars) {
			Buffer_addch(p, ' ');
			Buffer_addch(p, *q);
			/* next char counts */
			count = 1;
		} else {
			Buffer_addch(p, *q);
		}
	}

	free(r);
	r = Buffer_get(p);
	Buffer_long_free(&p, TRUE);

bark("original: %s", s);

	return r;
}
