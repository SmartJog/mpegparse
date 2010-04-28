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
