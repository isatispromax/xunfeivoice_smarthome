#ifndef _CODECON_H
#define _CODECON_H

int code_convert(const char *from_charset, const char *to_charset, char *inbuf,size_t inlen, char *outbuf, size_t outlen);
int u2g(char *inbuf, int inlen, char *outbuf, int outlen);
int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

#endif


/******************* END LINE ******************/