/* THE VPG PROJECT by Franz J Fortuny
   file :  vform.c
   to include instructions that will allow the VPG programmer the use
   of the forms library
   to bin linked:
    -lform -lncurses
*/
#include <stdio.h>
#include <stdlib.h>
#define MP while(*ptr++ != ',')
#define MPZ while(*ptr++ != 0); ++ptr
#include <curses.h>
#include <form.h>
#include <time.h>
#include <string.h>

#define MAXF 300

#define Autoskip 1
#define UpCase 2
#define LoCase 3
#define Passw 4
#define NoEdit 5
#define NoActive 6
#define NoNull 7
#define Alfa 8
#define Alfamax 9
#define Alnum 10
#define Alnumax 11
#define NoStatic 12
#define Choice 13

extern int vwa, vrow, vcol,usf,caps;	/* active window */
extern WINDOW *win[25];		/* array of pointers to windows */
extern char dy[];
char *getad(char *);
long totsec(char *);
void err(char *, ...);

int vform(char *prg)
{
  struct tm *tim;
  FIELD *vfl[MAXF];
  FORM *vfrm;
  int yy,xx,y,x,asiz[MAXF],atyp[MAXF],inser,yr,jj,retu=0,mayus,xxs;
  int i=0,j,c,ii,opcio[20],opt,ij;
  char typ,*buf,*ptr,*fmt,s[256],ss[100],*abuf[100],*cho[100];
  ptr = prg;

  //  qf,y,x,sVar,#,
  //         nVar,"%10.2f",
  //         "Label",0,...,;

  /* the fields are formed */
  MP;
  for(i=0;i<MAXF;i++) {
  opt = 0;
  xxs = 0;
start1:

  if(*ptr == ';') break;

  y = valu(ptr);

  if( y >= 900) {
   if(opt >= 20) err("Too many options: max. 20 per field\n");
   opcio[opt] = y - 900;
   if(opcio[opt] == NoStatic) {
    MP; xxs = atoi(ptr);
   }
   MP;
   if(opcio[opt] == Choice) {
    ij=0;
    while(!isdigit(*(ptr))) {
     cho[ij] = getad(ptr);
     ij++;
     MP;
    }
    cho[ij] = NULL;
   }
   opt++;
   goto start1;
  }

  MP;
  x = valu(ptr); MP;

  typ = *ptr;
  buf = getad(ptr);

  if(typ == '"'){ MPZ; }
  else MP;

  yy = 1;

  if(typ == 's' || typ == '"') { xx = atoi(ptr); MP; }
  else {
   fmt = ptr+1;
   while(!isdigit(*ptr)) ++ptr;
   xx = atoi(ptr);
   MPZ;
  }

  if(typ == 's') --xx;

  if(xx == 0) xx = strlen(buf);
  if(xxs == 0) xxs = xx;
  if((vfl[i] = new_field(yy,xxs,y,x,0,1)) == NULL)
    err("Field No. %d is returning NULL.\n",i);
  if(typ == 's' || typ == '"') set_field_buffer(vfl[i],0,buf);

  if(typ != '"') {
   set_field_back(vfl[i],A_REVERSE);
   switch(typ) {
    case 's': field_opts_off(vfl[i],O_BLANK|O_AUTOSKIP);
     break;
    case 'i':
    case 'l':
     field_opts_off(vfl[i],O_AUTOSKIP);
     set_field_type(vfl[i],TYPE_INTEGER,0,0,0);
     set_field_just(vfl[i],JUSTIFY_RIGHT);
    break;
    case 'f': case 'd':
     field_opts_off(vfl[i],O_AUTOSKIP);
   /*  set_field_type(vfl[i],TYPE_NUMERIC,0,0,0);*/
     set_field_just(vfl[i],JUSTIFY_RIGHT);
    break;
    case 'D':
     field_opts_off(vfl[i],O_AUTOSKIP);
    break;
   }
   if(opt > 0) {
    for(ii=0;ii<opt;ii++) {
     switch(opcio[ii]) {
      case Choice: set_field_type(vfl[i],TYPE_ENUM,cho,0,0); break;
      case NoStatic: field_opts_off(vfl[i],O_STATIC);
                     set_max_field(vfl[i],xx); break;
      case Autoskip: field_opts_on(vfl[i],O_AUTOSKIP); break;
      case UpCase: mayus = 1; break;
      case LoCase: mayus = 0; break;
      case Passw: field_opts_off(vfl[i],O_PUBLIC); break;
      case NoEdit: field_opts_off(vfl[i],O_EDIT); break;
      case NoActive: field_opts_off(vfl[i],O_ACTIVE); break;
      case NoNull: field_opts_off(vfl[i],O_NULLOK); break;
      case Alfa: set_field_type(vfl[i],TYPE_ALPHA,0); break;
      case Alfamax: set_field_type(vfl[i],TYPE_ALPHA,xx);
       field_opts_on(vfl[i],O_AUTOSKIP);
      break;
      case Alnum: set_field_type(vfl[i],TYPE_ALNUM,0); break;
      case Alnumax: set_field_type(vfl[i],TYPE_ALNUM,xx);
       field_opts_on(vfl[i],O_AUTOSKIP);
      break;
     }
    }
   }
  }

  /* This will set inactive the labels */
  if(typ == '"') {
   field_opts_off(vfl[i],O_ACTIVE);
  	set_field_just(vfl[i],JUSTIFY_RIGHT);
  }

  switch(typ) {
   case '"':
   case 's': break;
   case 'd': sprintf(s,fmt,*(double*)buf); break;
   case 'f': sprintf(s,fmt,*(float*)buf); break;
   case 'l': sprintf(s,fmt,*(long*)buf); break;
   case 'i': sprintf(s,fmt,*(short*)buf); break;
   case 'H':
   case 'D':
      if(*(long*)buf == 0l) {
       time((long*)buf);
      }
      tim = localtime ((long *) buf);
   	yr = tim->tm_year + 1900;
      if (typ == 'D')
	     sprintf (s, "%02d/%02d/%04d",
		  (usf) ? tim->tm_mon + 1 : tim->tm_mday,
		  (usf) ? tim->tm_mday : tim->tm_mon + 1,yr);
      else	sprintf (s, "%02d:%02d:%02d",
	     tim->tm_hour, tim->tm_min, tim->tm_sec);
   break;
   default: err("Bad data type in field forming.\n"); break;
  }

  if(typ=='D'||typ=='d'||typ=='f'||typ=='l'||typ=='i') set_field_buffer(vfl[i],0,s);
 

  atyp[i] = typ;
  asiz[i] = xx;
  abuf[i] = buf;

}
 vfl[i]=NULL;

  /* the connection is made between fields and a form */
  if((vfrm = new_form(vfl)) == NULL)
  	err("Form is returning NULL.\n");

   // Associate form to window

  set_form_win(vfrm,win[vwa]);
  post_form(vfrm);
  wrefresh(win[vwa]);

// Loop to accept data
   form_driver(vfrm,REQ_OVL_MODE); inser = 0;
  while((c = wgetch (win[vwa])) != 13) {
   if(c == 27) {retu = -1; goto finfor;}
   switch(c) {
    case 9: case KEY_DOWN: c = REQ_NEXT_FIELD; break;
    case KEY_BTAB: case KEY_UP: c = REQ_PREV_FIELD; break;
    case KEY_HOME: c = REQ_BEG_FIELD; break;
    case KEY_END: c = REQ_END_FIELD; break;
    case KEY_LEFT: c = REQ_LEFT_CHAR; break;
    case KEY_RIGHT: c = REQ_RIGHT_CHAR; break;
    case KEY_IC: c = REQ_INS_CHAR; break;
    case KEY_DC: c = REQ_DEL_CHAR; break;
    case KEY_BACKSPACE: c = REQ_DEL_PREV; break;
    case KEY_F(11): if(inser == 1) inser=0; else inser = 1;
      if(inser == 1) c = REQ_INS_MODE;
      else c = REQ_OVL_MODE; break;
    case KEY_F(12): c = REQ_DEL_LINE; break;
    case KEY_NPAGE: c = REQ_LAST_FIELD; break;
    case KEY_PPAGE: c = REQ_FIRST_FIELD; break;
    case KEY_F(13): c = REQ_NEXT_CHOICE; break;
    case KEY_F(14): c = REQ_PREV_CHOICE; break;
    default: if(caps) c = toupper(c); break;
   }
   form_driver(vfrm,c);
  }

  form_driver(vfrm,REQ_NEXT_FIELD);

  for(j=0;j<i;j++) {
   switch(atyp[j]) {
    case 's':
     strncpy(abuf[j],field_buffer(vfl[j],0),asiz[j]);
     for(jj=asiz[j]-1;jj>=0;jj--) {
      if(abuf[j][jj] == ' ') abuf[j][jj] = 0;
      else if(abuf[j][jj] != 0) break;
     }
    break;
    case 'd': *(double*)abuf[j] = atof(field_buffer(vfl[j],0)); break;
    case 'l': *(long*)abuf[j] = atol(field_buffer(vfl[j],0)); break;
    case 'i': *(int*)abuf[j] = atoi(field_buffer(vfl[j],0)); break;
    case 'f': *(float*)abuf[j] = atof(field_buffer(vfl[j],0)); break;
    case 'D': *(long*)abuf[j] = totsec(field_buffer(vfl[j],0)); break;
   }
  }

finfor:
  unpost_form(vfrm);
  /* free the form */
  free_form(vfrm);

  /* free the fields */
  for(j=0;j!=i;j++) free_field(vfl[i]);
  wrefresh(win[vwa]);

  return retu;

}
