/*
 * libxa1dump.h
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

#ifndef _LIBXA1_DUMP_H_
#define _LIBXA1_DUMP_H_

#include <unistd.h>

#define xa1_dump(x) xa1_dump_size((char *) x, sizeof(x))
#define xa1_dumpa(a,x)				\
	do {					\
		printf ("%08x  ", a);		\
		xa1_dump(x);			\
	} while (0)

#define xa1_printf(t, b) _xa1_printf_size (t, #t ": ", b, sizeof(b));
#define xa1_printf_size(t, b, s) _xa1_printf_size(t, #t ": ",  b, s)
#define _xa1_printf_size(t, h, b, s)		\
	do  {					\
		char __p[256];			\
		xa1_##t##sprintf(b, __p, s);	\
		printf("%s%s\n", h, __p);	\
	} while (0)

#define xa1_HEXsprintf(...) xa1_hsprintf(__VA_ARGS__)
#define xa1_BINsprintf(...) xa1_bsprintf(__VA_ARGS__)
#define xa1_ASCsprintf(...) xa1_asprintf(__VA_ARGS__)

void xa1_dump_size (char *buf, size_t bufsiz);
char *xa1_bsprintf(char *p, char *op, size_t bufsiz);
char *xa1_hsprintf(char *p, char *op, size_t bufsiz);
char *xa1_asprintf(char *p, char *op, size_t bufsiz);

#endif /* _LIBXA1_DUMP_H_ */
