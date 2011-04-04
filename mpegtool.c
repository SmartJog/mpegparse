/*
 * mpegtool.c
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
#include <sys/types.h>
#include <sys/stat.h>

#include "libxa1dump.h"
#include "mpeg.h"
#include "ebml.h"

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
