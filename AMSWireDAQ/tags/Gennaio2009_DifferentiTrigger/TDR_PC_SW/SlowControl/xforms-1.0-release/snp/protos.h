char *__dtoa(
		double d,
		int mode,
		int ndigits,
		int *decpt,
		int *sign,
		char **rve
		);

int snprintf(
		char *str,
		size_t n,
		char const *fmt,
		...
		);

double strtod(
		const char *s00,
		char **se
		);

unsigned long strtoul(
		char *nptr,
		char **endptr,
		register int base
		);

int vsnprintf(
		char *strbuf,
		int bufsize,
		const char *fmt0,
		va_list ap
		);


