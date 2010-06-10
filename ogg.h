#indef __XA1_OGG_H__
#define __XA1_OGG_H__
struct parseme ogg_page[] =
	{
		{"Capture Patern", 32, UINT32STR('O', 'g', 'g', 'S')},
		{"Version", 8, 0, _check_zero},
		{"Header Type", 8},
		{"Granule Position", 64},
		{"Bitstream Serial Number", 32},
		{"Page Sequence Number", 32},
		{"Checksum", 32},
		{"Page Segment", 8},
		{"Segment Table", 8},
		{NULL}
	};

#endif /* __XA1_OGG_H__ */
