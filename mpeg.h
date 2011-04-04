/*
 * mpeg.h
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

#indef __XA1_MPEG_H__
#define __XA1_MPEG_H__
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
		{NULL}
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
		{NULL}
	};

#endif /* __XA1_MPEG_H__ */
