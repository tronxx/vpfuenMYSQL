/*
  the VPG - CGI connection
  by Franz J Fortuny
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

  He,sVar,1-2-3-4-5-6-..,; The contents of env. variable 1,2,3 etc., will
             be copied to 'sVar'. The values must be defined using instr.
             #d..; (vpg).

  Hp,0 or 1,fn,; With 0, the field's title is printed. With 1, the field
             contents are printed. "fn" is the field number (constant or VPG
             iVariable).

  HP,"NameofField",; The CONTENTS of the field referenced by its name will
             be printed to the active output channel.

  HmH,"Title",;  Make header it will print all that is required for headers
  
  Hmb,"Title in big",; Start Body print a title.
  HmB,; Print end of Body.
 
  Hmf,"action","Method",; Start form giving action and Method

  Hmh,"name","value",; create a hidden input with a fixed value HIDDEN
  Hme,"prompt","name","value",0-2,size,; EDIT BOX
  Hmr,"prompt","name","value","Option","Value","option",...,; RADIO BUttons
  Hmt,"Prompt","name",rows,cols,; TEXT AREA
  Hms,"Prompt","name",0-1,"Option"-"Filename","option",option",...,; SELECT
*/

#include <stdio.h>
#define MP while(*ptr++ != ',')
void *getad(char *);
long totsec(char *ptr);

/* To read data from the browser */

char *buf,*sbuf;
int bufsz = 0,hnf =0;
typedef struct hinf {
    char *fname;
    char *fcont;
    } HINF;
HINF **vhinf;

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
   vhinf = (HINF*)calloc(HINF,hnf+1);
   memset(vhinf,0,sizeof(HINF)*(hnf+1));
   memset(sbuf,0,bufsiz+1);
   f = sbuf; p = sbuf; pm = buf; s[2]=0;
   for(i=0;i<bufsiz-1;i++) {
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
      *p = htoi(s); ++p; ++i; ++i;
     break;
     defautl:
      *p++ = *pm++;
     break;
    }
   }
   vhinf[infi].fcont = f;
   free(buf);
}

  /* copy from field to vpg variable */
void vHcopy(char *ptr)
{
 char *buf,typ,*st;
 int max,fn;
 if(*ptr == 'c') {
  MP; typ = *ptr; buf = getad(ptr);
  MP; max = atoi(ptr);
  MP; fn = valu(ptr);
 }
 else {
  MP; typ = *ptr; buf = getad(ptr);
  MP; max = atoi(ptr);
  MP; st = (char*)getad(ptr);
  for(fn=0;fn<hnf;fn++) {
   if(strcmp(st,vhinf[fn].fname) == 0) break;
  }
 }
 switch(typ) {
  case 's': strncpy(buf,vhinf[fn].fcont,max); return;
  case 'd': *(double*)buf = atof(vhinf[fn].fcont; return;
  case 'f': *(float*)buf = atof(vhinf[fn].fcont; return;
  case 'i': *(int*)buf = atoi(vhinf[fn].fcont; return;
  case 'l': *(long*)buf = atol(vhinf[fn].fcont; return;
  case 'D': *(long*)buf = totsec(vhinf[fn].fcont); return;
 }
}

void vHprint(char *ptr) /* print Hp,0-1,nf,;
{
  int fn; char op;
  MP; op = *ptr;
  MP; fn = valu(ptr);
  if(op == '0') printf("%s",vhinf[fn].fname);
  else printf("%s",vhinf[fn].fcont);
}

void vHprintn(char *ptr)
{
  char *st;
  int fn;
  MP; st = (char*)getad(ptr);
  for(fn=0;fn<hnf;fn++) {
   if(strcmp(st,vhinf[fn].fname) == 0) break;
  }
  printf("%s",vhinf[fn].fcont);
}

/* HmH,"Title",;  Make the HEADER of the page */
void vHHeader(char *ptr)
{
  char *p; MP; p = getad(ptr);
  printf("Content-Type: text/html\n");
  printf("<HEAD><TITLE>%s</TITLE></HEAD>\n",p);
}

/*  Hmb,"Title in big",; Start Body print a title. */
void vHBody(char *ptr)
{
  char *p; MP; p = getad(ptr);
  printf("<BODY>\n");
  printf("<H1>%s</H1>\n");
}

/*  HmB,; Print end of Body.*/
void vHEndBody
{
  printf("</BODY>\n");
}

/*   Hmf,"action","Method",; Start form givig action and method*/
void vHForm(char *ptr)
{
  char *p1,*p2;
  MP; p1 = getad(ptr); MP; p2 = getad(ptr);
  printf("<FORM action=\"%s\" method=\"%s\">\n",p1,p2);
}

void vHEndForm()
{
  printf("</FORM>\n");
}

/* Hmh,"name","value",; create a hidden input with a fixed value */
void vHhidden(char *ptr)
{
 char *p1,*p2;
 MP; p1 = getad(ptr); MP; p2 = getad(ptr);
 printf("<INPUT type=\"hidden\" name=\"%s\" value=\"%s\">\n",p1,p2);
}

/*  Hme,"prompt","name","value",0-1,size,; */
void vHedit(char *ptr)
{
  char *p1,*p2,*p3,typ,*ms;
  int sz;
  MP; p1 = getad(ptr);
  MP; p2 = getad(ptr);
  MP; p3 = getad(ptr);
  MP; typ = *ptr;
  MP; sz = valu(ptr);
  if(typ == '1') ms = "maxsize";
  else ms = "size";
  printf("%s <INPUT  type=\"%s\" %s=\"%d\" name=\"%s\">\n",
          p1,p2,ms,sz,p3);
}
/* Hmr,"prompt","name","value","Option","Value","option",...,; RADIO BUttons*/
void vHRButton(char *ptr)
{
  char *p1,*p2,*p3,*p4;
  MP; p1 = getad(ptr);
  MP; p2 = getad(ptr);
  MP;
  printf("%s\n",p1);
  while(*ptr != ';') {
   p3 = getad(ptr); MP; p4 = getad(ptr); 
   printf("<INPUT type=\"radio\" name=\"%s\" value=\"%s\">%s\n",
           p2,p3,p4);
   MP;
  }
}

/* Hmt,"Prompt","name",rows,cols,; TEXT AREA */
void vHText(char *ptr)
{
  char *p1,*p2;
  int rows,cols;
  MP; p1 = getad(ptr);
  MP; p2 = getad(ptr);
  MP; rows = valu(ptr);
  MP; cols = valu(ptr);
  printf("%s <TEXTAREA name=\"%s\" rows=\"%d\" cols=\"%d\"></TEXTAREA>\n",
          p1,p2,rows,cols);
}

/* Hms,"Prompt","name",0-1,"Option"-"Filename","option",option",...,; SELECT*/
void vHSelect(char *ptr)
{
  char *p1,*p2,typ,*nfile,*opt,s[100];
  FILE *fp;
  MP; p1 = getad(ptr);
  MP; p2 = getad(ptr);
  MP; typ = *ptr;
  MP;
  printf("%s <SELECT NAME=\"%s\">\n",p1,p2);
  if(typ == '1') nfile = getad(ptr);
  else {
   while(*ptr != ';') {
    opt = getad(ptr);
    printf("<OPTION VALUE=\"%s\">%s\n",opt);
    MP;
   }
   printf("</SELECT>\n");
   return;
  }
  if((fp=fopen(nfile,"rt")) == NULL) errh("Can't open options file %s\n",nfile);
  while(fgets(s,100,fp) != NULL) {
    s[strlen(s)-1] = 0;
    printf("<OPTION VALUE=\"%s\">%s",s,s);
  }
  fclose(fp);
  printf("</SELECT>\n");
}
