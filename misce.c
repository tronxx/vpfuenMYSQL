#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <curses.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

void *getad ();
void err (char *,...);
long IncMonth(long fecha_z, int inc_z);
long IncYear(long fecha_z, int inc_z);
long LastOfMonth(long fecha_z);
bool IsLastOfMonth(long fecha_z);

produ (char c, FILE * fp, int n)
{
  char s[256];
  extern char vido,vwa;
  extern unsigned attr;
  extern WINDOW *win[];
  memset (s, c, n);
  if (vido)
    waddnstr(win[vwa],s,n);
  else
    fwrite (s, n, 1, fp);
  fflush (fp);
}

void handle ()
{
  extern char stop;
  signal (SIGINT, (void*)&handle);
  stop = 1;
}

void nada ()
{
  extern char stop;
  signal (SIGINT, (void*)&nada);
  stop = 0;
}

long AddTime(char type_z, long fecha_z, int inc_z)
{
    int ii_z;
    long fecini_z=0;
    switch (type_z) {
    case 's': fecha_z += inc_z;             break;
    case 'm': fecha_z += 60 * inc_z;        break;
    case 'h': fecha_z += 3600  * inc_z;     break;
    case 'd': fecha_z += 86400 * inc_z;     break;
    case 'W': fecha_z += 86400 * 7 * inc_z; break;
    case 'M':
        fecini_z = fecha_z;
        fecha_z = IncMonth(fecha_z, inc_z);
        if(IsLastOfMonth(fecini_z)) fecha_z = LastOfMonth(fecha_z);
        break;
    case 'Y':
        fecini_z = fecha_z;
        fecha_z = IncYear(fecha_z, inc_z);
        if(IsLastOfMonth(fecini_z)) fecha_z = LastOfMonth(fecha_z);
        break;
    }
    return(fecha_z);

}

long IncMonth(long fecha_z, int inc_z)
{
    long fecini_z=0;
    struct tm *fechora_z;
    int anus_z = 0;
    int meses_z = 0;
#ifdef DEBUG
      FILE *debug;
    if ((debug = fopen ("debug.txt", "wb+")) == NULL)
      printf ("File debug.txt can't be opened\n");
#endif
    anus_z = inc_z / 12;
    meses_z = inc_z - anus_z * 12;
#ifdef DEBUG
  fprintf(debug, "valor de fecha_z=%ld anus_z=%d meses_z=%d\n", fecha_z, anus_z, meses_z);
  fprintf(debug, "voy a ejecutar el localtime sobre fecha_z\n");
#endif
  fechora_z = localtime(&fecha_z);
#ifdef DEBUG
  fprintf(debug, "Ejecutado el localtime sobre fecha_z\n");
  fclose(debug);
#endif
    fechora_z->tm_mon += meses_z;
    fechora_z->tm_year += anus_z;
    switch(fechora_z->tm_mon) {
    case 4:
    case 6:
    case 9:
    case 11:
        if(fechora_z->tm_mday > 30) fechora_z->tm_mday = 30; break;
    case 2:
        if(fechora_z->tm_year % 4) {
            if(fechora_z->tm_mday > 29) fechora_z->tm_mday = 29;
        } else {
            if(fechora_z->tm_mday > 28) fechora_z->tm_mday = 28;
        }
        break;
    }
    fecha_z = mktime(fechora_z);
    return (fecha_z);
}

bool IsLastOfMonth(long fecha_z)
{
    struct tm *fechora_z;
    bool sies_z = false;
    fechora_z=localtime(&fecha_z);
    switch (fechora_z->tm_mon) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        if(fechora_z->tm_mday == 31) sies_z = TRUE; break;
    case 4:
    case 6:
    case 9:
    case 11:
        if(fechora_z->tm_mday == 30) sies_z = TRUE; break;
    case 2:
        if(fechora_z->tm_year % 4) {
            if(fechora_z->tm_mday == 29) sies_z = true;
        } else {
            if(fechora_z->tm_mday == 28) sies_z = true;
        }
    }
    return (sies_z);
}

long LastOfMonth(long fecha_z)
{
    struct tm *fechora_z;
    fechora_z=localtime(&fecha_z);
    switch (fechora_z->tm_mon) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        fechora_z->tm_mday = 31; break;
    case 4:
    case 6:
    case 9:
    case 11:
        fechora_z->tm_mday = 30; break;
    case 2:
        if(fechora_z->tm_year % 4)
            fechora_z->tm_mday = 29;
        else
            fechora_z->tm_mday = 28;
    }
    fecha_z = mktime(fechora_z);
    return (fecha_z);
}

long IncYear(long fecha_z, int inc_z)
{
    long fecini_z=0;
    struct tm *fechora_z;
    fechora_z=localtime(&fecha_z);
    fechora_z->tm_year += inc_z;
    switch(fechora_z->tm_mon) {
    case 4:
    case 6:
    case 9:
    case 11:
        if(fechora_z->tm_mday > 30) fechora_z->tm_mday = 30; break;
    case 2:
        if(fechora_z->tm_year % 4) {
            if(fechora_z->tm_mday > 29) fechora_z->tm_mday = 29;
        } else {
            if(fechora_z->tm_mday > 28) fechora_z->tm_mday = 28;
        }
        break;
    }
    fecha_z = mktime(fechora_z);
    return (fecha_z);
}


long
futdate (long ol, int days)
{
  return (ol + ((long) days * 60l * 60l * 24l));
}

long
pasdate (long tod, int days)
{
  return (tod - ((long) days * 60l * 60l * 24l));
}

long
tidis (long l1, long l2)
{
  return ((l1 - l2) / 60l / 60l / 24l);
}

void
putdate (char *buf, long l, char f)
{
  struct tm *tiem, *localtime ();
  int yr;
  extern char *dy, *mt;
  char s[30];
  tiem = localtime (&l);
  if (tiem->tm_year >= 100)
    yr = tiem->tm_year - 100;
  else
    yr = tiem->tm_year;
  switch (f)
    {
    case '1':			/* format type YYMMDD */
      sprintf (s, "%02d%02d%02d", yr, tiem->tm_mon + 1, tiem->tm_mday);
      memmove (buf, s, 6);
      return;
    case '2':			/* format type MMYYDD */
      sprintf (s, "%02d%02d%02d", tiem->tm_mon + 1, tiem->tm_mday, yr);
      memmove (buf, s, 6);
      return;
    case '3':			/* format Day Mon Mday Year HH:MM:SS */
      sprintf (buf, "%3.3s %3.3s %2d %4d %02d:%02d:%02d", dy + tiem->tm_wday * 3,
	       mt + tiem->tm_mon * 3, tiem->tm_mday, 1900 + tiem->tm_year,
	       tiem->tm_hour, tiem->tm_min, tiem->tm_sec);
      return;
  case '4': /* format ccyymmdd */
	sprintf(s,"%04d%02d%02d",tiem->tm_year+1900,tiem->tm_mon+1,tiem->tm_mday);
   memmove(buf,s,8);
  return;
  case '5': /* just the hour */
   sprintf(s,"%02d:%02d:%02d",tiem->tm_hour,tiem->tm_min,tiem->tm_sec);
   memmove(buf,s,8);
  return;
    }
}

valu (char *ptr)
{
  if (isdigit (*ptr))
    return (atoi (ptr));
  return ((int) *(short *) getad (ptr));
}
unsigned
valuh (ptr)
     char *ptr;
{
  if (isdigit (*ptr))
    return (atoi (ptr));
  return (*(unsigned short *) getad (ptr));
}

long
totsec (char *ptr)
{
  extern char usf;
  int yy, mm, dd, y, m, d = 0;
  if (usf == 1)
    {
      mm = atoi (ptr);
      dd = atoi (ptr + 3);
    }
  else
    {
      dd = atoi (ptr);
      mm = atoi (ptr + 3);
    }
  yy = atoi (ptr + 6);
  if (yy < 70)
    yy += 2000;
  else if(yy < 100) yy += 1900;

  if ((mm < 1 || mm > 12) || (dd < 1 || dd > 31))
    return (-1l);
  for (y = 1970; y < yy; y++)
    {
      if (y % 4 == 0)
	d += 366;
      else
	d += 365;
    }
  for (m = 1; m < mm; m++)
    {
      switch (m)
	{
	case 2:
	  if (y % 4 == 0)
	    d += 29;
	  else
	    d += 28;
	  break;
	case 4:
	case 6:
	case 9:
	case 11:
	  d += 30;
	  break;
	default:
	  d += 31;
	  break;
	}
    }
  if( m == 4 || m == 6 || m == 9 || m == 11)
   if(dd > 30) return -1;
  if (m == 2)
    {
      if (yy % 4 != 0)
	{
	  if (dd > 28)
	    return (-1);
	}
      else
	{
	  if (dd > 29)
	    return (-1);
	}
    }
  d += dd;
  return ((long) (d * 24l * 60l * 60l));
}

rout (char *ptr)
{
  char i, *ii;
  ++ptr;
  i = *(int *) getad (ptr);
  while (*ptr++ != ',');
  while ((i != *ptr) && (*ptr != '`'))
    {
      ptr = ptr + (sizeof (char) + sizeof (int));
    }
  return (*(int *) (ptr + 1));
}

dirop (char *ptr)
{
  ++ptr;
  switch (*ptr)
    {
    case '(':
      ++ptr;
      ptr = (char *) getad (ptr);
      chdir (ptr);
      return (0);
      break;
    case '=':
      ++ptr;
      ptr = (char *) getad (ptr);
      getcwd (ptr, 80);
      return (0);
      break;
    default:
      chdir (ptr);
      return (0);
      break;
    }
}

struct posics
{
  char row, col;
};
extern struct posics *local;
extern int MAXLOC;
getlocs (char *ptr)
{
  FILE *fp;
  char s[256];
  int i = 0;
  if ((fp = fopen (ptr, "r")) == NULL)
    {
      err ("Screen locations file %s does not exist.\n", ptr);	/* ptr */
    }
  while ((fgets (s, 200, fp)) != NULL)
    {
      local[i].row = atoi (s);
      local[i++].col = atoi (s + 3);
    }
  fclose (fp);
}

relocs (char *ptr)
{
  int i;
  switch (*ptr++)
    {
    case '+':
      if (*ptr == 'r')
	for (i = 0; i < MAXLOC; i++)
	  local[i].row++;
      else
	for (i = 0; i < MAXLOC; i++)
	  local[i].col++;
      break;
    case '-':
      if (*ptr == 'r')
	for (i = 0; i < MAXLOC; i++)
	  local[i].row--;
      else
	for (i = 0; i < MAXLOC; i++)
	  local[i].col--;
      break;
    }
}
