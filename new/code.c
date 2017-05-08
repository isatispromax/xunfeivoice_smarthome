#include <iconv.h>
#include <stdio.h>
#include <string.h>
 
int code_convert(const char *from_charset, const char *to_charset, char *inbuf,size_t inlen, char *outbuf, size_t outlen)
{
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;
 
	cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
		return -1;
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) != 0)
		return -1;
	iconv_close(cd);
	return 0;
}
 
int u2g(char *inbuf, int inlen, char *outbuf, int outlen)
{
	return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
}
 
int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
}
 
int main()
{
	FILE*    fp_test;
	const char* utf8 = "ÄúºÃÖÐ¹ú";
	char buff[1024] ={0};
	printf("uft8 len %x %d\n",utf8,strlen(utf8));
	g2u((char*) utf8, strlen(utf8), buff, sizeof(buff));
	
	fp_test = fopen("recivebuff.txt","wb");
	fwrite(buff,1,sizeof(buff),fp_test);
	fclose(fp_test);
	printf("%s\n", utf8);
	printf("%s\n", buff);
	return 0;
}