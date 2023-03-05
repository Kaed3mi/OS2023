#include <blib.h>

size_t strlen(const char *s) {
	size_t res = 0;
	while (*(s + res)) res++;
	return res;
}

char *strcpy(char *dst, const char *src) {
	int i;
	for (i = 0; src[i]; i++)
		dst[i] = src[i];
	dst[i] = '\0';
	return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
	char *res = dst;
	while (*src && n--) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

char *strcat(char *dst, const char *src) {
	size_t dst_len = strlen(dst);
	size_t i;
	for (i = 0; src[i] != '\0'; i++) {
		dst[dst_len + i] = src[i];
		dst[dst_len + i] = '\0';
	}
	return dst;
}

int strcmp(const char *s1, const char *s2) {
	    for(; *s1==*s2 ; ++s1,++s2)
		 if(*s1=='\0')
			return(0);
	        return((*(unsigned char*)s1 < *(unsigned char*)s2)? -1 : +1);
}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n--) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		if (*s1 == 0) {
			break;
		}
		s1++;
		s2++;
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
	size_t i;
	char * sp = (char *)s;
	for (i = 0; i < n; i++)
		*(sp + i) = c;
	return s;
}

void *memcpy(void *out, const void *in, size_t n) {
	char *csrc = (char *)in;
	char *cdest = (char *)out;
	for (int i = 0; i < n; i++) {
		cdest[i] = csrc[i];
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
		const char *pstr1 = (const char*)s1;
		const char *pstr2 = (const char*)s2;
		while (n--) {
			 if (*pstr1 && *pstr2 && (*pstr1 == *pstr2)) {
				continue;
			 } else {
				 break;
			 }
		}
	return (*pstr1 - *pstr2);
}
