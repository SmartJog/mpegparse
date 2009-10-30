#include <stdio.h>

#define P(x) (isprint(x)?x:'.')
char hextbl[] = {'0', '1', '2', '3', '4', '5', '6', '7',
		 '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

char *xa1_bsprintf(char *p, char *op, size_t bufsiz) {
	int i, j;

	for (j = 0; j < bufsiz; j++) {
		for (i = 0; i < 8; i++) {
			*op++ = (*p>>(7 - i) & 1)?'1':'0';
		}
		*op++ = ' ';
		p++;
	}

	*(op + 1) = '\0';

	return op;
}

char *xa1_hsprintf(char *buf, char *op, size_t bufsiz) {
	int j;

	for (j = 0; j < bufsiz; j++) {
		*op++ = hextbl[(buf[j]>>4)&0xf];
		*op++ = hextbl[buf[j]&0xf];
	}

	*(op + 1) = '\0';

	return op;
}

char *xa1_asprintf(char *buf, char *op, size_t bufsiz) {
	int j;

	for (j = 0; j < bufsiz; j++)
		*op++ = P(buf[j]);

	*(op + 1) = '\0';

	return op;
}


void xa1_dump_size (char *buf, size_t bufsiz) {
	static char outbuf[8192];
	char *p;

	p = outbuf;

	*p++ = '|';
	*p++ = ' ';

	p = xa1_bsprintf(buf, p, bufsiz);

	*p++ = '|';

	p = xa1_hsprintf(buf, p, bufsiz);

	*p++ = '|';

	p = xa1_asprintf(buf, p, bufsiz);

	*p++ = '|';

	printf ("%s\n", outbuf);
}

