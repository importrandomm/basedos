#include "stdint.h"
#include "string.h"

// Forward declarations for functions used before their definition
size_t strspn(const char* str, const char* accept);
size_t strcspn(const char* str, const char* reject);

// Memory manipulation functions
void* memset(void* ptr, int value, size_t num) {
    unsigned char* p = (unsigned char*)ptr;
    while (num-- > 0) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}

void* memcpy(void* dest, const void* src, size_t num) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (num-- > 0) {
        *d++ = *s++;
    }
    return dest;
}

void* memmove(void* dest, const void* src, size_t num) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    
    if (d < s) {
        // Copy forwards
        while (num-- > 0) {
            *d++ = *s++;
        }
    } else {
        // Copy backwards
        d += num;
        s += num;
        while (num-- > 0) {
            *--d = *--s;
        }
    }
    return dest;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num) {
    const unsigned char* p1 = (const unsigned char*)ptr1;
    const unsigned char* p2 = (const unsigned char*)ptr2;
    
    while (num-- > 0) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

// String manipulation functions
char* strcat(char* dest, const char* src) {
    char* ptr = dest + strlen(dest);
    while (*src) {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dest;
}

char* strncat(char* dest, const char* src, size_t n) {
    char* ptr = dest + strlen(dest);
    while (*src && n-- > 0) {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == (char)c) {
            return (char*)str;
        }
        str++;
    }
    return NULL;
}

char* strrchr(const char* str, int c) {
    const char* last = NULL;
    while (*str) {
        if (*str == (char)c) {
            last = str;
        }
        str++;
    }
    return (char*)last;
}

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

int strncmp(const char* s1, const char* s2, size_t n) {
 while (n && *s1 && (*s1 == *s2)) {
 s1++;
 s2++;
 n--;
 }
 if (n == 0) return 0;
 return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char* strtok(char* str, const char* delim) {
 static char* last = 0;
 if (str) last = str;
 if (!last || !*last) return 0;
 
 // Skip leading delimiters
 str = last + strspn(last, delim);
 if (!*str) {
 last = 0;
 return 0;
 }
 
 // Find end of token
 char* end = str + strcspn(str, delim);
 if (*end) {
 *end = 0;
 last = end + 1;
 } else {
 last = end;
 }
 return str;
}

size_t strspn(const char* str, const char* accept) {
 const char* p;
 const char* a;
 size_t count = 0;

 for (p = str; *p != '\0'; ++p) {
 for (a = accept; *a != '\0'; ++a) {
 if (*p == *a) {
 ++count;
 break;
 }
 }
 if (*a == '\0') {
 return count;
 }
 }
 return count;
}

size_t strcspn(const char* str, const char* reject) {
 const char* p;
 const char* r;
 size_t count = 0;

 for (p = str; *p != '\0'; ++p) {
 for (r = reject; *r != '\0'; ++r) {
 if (*p == *r) {
 return count;
 }
 }
 ++count;
 }
 return count;
}

int atoi(const char* str) {
 int result = 0;
 int sign = 1;
 
 // Skip whitespace
 while (*str == ' ' || *str == '\t' || *str == '\n')
        str++;
        
    // Handle optional sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Convert number
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

// Integer to string conversion
void itoa(int value, char* str, int base) {
    char* p = str;
    char* p1, *p2;
    unsigned long tmp;
    
    // Handle 0 explicitly, otherwise empty string is printed
    if (value == 0) {
        *p++ = '0';
        *p = '\0';
        return;
    }
    
    // Handle negative numbers for base 10
    if (value < 0 && base == 10) {
        *p++ = '-';
        str++;
        tmp = -value;
    } else {
        tmp = (unsigned int)value;
    }
    
    // Process individual digits
    while (tmp != 0) {
        int rem = tmp % base;
        *p++ = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        tmp = tmp / base;
    }
    
    // Add null terminator
    *p = '\0';
    
    // Reverse the string
    p1 = str;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}