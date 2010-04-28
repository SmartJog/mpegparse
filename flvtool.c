#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "libxa1dump.h"
#include "flv.h"

int main (int argc, char *argv[]) {
	int 	fd;
	size_t	len, tot = 0;
	char	buf[4096];
	char	*p = buf, *q = NULL;
	int i;

	if (argc < 2) {
		perror ("Need more arguments");
		return 0;
	}

	fd = open (argv[1], O_RDONLY);
	if (fd == -1) {
		perror (argv[1]);
		return 0;
	}

	for (;(len = read (fd, buf, sizeof(buf)) != 0); tot += len) {
		xa1_dumpa(tot, p);
		if (parse((char *) p, len, flv_header, &q))
			break;
	}
			tot += q - p;
			p = q;

	do {
		xa1_dumpa(tot, p);
		while( parse((char *) p, sizeof(buf), flv_tag, &q)) {
			tot += q - p;
			p = q;

			xa1_dumpa(tot, p);
		}
	}while (len = read (fd, buf, sizeof(buf)) != 0);
}
