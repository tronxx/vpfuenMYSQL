/* desglosador de llaves archivos indexados COBOL */
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include "vsamstr.h"
struct vsam vs;
struct prnrec prn;

main(argc,argv) int argc;char *argv[];
{
   int i,k,cam,fd;
   long rec;

   if( (fd=open(argv[1],O_RDONLY)) == 0) exit(0);
   cam = atoi(argv[2]);

   read(fd,&vs,VSAM);

   rec=vs.strk[cam].kpos;
   if( rec != 0l ) saca(fd,cam,rec,0);
}

saca(fd,cam,rec,lev) int cam,lev;long rec; int fd;
#define SZ vs.strk[cam].ksz
{
   char buf[135];
   int i;
   long reme,*nm,*rma;
   struct klug loc;
   nm=(long*)(buf+SZ);
   rma=(long*)(buf+SZ+4);
   lseek(fd,rec,SEEK_SET);
   read(fd,&loc,KLUG);
   reme = loc.nantl;
   if(reme != 0l) saca(fd,cam,reme,lev+1);
       for(i=0;i<loc.nkeys;i++) {
           lseek(fd,rec+(long)KLUG+(long)(i*(SZ+8)),SEEK_SET);
           read(fd,buf,SZ+8);
           fwrite(buf,SZ,1,stdout);
           fprintf(stdout,"\n");
           if(*rma != 0l) saca(fd,cam,*rma,lev+1);
       }
}
