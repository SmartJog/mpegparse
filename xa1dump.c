/*
 * xa1dump.c
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






