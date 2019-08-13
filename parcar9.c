
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "vsamstr.h"
#define MAS10  315468000
#define MAX1 9999999999.99
#define MIN1 -9999999999.99

void gendat(char *dat);
void gendat2(char *dat);
int mues = 1,tofil,tofilm1,qnx2,fixpos;
char stTermi[8], stTermi2[8];
FILE *fpdat;
struct {
 char typ;
 int sz,offset;
} fil[150];
main (argc, argv)
     int argc;
     char *argv[];
{
  struct vsam v;
  struct apuns *ap;
  int fp,szb,posic;
  char *buf, *dat, k,b1[2048],*ptr,datfil[40];
  long rec = 0l, lug;
  int size, i;
  fixpos=0;
  if (argc < 3)
    {
      fprintf (stdout, "Program to create a .dat file from a vsam file.\n");
      fprintf (stdout, "Usage:\n");
      fprintf (stdout, "parcar vsamfile vsamfile.i\n");
      fprintf (stdout, "File 'vsamfile.dat' will be created\n");
      exit (0);
    }
       memset(stTermi,0,8);
       memset(stTermi2,0,8);
       ++ptr;
       stTermi[0] = '\'';
       stTermi2[0] = '\'';
       stTermi2[1] = ',';
  for(i=0;i<argc; i++) {
   if(*(argv[i]) == '-') {
     ptr = argv[i];
     ++ptr;
     switch(*ptr) {
      case 'q': /* fechas qnx2 */
       qnx2 = 1;
       break;
      case 'd': /* delim. character */
       memset(stTermi,0,8);
       memset(stTermi2,0,8);
       ++ptr;
       stTermi[0] = *ptr;
       stTermi2[0] = *ptr;
       stTermi2[1] = ',';
       break;
      case 'p': fixpos = 1; break;
     }
   }
  }
  i=0;
  if ((fp = open (argv[1], O_RDONLY)) == 0)
    {
      fprintf (stderr, "Can't open %s\n", argv[1]);
      exit (0);
    }

  if ((fpdat = fopen(argv[2],"r")) == NULL) {
   fprintf(stderr,"File %s can't be opened\n",argv[2]);
   exit(0);
  }

  fread(b1,2048,sizeof(char),fpdat);
  fclose(fpdat);
  // printf("%s\n",b1);
  ptr = b1;
  i=0; posic = 0;
  while(*ptr != ';') {
   if(*ptr == 's' || *ptr == 'n' ) { fil[i].typ = *ptr; ++ptr; fil[i].sz = atoi(ptr); }
   else fil[i].typ = *ptr;
   switch( fil[i].typ ) {
    case 'f': fil[i].sz = sizeof(float); break;
    case 'd': fil[i].sz = sizeof(double); break;
    case 'i': fil[i].sz = sizeof(short); break;
    case 'l': case 'D': case 'T': fil[i].sz = sizeof(long); break;
   }
   fil[i].offset = posic;
   posic += fil[i].sz;
//   printf("Field %d typ %c size %d offset %d\n",i,fil[i].typ,fil[i].sz,fil[i].offset);
   ++i;
   while(*ptr++ != ',') ;
  }
  tofil = i; tofilm1 = tofil -1;

  sprintf(datfil,"%s.dat",argv[1]);
  fpdat = fopen(datfil,"w");

  read (fp, &v, VSAM);
  size = v.vrsz[0] + PRNREC + APUNS * (int) v.vtk;
  buf = (char *) calloc (size, 1);
  dat = buf + PRNREC + APUNS * v.vtk;
  ap = (struct apuns*)(buf + PRNREC);
  for (i = 0; i < v.vtk; i++)
    {
      if (v.strk[i].rep == 0)
	k = i;
    }
  lug = v.vpk;
  while (lug != 0l)
    {
      lseek (fp, lug, SEEK_SET);
      read (fp, buf, size);
      if(!fixpos) gendat (dat);
      else gendat2(dat);
      rec++;
      lug = ap[k].proxi;
    }
  /*printf("lug %ld vrsz %d vtk %d\n",lug,v.vrsz[0],v.vtk); */
  fprintf (stdout, "Total Number of records: %ld (%s)\n", rec, argv[1]);
  close (fp);
}

void gendat(char *dat)
{
  int i,j;
  char s[4096],s2[512],*dato,*fins;
  struct tm *tim, *localtime ();

  memset (s, 0, sizeof(s));

  for ( i=0; i < tofil; i++) {

   dato = dat + fil[i].offset;
   memset (s2, 0, sizeof(s2));

   switch( fil[i].typ ) {
    case 's':
     fins = dato + (fil[i].sz - 1);
     j = fil[i].sz;
     while ( *fins == ' ') { --j; --fins; if(j==0) break; }
     strncpy(s2,dato,j);
     break;
    case 'f':
     if(*(float*)dato > MAX1 || *(float*)dato < MIN1) *(float*)dato=0.0;
     sprintf(s2,"%-.2f",*(float*)dato); break;
    case 'd':
     if(*(double*)dato > MAX1 || *(double*)dato < MIN1) *(double*)dato=0.0;
     sprintf(s2,"%-.2f",*(double*)dato); break;
    case 'i':
     sprintf(s2,"%-d",*(short*)dato); break;
    case 'l':
     sprintf(s2,"%-ld",*(long*)dato); break;
    case 'T':
     *(long*)dato += 86400;
     if(qnx2) *(long*)dato += MAS10;
     tim = localtime ((long *) dato);
     sprintf(s2,"%4d-%02d-%02d %02d:%02d:%02d",
       tim->tm_year+1900,
       tim->tm_mon + 1,
       tim->tm_mday,
       tim->tm_hour,tim->tm_min,tim->tm_sec);
     break;
    case 'D':
     *(long*)dato += 86400;
     if(qnx2) *(long*)dato += MAS10;
     tim = localtime ((long *) dato);
     sprintf(s2,"%4d-%02d-%02d",
       tim->tm_year+1900,
       tim->tm_mon + 1,
       tim->tm_mday);
     break;
   }

   if(fil[i].typ != 'n') {
/*    strcat(s,"\t");*/
    strcat(s,s2);
    if(i != tofilm1) strcat(s,"\t");
    else  {  /*strcat(s,"\t");*/ strcat(s,"\n"); }
   }

   if(fil[i].typ == 'n' && i == tofilm1)
    strcat(s,"\n");

  }

  fprintf(fpdat,s);

}

void gendat2(char *dat)
{
  int i,j,crec;
  char s[4096],s2[512],*dato,*fins;
  struct tm *tim, *localtime ();

  memset (s, 0, sizeof(s));
  j=0;
  for ( i=0; i < tofil; i++) {

   dato = dat + fil[i].offset;
   memset (s2, 0, sizeof(s2));

   switch( fil[i].typ ) {
    case 's':
     memcpy(s2,dato,fil[i].sz);
     crec = fil[i].sz;
     break;
    case 'f':
     if(*(float*)dato > MAX1 || *(float*)dato < MIN1) *(float*)dato=0.0;
     sprintf(s2,"%-13.2f",*(float*)dato); crec = 13; break;
    case 'd':
     if(*(double*)dato > MAX1 || *(double*)dato < MIN1) *(double*)dato=0.0;
     sprintf(s2,"%-16.2f",*(double*)dato); crec = 16; break;
    case 'i':
     sprintf(s2,"%-6d",*(short*)dato); crec = 6; break;
    case 'l':
     sprintf(s2,"%-11ld",*(long*)dato); crec = 11; break;
    case 'T':
     if(qnx2) *(long*)dato += MAS10;
     tim = localtime ((long *) dato);
     sprintf(s2,"%4d-%02d-%02d %02d:%02d:%02d",
       tim->tm_year+1900,
       tim->tm_mon + 1,
       tim->tm_mday,
       tim->tm_hour,tim->tm_min,tim->tm_sec); crec = 19;
     break;
    case 'D':
     if(qnx2) *(long*)dato += MAS10;
     tim = localtime ((long *) dato);
     sprintf(s2,"%4d-%02d-%02d",
       tim->tm_year+1900,
       tim->tm_mon + 1,
       tim->tm_mday); crec = 10;
     break;
   }

   if(fil[i].typ != 'n') {
    memcpy(s+j,s2,crec);
    j += crec;
   }

  }

  fwrite(s,j,sizeof(char),fpdat);

}

