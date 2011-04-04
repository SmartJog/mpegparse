/*
 * ebml.h
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

#ifndef __XA1_EBML_H__
#define __XA1_EBML_H__

#include <arpa/inet.h>
#include "mpegparse.h"

/* stollen from FFMPEG */
/* EBML version supported */
#define EBML_VERSION 1

/* top-level master-IDs */
#define EBML_ID_HEADER             0x1A45DFA3

/* IDs in the HEADER master */
#define EBML_ID_EBMLVERSION        0x4286
#define EBML_ID_EBMLREADVERSION    0x42F7
#define EBML_ID_EBMLMAXIDLENGTH    0x42F2
#define EBML_ID_EBMLMAXSIZELENGTH  0x42F3
#define EBML_ID_DOCTYPE            0x4282
#define EBML_ID_DOCTYPEVERSION     0x4287
#define EBML_ID_DOCTYPEREADVERSION 0x4285

/* general EBML types */
#define EBML_ID_VOID               0xEC
#define EBML_ID_CRC32              0xBF

typedef enum {
    EBML_NONE,
    EBML_UINT,
    EBML_FLOAT,
    EBML_STR,
    EBML_UTF8,
    EBML_BIN,
    EBML_NEST,
    EBML_PASS,
    EBML_STOP,
} EbmlType;

typedef const struct EbmlSyntax {
    uint32_t id;
    EbmlType type;
    int list_elem_size;
    int data_offset;
    union {
        uint64_t    u;
        double      f;
        const char *s;
        const struct EbmlSyntax *n;
    } def;
} EbmlSyntax;

typedef struct {
    int nb_elem;
    void *elem;
} EbmlList;

typedef struct {
    int      size;
    uint8_t *data;
    int64_t  pos;
} EbmlBin;

typedef struct {
    uint64_t version;
    uint64_t max_size;
    uint64_t id_length;
    char    *doctype;
    uint64_t doctype_version;
} Ebml;

static EbmlSyntax ebml_header[] = {
	{ EBML_ID_EBMLREADVERSION,        EBML_UINT,},
	{ EBML_ID_EBMLMAXSIZELENGTH,      EBML_UINT,},
	{ EBML_ID_EBMLMAXIDLENGTH,        EBML_UINT,},
	{ EBML_ID_DOCTYPE,                EBML_STR,},
	{ EBML_ID_DOCTYPEREADVERSION,     EBML_UINT,},
	{ EBML_ID_EBMLVERSION,            EBML_NONE },
	{ EBML_ID_DOCTYPEVERSION,         EBML_NONE },
	{ 0 }
};

static EbmlSyntax ebml_syntax[] = {
    { EBML_ID_HEADER,                 EBML_NEST, 0, 0, {.n=ebml_header} },
    { 0 }
};
/* stollen from FFMPEG */

#ifndef NULL
#define NULL 0
#endif

enum {
	EBML_ID = 0,
	EBML_SIZE,
	EBML_DATA,
};

#define clean_bits(p, f, v) do {			\
	p[f].size = v; p[f].data &= (1<<v -1);		\
	} while (0)

#define first_bit(v)				\
	((v)>>7)?7:				\
	((v)>>6)?6:				\
	((v)>>5)?5:				\
	((v)>>4)?4:				\
	((v)>>3)?3:				\
	((v)>>2)?2:				\
	((v)>>1)?1:				\
		 0

int check_ebml_id (struct parseme p[], int field, char *buf, size_t buflen) {
	int b = first_bit(p[field].data);

	p[field].size = 64 - b*8;
	//	clean_bits (p, field, b);

	printf("ID firstbit = %d, new size = %d\n", b, p[EBML_ID].size);

	p[EBML_ID].check = NULL;
	return 1;
}

int check_ebml_update_size (struct parseme p[], int field, char *buf, size_t buflen) {
	printf("id: 0x%x\nsize %d\n",p[EBML_ID].data, p[EBML_SIZE].data);
	if (p[EBML_ID].data == 0x4282)
		p[EBML_SIZE].data *= 8;
	if (p[EBML_SIZE].data < 7)
		p[EBML_SIZE].data = 7;

	p[EBML_DATA].size = p[EBML_SIZE].data + 1;

	printf("setting data size to %d\n", p[EBML_SIZE].data);
	printf("so far\n");
	printf("id: 0x%x\nsize %d\n",p[EBML_ID].data, p[EBML_SIZE].data);

	if (p[EBML_ID].data == 0x4282) {
		printf ("string... don't know how to parse... have %d left %s\n", buflen, buf + 1);
	}

	p[field].check = NULL;
	return 1;
}

int check_ebml_size (struct parseme p[], int field, char *buf, size_t buflen) {
	int b = first_bit(p[field].data);
	printf("SIZE fb =%d s = %d d = 0x%x m = 0x%x\n", b, 64 - b*8, p[field].data, ((1<<b) - 1));

	p[field].size = 64 - b*8;
	p[field].data &= ((1<<b) - 1);

	p[EBML_SIZE].check = check_ebml_update_size;

	if (p[EBML_SIZE].size == 8)
		check_ebml_update_size(p, field, buf, buflen);

	printf("SIZE firstbit = %d, new size = %d\n", b,  p[EBML_SIZE].size);
	return 1;
}


struct parseme ebml[] =
	{
		{"ID", 8, 0, check_ebml_id},
		{"Size", 8, 0, check_ebml_size},
		{"Data", 32},
		{NULL}
	};

#endif /* __XA1_EBML_H__ */








