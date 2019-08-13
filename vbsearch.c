#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <malloc.h>
#include "archi.h"
 struct ixrec {
  int fd; /* file descriptor of file */
  int tosize; /* total size of recod */
 };

struct ixrec *inx;

struct vpind {
 char sf; /* source file */
 long kpos; /* key position in source file */
 char kc[256]; /* pointer to characters of key */
 } ix;

 int ai; /* active index */
hrut(char *ptr,int fa)
{
 int i;
 long vbsearch(),inpos;
 char *key,*getad();
 static int totind;
 ++ptr;
 if(inx == 0) {
  if(*ptr == 'm') totind = atoi(ptr+1);
  else totind = 10;
  if((inx=calloc(totind,sizeof(struct ixrec))) == 0)
   { perror("Can't obtain memory for indexes."); exit(0); }
  for(i=0;i<totind;i++) inx[i].fd = -1;
  if(*ptr == 'm') return(0);
 }
 switch(*ptr) {
  case 'a': /* index_active */
   ai = atoi(ptr+1);
  break;
  case 'f': /* index_find */
   ++ptr; 
   key = getad(ptr);
   ar[fa].a_lug = vbsearch(key);
  break;
  case 's': /* index_search */
   ++ptr;
   key = getad(ptr);
   if((ar[fa].a_lug=vbsearch(key)) == -1)
    ar[fa].a_lug = ix.kpos;
  break;
  case 'n': /* index_next */
   casen:
   if(read(inx[ai].fd,&ix,inx[ai].tosize) != inx[ai].tosize)
    ar[fa].a_lug = -1l;
   else ar[fa].a_lug = ix.kpos;
  break;
  case 'p': /* index_prev */
   inpos = lseek(inx[ai].fd,0l,1);
   if(inpos == inx[ai].tosize) {
    ar[fa].a_lug = -1l; return(0); }
   lseek(inx[ai].fd,(long)-(inx[ai].tosize*2l),1);
  goto casen;
  case 't': /* index_top */
   lseek(inx[ai].fd,0l,0);
  goto casen;
  case 'b': /* index_bottom */
   lseek(inx[ai].fd,(long)(-inx[ai].tosize),2);
  goto casen;
  case 'o': /* index_open */
   if(inx[ai].fd != -1) close(inx[ai].fd);
   i = atoi(ptr+1); while(*ptr++ != ','); key = getad(ptr);
   if((inx[ai].fd = open(key,O_RDONLY)) == -1)
    return(2);
   inx[ai].tosize = i + sizeof(char) + sizeof(long);
  break;
  case 'C': /* index_close */
   if(inx[ai].fd != -1) {close(inx[ai].fd); inx[ai].fd = -1; }
  break;
  case 'A': /* index_close_all */
   for(i=0;i<totind;i++) if(inx[i].fd != -1) {close(inx[i].fd); inx[i].fd= -1;}
  break;
 } /* switch hrut */
 return(0);
} /* hrut */

long vbsearch(char *key)
{
 long rs,pos,posmin,maxreg,regread;
 int fd,r,sk;
 rs = inx[ai].tosize; fd = inx[ai].fd;
 sk = rs - sizeof(char) - sizeof(long) - 1;
 maxreg = lseek(fd,0l,2) / rs;
 regread = maxreg / 2l; posmin = 0l;
 do {
  lseek(fd,regread*rs,0);
  read(fd,&ix,(int)rs);
  if((r=strncmp(key,ix.kc,sk)) == 0) {
   return(ix.kpos);
  }
  if(r>0) {
   posmin = regread+1l;
  }
  else {
   maxreg = regread-1l;
  }
  regread = (maxreg + posmin) / 2l;
 } while(posmin <= maxreg);
 return(-1l);
}
