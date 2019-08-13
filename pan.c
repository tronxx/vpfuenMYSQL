/* Funcion para formar pantallas de trabajo segun parametros que se deben
   en el archivo cuyo nombre viene como argumento del programa */
#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

void err (char *,...);
extern WINDOW *win[];
extern int vwa;

void
pan (char *a)
{
  int i;
  char s[300];
  FILE *fp;
  extern unsigned attr, attr1;
  if ((fp = fopen (a, "r")) == NULL)
    err ("Can't open screen file %s", a);
  while (fgets (s, 200, fp) != NULL)
    {
      if (s[0] == 'A')
	{
	  posre (fp, s);
	  break;
	}
      s[strlen (s) - 1] = 0;
      wmove (win[vwa], atoi (s), atoi (s + 3));
      vattr (s + 6, strlen (s) - 6);
    }
  fclose (fp);
}

posre (fp, s)
     FILE *fp;
     char s[];
{
  extern unsigned attr, attr1;
  int col, r;
  col = win[vwa]->_curx;
  r = win[vwa]->_cury;
  while (fgets (s, 200, fp) != NULL)
    {
      s[strlen (s) - 1] = 0;
      wmove (win[vwa], r++, col);
      vattr (s, strlen (s));
    }
}
