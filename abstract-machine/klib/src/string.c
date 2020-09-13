#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  char *p=s;
  int count=0;
  while(*(p++)!='/0') count++;
  return count;
}

char *strcpy(char* dst,const char* src) {
  strncpy(dst,src,strlen(src));
  return NULL;
}

char* strncpy(char* dst, const char* src, size_t n) {
  char *p1=dst;
  char *p2=src;
  for(int i=0;i<n;i++){
    *p1=*p2;
    if(*p2=='\0') break;
  }
  return NULL;
}

char* strcat(char* dst, const char* src) {
  return NULL;
}

int strcmp(const char* s1, const char* s2) {
  char *ss1=s1;
  char *ss2=s2;
  while(*ss1!='/0' && *ss2!='/0'){
    if(*(ss1++)!=*(ss2++)) return -1;
  }
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  char *ss1=s1;
  char *ss2=s2;
  for(int i=0;i<n;i++){
    if(*ss1!='/0' && *ss2!='/0'){
      if(*(ss1++)!=*(ss2++)) return -1;
    }
    else if(*ss1=='/0' && *ss2=='/0') break;
    else return -1;
  }
  return 0;
}

void* memset(void* v,int c,size_t n) {
  return NULL;
}

void* memmove(void* dst,const void* src,size_t n) {
  return NULL;
}

void* memcpy(void* out, const void* in, size_t n) {
  return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  return 0;
}

#endif
