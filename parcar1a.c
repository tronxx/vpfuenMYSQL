#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#define MAS10 315532800

void gendat(char *dat);

int mues = 1,tofil,tofilm1,qnx2;
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


  int fp,szb,posic;
  char *dat, k,b1[2048],*ptr,datfil[40];
  long rec = 0l, lug;
  int size, i;
  if (argc < 3)
    {
      fprintf (stdout, "Program to create a .dat file from a random file.\n");
      fprintf (stdout, "Usage:\n");
      fprintf (stdout, "parcar ramdfile ramdfile.i\n");
      fprintf (stdout, "File 'ramdfile.dat' will be created\n");
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
     }
   }
  }
  i=0;
  if ((fp = open (argv[1], O_RDONLY)) == 0)
    {
      fprintf (stderr, "Can't open %s\n", argv[1]);
      exit (0);
    }

  if ((fpdat = fopen(argv[2],"rt")) == NULL) {
   fprintf(stderr,"File %s can't be opened\n");
   exit(0);
  }

  fread(b1,500,sizeof(char),fpdat);
  fclose(fpdat);
  printf("%s\n",b1);
  ptr = b1;
  i=0; posic = 0;
  while(*ptr != ';') {
   if(*ptr == 's' || *ptr == 'n' ) { fil[i].typ = *ptr; ++ptr; fil[i].sz = atoi(ptr); }
   else fil[i].typ = *ptr;
   switch( fil[i].typ ) {
    case 'f': fil[i].sz = sizeof(float); break;
    case 'd': fil[i].sz = sizeof(double); break;
    case 'i': fil[i].sz = sizeof(short); break;
    case 'l': case 'D': fil[i].sz = sizeof(long); break;
   }
   fil[i].offset = posic;
   posic += fil[i].sz;
   printf("Field %d typ %c size %d offset %d\n",i,fil[i].typ,fil[i].sz,fil[i].offset);
   ++i;
   while(*ptr++ != ',') ;
  }
  tofil = i; tofilm1 = tofil -1;

  sprintf(datfil,"%s.dat",argv[1]);
  fpdat = fopen(datfil,"w");
  size = 0;
  for(i=0; i<tofil; i++) size += fil[i].sz;

  dat = (char *) calloc (size, 1);

  lug = (long)size;
  lseek (fp, lug, 0);
  while (read (fp, dat, size) != 0)
    {
      gendat (dat);
      rec++;
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
     sprintf(s2,"%-10.2f",*(float*)dato); break;
    case 'd':
     sprintf(s2,"%-10.2f",*(double*)dato); break;
    case 'i':
     sprintf(s2,"%-d",*(short*)dato); break;
    case 'l':
     sprintf(s2,"%-ld",*(long*)dato); break;
    case 'T':
     if(qnx2) *(long*)dato += MAS10;
     tim = localtime ((long *) dato);
     sprintf(s2,"%4d-%02d-%02d %02d:%02d:%02d",
       tim->tm_year+1900,
       tim->tm_mon + 1,
       tim->tm_mday,
       tim->tm_hour,tim->tm_sec,tim->tm_sec);
     break;
    case 'D':
     if(qnx2) *(long*)dato += MAS10;
     tim = localtime ((long *) dato);
     sprintf(s2,"%4d-%02d-%02d",
       tim->tm_year+1900,
       tim->tm_mon + 1,
       tim->tm_mday);
     break;
   }

   if(fil[i].typ != 'n') {
    strcat(s,stTermi);
    strcat(s,s2);
    if(i != tofilm1) strcat(s,stTermi2);
    else  { strcat(s,stTermi); strcat(s,"\n"); }
   }

   if(fil[i].typ == 'n' && i == tofilm1)
    strcat(s,"\n");

  }

  fprintf(fpdat,s);

}

