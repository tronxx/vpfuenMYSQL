/* Funcion para imprimir un campo de acuerdo a ciertos parametros */
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>

char *coma (char *);

void 
putval (char s[], char typ, char *dir, char *forma);

extern unsigned attr;
extern char usf, vido, *dy, *mt;
extern int vwa;
extern WINDOW *win[];
void 
impf (FILE * tof, char tipo, char *dato, int forma, char *fmt)
{
  char s[256];
  int nc=0;
  
  if (tipo == 's')
    {
      if (forma == 0)
	nc = strlen (dato);
      else
	nc = forma;
      if (vido)
    waddnstr(win[vwa],dato,nc);
      else
	fwrite (dato, nc, 1, tof);
      return;
    }
  else
    putval (s, tipo, dato, fmt);
  if (vido)
    waddnstr(win[vwa],s,strlen(s));
  else
    fwrite (s, strlen (s), 1, tof);
}

void 
putval (char s[], char typ, char *dir, char *forma)
{
  struct tm *tim, *localtime ();
  char *c, fmt[64];
  int *i, yr;
  long *l;
  float *f;
  double *d;
  if (*(forma + 1) == ',')
    {
      strcpy (fmt, forma);
      *(fmt + 1) = ' ';
      switch (typ)
	{
	case 'i':
	  sprintf (s, fmt, *(short *) dir);
	  break;
	case 'e':
	  sprintf (s, fmt, *(int *) dir);
	  break;
	case 'l':
	  sprintf (s, fmt, *(long *) dir);
	  break;
	case 'd':
	  sprintf (s, fmt, *(double *) dir);
	  break;
	case 'f':
	  sprintf (s, fmt, *(float *) dir);
	  break;
	}
      strcpy (s, coma (s));
      return;
    }
  switch (typ)
    {
    case 'i':			/* campo de entero corto 16 bits */
      sprintf (s, forma, *(short *) dir);
      break;
    case 'e':			/* campo de entero normal 4 bytes en 32 bits */
      sprintf (s, forma, *(int *) dir);
      break;
    case 'l':			/* campo de long */
      sprintf (s, forma, *(long *) dir);
      break;
    case 'f':			/* campo de flotante precision sencilla */
      sprintf (s, forma, *(float *) dir);
      break;
    case 'd':			/* campo de flotante doble precision */
      sprintf (s, forma, *(double *) dir);
      break;
    case 'c':			/* campo de un caracter valor numerico */
      sprintf (s, forma, *(char *) dir);
      break;
    case 'D':			/* DATE field */
    case 'H':			/* day of the week and hour */
      tim = localtime ((long *) dir);
     /* if (tim->tm_year >= 100)
	yr = tim->tm_year - 100;
      else
	yr = tim->tm_year; */
	yr = tim->tm_year + 1900;
      if (typ == 'D')
	sprintf (s, "%02d/%02d/%04d",
		 (usf) ? tim->tm_mon + 1 : tim->tm_mday,
		 (usf) ? tim->tm_mday : tim->tm_mon + 1,
		 yr);
      else
	sprintf (s, "%02d:%02d:%02d %3.3s",
	     tim->tm_hour, tim->tm_min, tim->tm_sec, dy + tim->tm_wday * 3);
      break;
    }
}

char *vRetString(char *ptr)
{
 static char s[32];
 struct tm *tim;
  switch(*ptr) {
   case '"': ++ptr; return(ptr);
   case 'N':
   case 's': return((char*)getad(ptr));
   case 'd': sprintf(s,"%16.4f",*(double*)getad(ptr)); return s;
   case 'l': sprintf(s,"%11ld",*(long*)getad(ptr)); return s;
   case 'i': sprintf(s,"%6d",*(short*)getad(ptr)); return s;
   case 'e': sprintf(s,"%11d",*(int*)getad(ptr)); return s;
   case 'c': sprintf(s,"%4u",*(unsigned char*)getad(ptr)); return s;
   case 'D':
			tim=localtime((long*)getad(ptr));
	sprintf(s,"%02d/%02d/%04d",(usf) ? tim->tm_mon+1 : tim->tm_mday,
                 (usf) ? tim->tm_mday : tim->tm_mon+1,
                 tim->tm_year+1900); return s;
   default: return((char*)getad(ptr));
  }
}

