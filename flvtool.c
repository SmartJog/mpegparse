#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "libxa1dump.h"
#include "flv.h"

typedef struct refbuf refbuf_t;
struct refbuf {
	char		*data;
	ssize_t		len;
};

refbuf_t *refbuf_new (size_t size) {
	refbuf_t *ret = malloc(sizeof(refbuf_t));

	if (!ret)
		return NULL;

	ret->data = malloc(size);
	if (!ret->data) {
		free (ret);
		return NULL;
	}
	ret->len = size;

	return ret;
}

refbuf_t *refbuf_realloc (refbuf_t *refbuf, size_t newsize) {
	char *newdata;

	dprintf("reallocing to %d\n", newsize);

	if (!refbuf)
		return refbuf_new (newsize);

	newdata = realloc(refbuf->data, newsize);
	if (!newdata)
		return NULL;

	refbuf->len = newsize;
	refbuf->data = newdata;

	return refbuf;
}

void refbuf_destroy (refbuf_t *refbuf) {
	free(refbuf->data);
	free(refbuf);
}

int main (int argc, char *argv[]) {
	int 		fd, bytes, last = 0;
	ssize_t		len;
	size_t		offset = 0, readc = 0, tot = 0, fill = 0;
	struct stat	statb;

	refbuf_t	*refbuf;
	parseme_t	*h, *t;

	if (argc < 2) {
		perror ("Need more arguments");
		return 0;
	}

	fd = open (argv[1], O_RDONLY);
	if (fd == -1) {
		perror (argv[1]);
		return 0;
	}

	if (fstat (fd, &statb) == -1) {
		perror (argv[1]);
		return 0;
	}

	h = mpegparse_new (flv_header);
	t = mpegparse_new (flv_tag);

	refbuf = refbuf_new (4096);

	if (!h || !t || !refbuf) {
		perror ("memory issue");
		return 0;
	}

	fill = read (fd, refbuf->data, refbuf->len); /* not really ideal */
	if (refbuf->len <= 0) {
		perror ("couldn't read");
		return 0;
	}

	bytes = parse (refbuf->data, fill, h, NULL);
	if (bytes <= 0)
		goto err_parse;
	tot += bytes;
	readc += bytes;

	while (1) {
		for (readc = 0; fill + readc < refbuf->len;) {
			bytes = read (fd, refbuf->data + offset, refbuf->len);
			if (bytes <= 0) {
				printf ("tot(%d) + readc(%d) [%d] statb.st_size (%d)\n",
					tot, readc, tot + readc, statb.st_size);
				if (tot + readc >= statb.st_size) {
					last = 1;
					refbuf->len = readc;
					break;
				}
				goto err_readc;
			}
			readc += bytes;
		}
		fill += readc;
		xa1_dump_size (refbuf->data + readc, 8);

		while ((bytes = parse(refbuf->data + readc, refbuf->len - readc, t, NULL)) > 0) {
			pm_data_type val;
			if (refbuf->len - bytes - readc < FLV_TAG_SIZE) {
				xa1_dump_size (refbuf->data + readc, 8);
				tot += bytes;
				bytes = -bytes + FLV_TAG_SIZE;
				break;
			}

			if (pm_get_key (t, "PreviousTagSize", &val))
				printf ("[%d] PrviousTagSize: %d\n", tot, val);
			xa1_dump_size (refbuf->data + readc, 8);

			readc += bytes;
			tot += bytes;
		}
		if  (!bytes)
			goto err_parse;

		offset = refbuf-> len - readc;
		memmove (refbuf->data, refbuf->data + readc, offset);
		if (bytes + refbuf->len < 0 && !last) {
			refbuf = refbuf_realloc (refbuf, -bytes + FLV_TAG_SIZE);
			fill = offset;
		}

		printf ("[%d/%d] need %d more \n", tot, statb.st_size, -bytes);
	}

 err_parse:
	printf ("ERROR: [%d/%d] Couldn't parse (bytes = %d, readc = %d)\n", tot, statb.st_size, bytes, readc);
	printf("[%d]", tot - 8);
	xa1_dump_size (refbuf->data + readc -8, 8);
	printf("[%d]", tot);
	xa1_dump_size (refbuf->data + readc, 8);
	printf("[%d]", tot + 8);
	xa1_dump_size (refbuf->data + readc + 8, 8);
	return 0; /* need to free */

 err_readc:
	printf ("ERROR while reading at %d/%d, read %d needed %d %m\n", tot, statb.st_size, readc, refbuf->len);
	return 0;
}
