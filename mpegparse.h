/*
 * mpegparse.h
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

#ifndef __MPEGPARSE_H__
#define __MPEGPARSE_H__

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#ifdef DEBUG
#define dprintf(args...) fprintf (stderr, args);
#else
#define dprintf(args...)
#endif

#define UINT32STR(a, b, c, d) ((a<<24) + (b<<16) + (c<<8) + (d))
#define UINT24STR(b, c, d) ((b<<16) + (c<<8) + (d))
#define UINT16STR(c, d) ((c<<8) + (d))

#define SWAP(a,b)				\
	do {					\
		typeof(a) _tmp;			\
		_tmp = a;			\
		a = b;				\
		b = _tmp;			\
	} while(0);

#define UINT32ENC2(a, b) ((a<<16) + (b))
#define UINT32GET12(c) ((c)>>16)
#define UINT32GET22(c) ((c)&0xffff)

#define PM_RANGE(a,b) UINT32ENC2(a,b), _check_range

#define PM_CHECK_FAIL do {					\
		printf("Check failed in '%s'.\n", __func__);	\
	} while(0);


#define _PM_EMBEDDED_T(x) ((void *) (x))
#define _PM_EMBEDDED_EMBED(k, v) ((k).check = _PM_EMBEDDED_T(v))
#define _PM_EMBEDDED_GETPM(pi) ((parseme_t *)((pi).check))
extern const char _PM_EMBEDDED[];
extern const char _PM_EMBEDDED_A[];

typedef struct parseme parseme_t;

#define pm_data_type uint64_t

struct parseme {
	const char	*name;
	size_t		size;
	pm_data_type	def;
	int		(*check) (struct parseme *p, int field, char *buf, size_t buflen);
	pm_data_type 	data;
};

#define ARRAY_SIZE(p) (sizeof(p)/sizeof(p[0]))

int pm_get_key(parseme_t p[], char *key, pm_data_type *value);
void mpegparse_destroy (parseme_t *p);
parseme_t *_mpegparse_new (parseme_t *p, size_t nmemb);
#define mpegparse_new(p) _mpegparse_new(p, ARRAY_SIZE(p))
int _check_zero (parseme_t p[], int i, char *buf, size_t buflen);
int _check_range (parseme_t p[], int i, char *buf, size_t buflen);
int parse (char *b, size_t bufsiz, parseme_t p[], char **endptr);
#endif /* __MPEGPARSE_H__ */
