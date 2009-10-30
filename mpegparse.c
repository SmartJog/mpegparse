#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <endian.h>
#include "libxa1dump.h"

#define MIN(X,Y) ((X)<(Y))?(X):(Y)
#define MAX(X,Y) ((X)>(Y))?(X):(Y)

struct parseme {
	char	*name;
	ssize_t size;
	uint32_t def;
	uint32_t data;
};


struct parseme mpeg_ps_stream_h[] =
	{
		{"sync", 32, 0x000001ba},
		{"mark0", 2, 1},
		{"scr32_30", 3, 0},
		{"mark1", 1, 1},
		{"scr29_15", 15, 0},
		{"mark2", 1, 1},
		{"scr14_0", 15, 0},
		{"mark3", 1, 1},
		{"scr_ext", 9, 0},
		{"mark4", 1, 1},
		{"pmr", 22, 0},
		{"mark5", 2, 3},
		{"reserved", 5, 31},
		{"psl", 3, 0},
		{NULL, 0, -1, 0}
	};

struct parseme mpeg_ps_stream_sys_h[] =
	{
		{"sync", 32, 0x000001bb},
		{"header_length", 16, 0},
		{"mark0", 1, 1},
		{"rate_bound", 22, 0},
		{"mark1", 1, 1},
		{"audio_bound", 6, 0},

		{"fixed_flag", 1, 0},
		{"CSPS_flag", 1, 0},
		{"system_audio_lock_flag", 1, 0},
		{"system_video_lock_flag", 1, 0},
		{"mark2", 1, 1},

		{"video_bound", 5, 0},
		{"packet_rate_restriction_flag", 1, 0},
		{"reserved_bits", 7, 0x7f},
		{NULL, 0, -1, 0}
	};

int parse (char *b, size_t bufsiz, struct parseme p[], uint32_t **endptr) {
	int i, j;
	uint8_t offset = 0;
	char *buf = b;
	char d = *buf;

	int pad = 0;
	int overflow = 0;

	ssize_t size = 0;

	size_t align =  sizeof(buf[0])*8;

	for ( i = 0; p[i].name; i++) {
		if (size < 1) size = p[i].size;

		if (offset + size < align)
			pad = align - (offset + size);
		else
			pad = 0;

		p[i].data |= *buf>>pad & ~(~0<<(align - (offset + pad)));

		if (offset + size > align) { /* we overflow, so offset is now 0 */
			p[i].data = p[i].data<<(MIN(size, align));
			size -= (align - (offset + pad));
			offset = 0;
			i--;
		} else {
			if (p[i].def && p[i].data != p[i].def)
				goto err;
			size -= (align - (offset + pad));
			offset = pad?align-pad:0;
		}

		if (!offset) buf++;
	}

	printf ("parsing succeded !\n");
	for ( i = 0; p[i].name; i++)
		printf ("%s'%s' \t-> '%u' \n",  p[i].def?"*":" ", p[i].name, p[i].data);

	if (endptr)
		*endptr = buf;

	return 1;

 err: printf ("at 0x%08x, offset %d:%d, %s should be '%02x' but is '%02x'.\n",
	      (buf - b)*4, buf - b, offset, p[i].name, p[i].def, p[i].data);
	return 0;
}

struct mpeg_ps_stream_header {
	/*  0 */
	uint32_t	sync;	/* 0x000001ba */
	/* 32 */
	uint8_t		mark0:6; 	/* bx01 */
	uint8_t		scr32_30:3;
	uint8_t		mark1:1;	/* bx1 */
	uint16_t	scr29_15:15;
	uint8_t		mark2:1;	/* bx1 */
	uint16_t	scr14_0:15;
	uint8_t		mark3:1;	/* bx1 */
	uint16_t	scr_ext:9;
	uint8_t		mark4:1;	/* bx1 */
	uint32_t	pmr:22;		/* In units of 50 bytes per second */
	uint8_t		mark5:2;	/* bx11 */
	uint8_t		reserved:5;
	uint8_t		psl:3;
	char		sb[0];
};

int main (int argc, char *argv[]) {
	int 		fd;
	size_t		len, tot = 0;
	uint32_t	buf[8];
	uint32_t	*p = buf;
	int i;

	struct mpeg_ps_stream_header *psh;

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
		xa1_dump(p);
		if (! parse(p, len, mpeg_ps_stream_h, &p))
			continue;

		xa1_dump(p);
		if (! parse(p, len, mpeg_ps_stream_sys_h, &p))
			continue;

		//		psh = (struct mpeg_ps_stream_header *) buf;

		/*
		if (be32toh(psh->sync) != 0x000001ba) {
			printf ("No valid sync code (0x%08x) at %08x.\n", be32toh(psh->sync), tot);
			continue;
		}
		if (psh->mark0 != 1) {
			printf ("maker #0 (0x%02x) is not at 1.\n", psh->mark0);
			//			continue;
		}
		if (psh->mark1 != 1) {
			printf ("maker #1 (0x%02x) is not at 1.\n", psh->mark1);
			continue;
		}
		if (psh->mark2 != 1) {
			printf ("maker #2 is not at 1.\n");
			continue;
		}
		if (psh->mark3 != 1) {
			printf ("maker #3 is not at 1.\n");
			continue;
		}
		if (psh->mark4 != 1) {
			printf ("maker #4 is not at 1.\n");
			continue;
		}
		if (psh->mark5 != 4) {
			printf ("maker #2 is not at bx11 (3).\n");
			continue;
		}
		*/
		printf ("Found valid structure at 0x%08x.\n", tot);
		break;


	}
}



