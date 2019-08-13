/*
  the VPG - CGI Connection
  By Franz J Fortuny
  routines to allow VPG programmers to interact with the HTTP Internet
  protocol
  February 15, 1998

  Implemented instructions:

  Hi,iVar,;  The data from the browser will be read. The method will be
             evaluated: if GET, data will be taken from the ENV_Variable
             if POST, then the size will be read, a buffer will be allocated
             to contain the data, the data will be read, the buffer will
             be parsed and a structure with the data will be created. iVar
             will contain the Number of Fields captured.

  Hc,vVar,[#],fn,; The data from field # fn will be copied to vpg variable
             'vVar'. The variable will actually be given the correct value
             depending on its type.

  HC,vVar,[#],"Name",; Same as 'Hc' but instead of the Field #, the Field
             name is used.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MP while(*ptr++ != ',')

void *getad(char *);
long totsec(char *);

/* To read data from the browser 
  Hi,iVar,;  The data from the browser will be read. The method will be
             evaluated: if GET, data will be taken from the ENV_Variable
             if POST, then the size will be read, a buffer will be allocated
             to contain the data, the data will be read, the buffer will
             be parsed and a structure with the data will be created. iVar
             will contain the Number of Fields captured.
*/             

static char *buf;
static char *sbuf;
static int bufsiz = 0,hnf =0;
typedef struct {
    char *fname;
    char *fcont;
    } HINF;
static HINF *vhinf;
extern FILE *fout;
int vHinput(char *ptr)
{
  char *p,*pm,*f,s[8];
  int i,infi=0;

   if(strcmp("POST",getenv("REQUEST_METHOD")) == 0) {
    bufsiz = atoi(getenv("CONTENT_LENGTH")) + 1;
     if(bufsiz == 1) {
      return(-1);
     }
    buf = (char*)calloc(1,bufsiz);
    fread(buf,1,bufsiz,stdin);
   }

   else {
    if(getenv("QUERY_STRING") == NULL) return - 1;
    bufsiz = strlen(getenv("QUERY_STRING")) + 1;
    if(bufsiz == 1) {
      return(-1);
    }
    buf = (char*)calloc(1,bufsiz);
    strcpy(buf,getenv("QUERY_STRING"));
   }

   sbuf = (char*)calloc(1,bufsiz);
   p = buf;

   for(i=0;i<bufsiz;i++) if(*p++ == '=') hnf++;

   MP; *(int*)getad(ptr) = hnf;

   vhinf = (HINF*)calloc(sizeof(HINF),hnf+1);
   memset(vhinf,0,sizeof(HINF)*(hnf+1));
   memset(sbuf,0,bufsiz+1);

   f = sbuf; p = sbuf; pm = buf; s[2]=0;

   for(i = 0; i < bufsiz - 1;  i++) {

    switch(*pm) {

     case '=':
      *p++ = 0;
      vhinf[infi].fname = f;
      f = p; ++pm;
     break;

     case '&':
      *p++ = 0;
      vhinf[infi++].fcont = f;
      f = p; ++pm;
     break;

     case '%':
      ++pm; s[0] = *pm; ++pm; s[1] = *pm; ++pm;
      *p = (unsigned char) strtoul(s,NULL,16);  ++p; ++i; ++i;
     break;

     case '+':
      *p++ = ' '; pm++;
     break;

     default:
      *p++ = *pm++;
     break;

    }

   }

   vhinf[infi].fcont = f;

   free(buf);
}


void vHopts(char *ptr)
{
 char *buf,typ,*st,*p1,*p2,*p3;
 int max,fn,cols,rows;
   switch(*ptr) {

/*  Hc,vVar,[#],fn,; The data from field # fn will be copied to vpg variable
             'vVar'. The variable will actually be given the correct value
             depending on its type.

    HC,vVar,[#],"Name",; Same as 'Hc' but instead of the Field #, the Field
             name is used.
*/
     case 'c':
       MP; typ = *ptr; buf = getad(ptr);
       MP; max = atoi(ptr);
       MP; fn = valu(ptr);
      switch(typ) {
       case 's': strncpy(buf,vhinf[fn].fcont,max); return;
       case 'd': *(double*)buf = atof(vhinf[fn].fcont); return;
       case 'f': *(float*)buf = atof(vhinf[fn].fcont); return;
       case 'i': *(int*)buf = atoi(vhinf[fn].fcont); return;
       case 'l': *(long*)buf = atol(vhinf[fn].fcont); return;
       case 'D': *(long*)buf = totsec(vhinf[fn].fcont); return;
      }
     break;
     case 'C':
       MP; typ = *ptr; buf = getad(ptr);
       MP; max = atoi(ptr);
       MP; st = (char*)getad(ptr);
       for(fn=0;fn<hnf;fn++) {
        if(strcmp(st,vhinf[fn].fname) == 0) break;
       }
      switch(typ) {
       case 's': strncpy(buf,vhinf[fn].fcont,max); return;
       case 'd': *(double*)buf = atof(vhinf[fn].fcont); return;
       case 'f': *(float*)buf = atof(vhinf[fn].fcont); return;
       case 'i': *(int*)buf = atoi(vhinf[fn].fcont); return;
       case 'l': *(long*)buf = atol(vhinf[fn].fcont); return;
       case 'D': *(long*)buf = totsec(vhinf[fn].fcont); return;
      }
     break;

/*HP,"Title",; */
     case 'P': /* print the field according to title */
       MP; st = (char*)getad(ptr);
       for(fn=0;fn<hnf;fn++) {
        if(strcmp(st,vhinf[fn].fname) == 0) break;
       }
      fprintf(fout,"%s\n",vhinf[fn].fcont); 
     return;

/*Hp,0,lVal,; Hp,1,lVal,; 0 = titulo  1 = dato*/
     case 'p':
      MP; typ = *ptr; MP; fn = valu(ptr);
      if(typ == '0') fprintf(fout,"%s",vhinf[fn].fname);
      else fprintf(fout,"%s",vhinf[fn].fcont);
     return;

     case 'k': /* copy TITLE: Hk,n,sBuf,#,; */
      MP; fn = valu(ptr);
      MP; buf = getad(ptr);
      MP; max = valu(ptr);
      strncpy(buf,vhinf[fn].fname,max);
     return;

#define PP fprintf(fout,

     case 'f': /*HTML format command */
      ++ptr;
      switch(*ptr) {
       case 'p': PP"<p>"); return;
       case 'r': PP"<br>"); return;
       case '1': PP"<H1>"); return; case '6': PP"</H1>"); return;
       case '2': PP"<H2>"); return; case '7': PP"</H2>"); return;
       case '3': PP"<H3>"); return; case '8': PP"</H3>"); return;
       case 'e': PP"<pre>"); return; case 'E': PP"</pre>"); return;
       case 'h': PP"<hr>"); return;
       case 'b': PP"<b>"); return; case 'B': PP"</b>"); return;
       case 'i': PP"<i>"); return; case 'I': PP"</i>"); return;
       case 'u': PP"<u>"); return; case 'U': PP"</u>"); return;
      }
     break;

     case 'F': /* HTML FORM creation */
     ++ptr;
      switch(*ptr) {
       case '1': /* form begins */
        MP; p1 = getad(ptr); MP; p2 = getad(ptr);
        PP"<FORM action=\"%s\" method=\"%s\">",p1,p2); return;
       case '0': /* form ends */ PP"</FORM>"); return;
      }
     break;

     case 'I': /* Input fields or TAGS */
     ++ptr;
      switch(*ptr) {
       case 't':
        MP; max = valu(ptr); MP; typ = *ptr; MP; p2 = getad(ptr);
        if(typ == '1') {
         PP"<INPUT type=\"text\" maxlength=\"%d\" name=\"%s\">",
         max,p2);
        }
        else {
         PP"<INPUT type=\"text\" size=\"%d\" name=\"%s\">",
         max,p2);
        }
        return;
       case 'p': /*password */
        MP; max = valu(ptr); MP; typ = *ptr; MP; p2 = getad(ptr);
        if(typ == '1') {
         PP"<INPUT type=\"password\" maxlength=\"%d\" name=\"%s\">",
         max,p2);
        }
        else {
         PP"<INPUT type=\"password\" size=\"%d\" name=\"%s\">",
         max,p2);
        }
       return;
       case 'r': /*radio buttons */
        MP; p1=getad(ptr); MP;
        while(*ptr != ';') {
         p2 = getad(ptr);
         PP"<INPUT type=\"radio\" name=\"%s\" value=\"%s\">%s",
             p1,p2,p2);
         MP;
        }
       return;
       case 'c': /*Checkboxl */
       MP; p1=getad(ptr); MP;
       while(*ptr != ';') {
         p2 = getad(ptr);
         PP"<INPUT type=\"checkbox\" name=\"%s\" value=\"%s\">%s",
             p1,p2,p2);
         MP;
        }
       return;
       case 'b': /* buttons */
       MP; p1=getad(ptr);
         PP"<INPUT type=\"submit\" name=\"%s\" value=\"%s\">",p1,p1);
       return;
       case 'B': /* buttons */
       MP; p1=getad(ptr);
         PP"<INPUT type=\"reset\" name=\"%s\" value=\"%s\">",p1,p1);
       return;
      }
     break;

     case 'T':
        ++ptr;
        switch(*ptr) {
         case 'b': /* begin text area */
          MP; p1 = getad(ptr);
          MP; rows = valu(ptr); MP; cols = valu(ptr); MP;
          if(*ptr == '1') {
	   PP"<TEXTAREA name=\"%s\" rows=\"%d\" cols=\"%d\" wrap=\"physical\">",
	    p1,rows,cols);
          }
          else {
	   PP"<TEXTAREA name=\"%s\" rows=\"%d\" cols=\"%d\">",
	    p1,rows,cols);
       	  }
         return;
         case 'B': /* end textarea */
           PP"</TEXTAREA>");
         return;
        }
     break;

     case 's':
      ++ptr;
      switch(*ptr) {
      	case 'b':

      	 MP; typ = *ptr; MP; p1 = getad(ptr);

      	 if(typ == '1') {
      	  MP; typ = *ptr;
      	  PP"<SELECT MULTIPLE name=\"%s\">\n",p1);
      	  if(typ == '0') {
      	   MP;
      	   while(*ptr != ';') {
      	     p1 = getad(ptr);
      	     PP"<OPTION>%s\n",p1);
      	     MP;
      	   }
      	   return;
      	  }
      	   else {
      	   return;
      	   }
      	 }

      	 else {
      	  MP; typ = *ptr;
      	  PP"<SELECT name=\"%s\">\n",p1);
      	  if(typ == '0') {
      	   MP;
      	   while(*ptr != ';') {
      	     p1 = getad(ptr);
      	     PP"<OPTION>%s\n",p1);
      	     MP;
      	   }
      	   return;
      	  }
      	  else {
      	  return;
      	  }
      	 }
      	break;
      	case 'B':
      	 PP"</SELECT>");
      	break;
      }
     break;
   }

}

