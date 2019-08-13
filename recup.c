/*
  recup.c = recuperador
*/
#include <stdio.h>
#include <unistd.h>
#include "archi.h"
#include <errno.h>
void recup(int f)
{
  int i, rd;
  char buf[512]; short int *rsiz;
  long lug;
  lug = ar[f].a_lug;
  if(lug != 0l) lug = lug + (long) ar[f].a_sz;
  errno = 0;
  while(lseek(ar[f].a_fd,lug,0),(rd = read(ar[f].a_fd,buf,512)) != 0) {
    if(errno != 0) {
     if(errno == 4) {errno = 0; continue; }
     err("Check Situation. \n");
    }
    for(i=0;i<rd;i++) {
     if(buf[i] == 0x1e) {
       if(i <= (rd - 4)) {
         rsiz = (short*)(buf+i+2);
         if(*rsiz == ar[f].a_sz) { ar[f].a_lug = lug + (long) i; return; }
         else continue;
       }
       else {
         lug = lug + (long) i; break;
       }
     }
    }
    lug += 512l;
  }
  ar[f].a_lug = 0l; return;
}

void sizfil(int f,char *ptr)
{
  void *getad();
  *(long*)getad(ptr) = lseek(ar[f].a_fd,0l,2);
}
