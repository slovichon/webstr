#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include "webstr.h"

bool valid_domain(char *domain)
{
	struct hostent *h, *t;
	struct in_addr addr;
	char **ip, alias_ip[15]; /* xxx.xxx.xxx.xxx */

bark("testing validity of %s", domain);
	h = gethostbyname(domain);

	if (h == NULL)
		return FALSE;

bark("host: %s", h->h_name);

	for (ip = h->h_addr_list; *ip != NULL; ip++) {
		snprintf(alias_ip, 15, "%d.%d.%d.%d",
			(ip[0][0]+256)%256,
			(ip[0][1]+256)%256,
			(ip[0][2]+256)%256,
			(ip[0][3]+256)%256);
bark("alias ip: %s ", alias_ip);
		if (inet_aton(alias_ip, &addr)) {
			t = gethostbyaddr(&addr, sizeof addr, AF_INET);
bark("[legit: %s]\n", t->h_name);
			/* do we have to scan the list of aliases? */
			if (t != NULL) {
				if (strcmp(domain, t->h_name) == 0) {
bark("domain name mapped properly");
					return TRUE;
				}
			}
		} else
			printf("[no legit]\n");
	}
	
bark("domain name *not* mapped properly");
	return FALSE;
}
