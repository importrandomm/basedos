#include "basedos.h"

size_t strlen(const char* str) {
 size_t len = 0;
 while (str[len]) {
 len++;
 }
 return len;
}

char* strcpy(char* dest, const char* src) {
 char* ptr = dest;
 while ((*dest++ = *src++));
 return ptr;
}

int strcmp(const char* s1, const char* s2) {
 while (*s1 && (*s1 == *s2)) {
 s1++;
 s2++;
 }
 return *(unsigned char*)s1 - *(unsigned char*)s2;
}