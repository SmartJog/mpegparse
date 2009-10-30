#ifndef _LIBXA1_DUMP_H_
#define _LIBXA1_DUMP_H_

#define xa1_dump(x) xa1_dump_size(x, sizeof(x))
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
