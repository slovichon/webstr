#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "webstr.h"

int main(int argc, char *argv[])
{
	char *p,*q;
	struct webstr_prefs prefs;

	prefs.auto_urls = TRUE;
	prefs.max_chars = 20;

	prefs.auto_tlds = calloc(4, sizeof(char *));
	prefs.auto_tlds[0] = "co";
	prefs.auto_tlds[1] = "com";
	prefs.auto_tlds[2] = "edu";
	prefs.auto_tlds[3] = NULL;
	
	prefs.allowed_protos = calloc(5, sizeof(char *));
	prefs.allowed_protos[0] = "http://";
	prefs.allowed_protos[1] = "mailto:";
	prefs.allowed_protos[2] = "https://";
	prefs.allowed_protos[3] = "gopher:";
	prefs.allowed_protos[4] = NULL;

	prefs.allowed_html = calloc(3, sizeof(struct webstr_allowed_html));

	prefs.allowed_html[0].tag = "a";
	prefs.allowed_html[0].attrs = calloc(3, sizeof(char *));
	prefs.allowed_html[0].attrs[0] = "href";
	prefs.allowed_html[0].attrs[1] = "name";
	prefs.allowed_html[0].attrs[2] = NULL;

	prefs.allowed_html[1].tag = "span";
	prefs.allowed_html[1].attrs = calloc(5, sizeof(char *));
	prefs.allowed_html[1].attrs[0] = "style";
	prefs.allowed_html[1].attrs[1] = "title";
	prefs.allowed_html[1].attrs[2] = "id";
	prefs.allowed_html[1].attrs[3] = "lang";
	prefs.allowed_html[1].attrs[4] = NULL;

	prefs.allowed_html[2].tag = NULL;

/*
	for (q = prefs.auto_tlds; *q; q++)
		printf("%s\n", *q);
*/
	p = webstr_parse(
//	"here is some content\n"
//	"here are some <invalid> tags </invalid>\n"
//	"here are some <valid> tags </valid>\n"
	"hello <span style=\"foo:bar; glarch:nilch; position:expression(...)\">there</span> world\n"
//	"here is some \"miscellaneous\" & inappropriate >content<\n"
//	" asdf gopher:comp.unix.bsd.openbsd.pf jkl;\n"
//	"here is a <tag with=\"an\"> invalid attribute </tag>\n"
//	"here is a <tag style='bad:css'> yep </tag>\n"
//	"here is a tag with <a style='attribute'> yep </a>\n"
//	"here is a link with <a href=\"malicious:foo\"> url </a>\n"
	//"hereisarunonsentencelalalalalalalalalalalalalalalalalalal\n",
//	"lalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalalala\n"
	, STR_ALL, &prefs);

	printf("parsed: %s\n", p);
	free(p);
	return 0;
}
