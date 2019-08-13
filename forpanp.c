/* Programa para formar archivos legibles con las insrucciones tP...; y
  tL....; del vpg4
  Versión para AS/400
  */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
  void err(char *, ...);
  void ext(char *);
  void extout(char *);
  void extinp(char *);
  int row,col,pos;
  FILE *scfp,*dsfp;
void main(int argc, char *argv[])
{
  FILE *fp;
  char s[90],*ptr,ars[90];

  row=0; col=0; pos=0;

  if(argc < 4) err("Faltan argumentos: call forpan fuente posis panta");
  sprintf(ars,"%s",argv[1]);
  if((fp=fopen(ars,"r")) == NULL)
   err("El archivo %s no se puede abrir",ars);
  sprintf(ars,"%s",argv[2]);
  if((scfp=fopen(ars,"w")) == NULL)
   err("No se puede abrir %s",ars);
  sprintf(ars,"%s",argv[3]);
  if((dsfp=fopen(ars,"w")) == NULL)
   err("No se puede abrir %s",ars);
  while(fgets(s,86,fp) != NULL) {
   if(strlen(s) == 1) { ++row; continue; }
   s[strlen(s)-1]=0;
   col=0; ptr = s;
   ext(ptr); ++row;
  }
}

void ext(char *ptr)
{
 char s[90]; int i=0,kcol=0;
 do {
  while(isspace(*ptr)) {++ptr; ++col;}
  if(*ptr == '_') { kcol = col;
   while(*ptr != ' ') { s[i++]= *ptr++; ++col; if(*ptr == 0) break; }
   s[i]=0;
   fprintf(scfp,"%02d %02d %3d %s\n",row,kcol,pos++,s);
   i=0; memset(s,0,90);
  }
  else { kcol= col;
   while(*ptr != '_') { s[i++]= *ptr++; ++col; if(*ptr == 0) break; }
   s[i]=0;
   fprintf(dsfp,"%02d %02d %s\n",row,kcol,s);
   i=0; memset(s,0,90);
  }
 } while(*ptr != 0);
}

#include "err2.c"
