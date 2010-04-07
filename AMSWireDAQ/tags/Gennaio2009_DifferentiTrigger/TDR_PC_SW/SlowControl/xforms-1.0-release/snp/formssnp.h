#if !defined( __FORMSSNP_H__ )
#define __FORMSSNP_H__

int snprintf(
                char *str,
                size_t n,
                char const *fmt,
                ...
                );

int vsnprintf(
                char *strbuf,
                int bufsize,
                const char *fmt0,
                va_list ap
                );

#endif

