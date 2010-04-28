#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libxa1dump.h"

int main (int argc, char *argv[]) {
	int fd;
	char buf[8];
	int len = 1;
	unsigned int tot = 0;
	int i, seen = 0;

	if (argc < 2) {
		perror ("Need more arguments");
		return 0;
	}

	fd = open (argv[1], O_RDONLY);
	if (fd == -1) {
		perror (argv[1]);
		return 0;
	}

	while ((len = read (fd, buf, sizeof(buf))) != 0) {
		int  nulls = 0;
		for (i = 0; i < sizeof(buf); i++) {
			if (buf[i] != 0)
				break;
			nulls++;
		}

		if (nulls == sizeof(buf)) {
			tot += len;
			if (! seen++)
				printf ("*\n");
			continue;
		}

		seen = 0;
		xa1_dumpa(tot, buf);
		tot += len;
	}

	return 0;
}






