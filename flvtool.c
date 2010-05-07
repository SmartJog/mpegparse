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
	int 		fd, bytes;
	ssize_t		len;
	size_t		offset = 0, readc = 0;

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

	h = mpegparse_new (flv_header);
	t = mpegparse_new (flv_tag);

	refbuf = refbuf_new (4096);

	if (!h || !t || !refbuf) {
		perror ("memory issue");
		return 0;
	}

	refbuf->len = read (fd, refbuf->data, refbuf->len); /* not really ideal */
	bytes = parse (refbuf->data, refbuf->len, h, NULL);
	if (bytes <= 0)
		goto err_parse;

	readc += bytes;

	while (1) {
		while ((bytes = parse(refbuf->data + readc, refbuf->len - readc, t, NULL)) > 0) {
			if (refbuf->len - bytes - readc < FLV_TAG_SIZE) {
				printf("buffer %d too small, bail out\n", refbuf->len - readc);
				bytes = -bytes + FLV_TAG_SIZE;
				break;
			}
			readc += bytes;
		}
		if  (!bytes) {
			printf("Couldn't parse FLV Metadata at %d, %s\n",
			       readc, refbuf->data + readc);
			goto err_parse;
		}


		offset = refbuf-> len - readc;
		memmove (refbuf->data, refbuf->data + readc, offset);
		if (bytes + refbuf->len < 0)
			refbuf = refbuf_realloc (refbuf, -bytes + FLV_TAG_SIZE);

		for (readc = 0; readc < refbuf->len;) {
			bytes = read (fd, refbuf->data + offset, refbuf->len);
			if (bytes)
				readc += bytes;
			else
				goto err_readc;
		}
		readc = 0;
	}

 err_parse:
	printf ("ERROR: Couldn't parse (bytes = %d)\n", bytes);
	xa1_dump_size (refbuf->data, refbuf->len);
	return 0; /* need to free */

 err_readc:
	printf ("ERROR while reading %m\n");
	return 0;
}
