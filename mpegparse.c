/*
 * mpegparse.c
 * This file is part of mpegparse.
 *
 * Copyright (C) 2011 - SmartJog S.A.S.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <endian.h>
#include "mpegparse.h"

#define MIN(X,Y) ((X)<(Y))?(X):(Y)
#define MAX(X,Y) ((X)>(Y))?(X):(Y)

const char _PM_EMBEDDED[] = "Embeded Parseme";
const char _PM_EMBEDDED_A[] = "Embeded Parseme Array";

int _check_zero (parseme_t p[], int i, char *buf, size_t buflen) {
	dprintf("checking with %s\n", __func__);
	if (p[i].data == 0)
		return 1;

	PM_CHECK_FAIL;
	return 0;
}

int _check_range (parseme_t p[], int i, char *buf, size_t buflen) {
	dprintf("checking with %s\n", __func__);
	uint16_t a = UINT32GET12(p[i].def);
	uint16_t b = UINT32GET22(p[i].def);
	pm_data_type d = p[i].data;

	if (a > b)
		SWAP(a,b);
	if ((d < a) || (d > b)) {
		PM_CHECK_FAIL;
		return 0;
	}
	return 1;
}

void mpegparse_destroy(parseme_t *p) {
	free (p);
}

parseme_t *_mpegparse_new (parseme_t *p, size_t nmemb) {
	parseme_t *ret = calloc (nmemb, sizeof(*ret));
	if (!ret)
		return NULL;

	memcpy (ret, p, nmemb*sizeof(*ret));
	//	dprintf("returning %p\n", ret);
	return ret;
}

int pm_get_key(parseme_t p[], char *key, pm_data_type *value) {
	int i;

	for ( i = 0; p[i].name; i++ ) {
		dprintf("trying for '%s'.\n", p[i].name);
		if (p[i].name == _PM_EMBEDDED) {
			dprintf("trying _PM_EMBEDDED\n");
			if (pm_get_key(_PM_EMBEDDED_GETPM(p[i]), key, value))
				return 1;
		}

		if (strcmp (p[i].name, key) == 0) {
			dprintf("ok for '%s'.\n", p[i].name);
			*value = p[i].data;
			return 1;
		}
	}
	return 0;
}

int parse (char *b, size_t bufsiz, parseme_t p[], char **endptr) {
	int i;
	uint8_t offset = 0;
	char *buf = b;

	int pad = 0;
	int tot = 0; /* XXX(xaiki): hack */
	ssize_t size = 0;

	size_t align =  sizeof(buf[0])*8;

	if (endptr)
		*endptr = b;

	/* XXX(xaiki): hack */
	for ( i = 0; p[i].name; i++ ) {
		dprintf("p[%d](%s).size = %d.\n", i, p[i].name, p[i].size/8);
		tot += p[i].size/8;	/* XXX(xaiki): hack */
		p[i].data = 0;
	}

	/* XXX(xaiki): hack */
	if (bufsiz < tot) {
		dprintf("invalid bufsiz = %u, need at least %u .\n", bufsiz, tot);
		return 0; /* we don't know how much we'll need to parse this */
	}

	for ( i = 0; p[i].name; i++) {
		dprintf("trying on %s (%u), %u left\n", p[i].name, size?size:p[i].size, bufsiz - (buf - b));
		if ((p[i].name == _PM_EMBEDDED) && (p[i].check)) {
			p[i].size = parse (buf, bufsiz - (buf - b), _PM_EMBEDDED_GETPM(p[i]), NULL);
			if (p[i].size <= 0) {
				dprintf("FAILED, UNHANDLED\n"); /* XXX(xaiki): handle */
				p[i].size = 0;
			} else {
				//	buf += p[i].size;
				p[i].size = 0;
				//if (p[i+1].name)
				//p[i+1].size -= p[i].size; /* humm, well, ok, why not ... */
			}
			dprintf("Returned: %d\n", p[i].size);
			continue;
		}

		if (p[i].size == 0) {
			dprintf("size == 0 ? probably some unhandled stuff, skip for now.\n");
			continue;
		} else if (p[i].size > sizeof(pm_data_type)*8) {
			tot = p[i].size/8;
			p[i].size = 0;

			if (offset) {
				dprintf("No offset: Not Implemeted.\n");
				goto err_print;
			}
			if (p[i].def) {
				dprintf("def '%u' checking: Not Implemeted.\n", p[i].def);
				goto err_print;
			}

			if (p[i].check && ! p[i].check(p, i, buf, bufsiz - (buf - b)))
				goto err_print;

			p[i].data = -1;

			if (tot > bufsiz - (buf - b)) {
				dprintf("Buf too small !!! %u left, and I still want to put %d ! total packet size is %d\n",
					bufsiz - (buf - b), tot, buf - b + tot);
#ifdef NDEBUG
				int j;
				for ( j = 0; p[j].name; j++)
					if (p[j].size <= sizeof(pm_data_type)*8)
						dprintf ("%s'%s:%u'->'%u'",  p[j].def?"*":p[j].check?"=":" ", p[j].name, p[j].size, p[j].data);
				dprintf("\n");
#endif
				return -tot;
			}
			buf += tot;
			continue;
		}

		if (size < 1)
		retry:
			size = p[i].size;

		if (offset + size < align)
			pad = align - (offset + size);
		else
			pad = 0;

		p[i].data |= *buf>>pad & ~(~0<<(align - (offset + pad)));


		if (offset + size > align) { /* we overflow */
			p[i].data = p[i].data<<(MIN(size, align));
			size -= (align - (offset + pad));
			offset = 0;
			i--;
		} else {
			if (p[i].check) {
				int t = p[i].size;
				if (! p[i].check(p, i, buf, bufsiz - (buf - b))) {
					printf ("at 0x%08x, offset %ld:%d, Field '%s':'%d' couldn't be checked by '%p'.\n",
						(unsigned int) (buf - b)*4, buf - b, offset, p[i].name, p[i].data, p[i].check);
					goto err;
				}
				if (t != p[i].size) /* updated, retry */
					goto retry;
			} else if (p[i].def && p[i].data != p[i].def) {
				printf ("at 0x%08x, offset %ld:%d, Field '%s' should be '%02x' but is '%02x'.\n",
					(unsigned int) (buf - b)*4, buf - b, offset, p[i].name, p[i].def, p[i].data);

				goto err;
			}

			size -= (align - (offset + pad));
			offset = pad?align-pad:0;
			/* HACK */
			if (p[i+1].data != 0) {
				dprintf("WARNING !!!!! DATA NOT 0 !!!!\n");
				p[i+1].data = 0;
			}
		}

		if (!offset) buf++;
	}

	dprintf ("parsing succeded !\n");
#ifdef DEBUG
	for ( i = 0; p[i].name; i++)
		if (p[i].size <= sizeof(pm_data_type)*8)
			dprintf ("%s'%s:%u' \t-> '%x' \n",  p[i].def?"*":p[i].check?"=":" ", p[i].name, p[i].size, p[i].data);
#endif

	if (endptr)
		*endptr = buf;

	return buf - b;

 err_print:
	printf ("at 0x%08x, offset %ld:%d, Field '%s' should be '%02x' but is '%02x'.\n",
		(unsigned int)(buf - b)*4, buf - b, offset, p[i].name, p[i].def, p[i].data);
 err:
	return 0;
}
