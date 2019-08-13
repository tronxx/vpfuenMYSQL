/* Sub-rutinas para toma de strings y situacion de terminales */
/* modified for curses library on April 1997 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <curses.h>
#define MAXCAR 256
#define WKK win[vwa]
#define CTRL_U    21

int s_n, tkk;
extern unsigned attr;
extern int numin, vwa;
extern WINDOW *win[];

takein (buf, maxcar, upcase)
     char *buf;
     int maxcar, upcase;
{
  char s[MAXCAR];
  int i = 0, row, col, rr = 0, cc = 0, j, ll;
  unsigned int c;
  extern int tin;
  extern char reb;
  row = win[vwa]->_cury;
  col = win[vwa]->_curx;
  if (maxcar > MAXCAR)
    {
      perror ("TAKEIN: More than 256 chars\n");
      exit (0);
    }
  memset (s, 0, (unsigned) MAXCAR);
  memmove (s, buf, maxcar);
  for (i = strlen(s); i < maxcar; i++) s[i] = ' ';
  i = 0;
  immedok (WKK, TRUE);
  wattrset (win[vwa], A_REVERSE);
  mvwaddnstr (win[vwa], row, col, s, maxcar);
  wmove (win[vwa], row, col);
  wrefresh (win[vwa]);
  while ((c = wgetch (WKK)) != 13) {
      if (!isascii (c))	{
	  if (c == KEY_DOWN)
	    break;
	  switch (c) {
            case CTRL_U:
                llama_calcu(); redrawwin (win[vwa]); wrefresh (win[vwa]);
            break;
	    case KEY_LEFT:
	      if (i > 0) {
		  --i;
		  cc = WKK->_curx - 1;
		  wmove (WKK, WKK->_cury, cc);
                  wrefresh (WKK);
              }
              break;
            case KEY_BACKSPACE:
                if (i > 0) {
                    --i;
		    s[i] = ' ';
		    cc = WKK->_curx - 1;
		    wmove (WKK, WKK->_cury, cc);
		    wechochar (WKK, (chtype) ' ');
		    wmove (WKK, WKK->_cury, cc);
		    wrefresh (WKK);
		}
		break;
	    case KEY_RIGHT:
	      if (i < maxcar)
		{
		  ++i;
		  cc = WKK->_curx + 1;
		  wmove (WKK, WKK->_cury, cc);
		  wrefresh (WKK);
		}
	      break;
	    case KEY_END:
                /* i = maxcar; */
                for (ll = strlen(s); s[ll - 1] == ' '; ll--);
                i = ll;
	      cc = col + i;
	      wmove (WKK, WKK->_cury, cc);
	      wrefresh (WKK);
	      break;
	    case KEY_HOME:
	      i = 0;
	      cc = col;
	      wmove (WKK, row, cc);
	      wrefresh (WKK);
	      break;
	    case KEY_IC:
	      if (i < maxcar)
		{
		  rr = WKK->_cury;
		  cc = WKK->_curx;
		  memmove (s + i + 1, s + i, maxcar - i - 1);
		  s[i] = ' ';
		  mvwaddnstr (WKK, rr, cc, s + i, maxcar - i);
		  wmove (WKK, rr, cc);
		  wrefresh (WKK);
		}
	      break;
	    case KEY_UP:
	      wattrset (WKK, attr);
	      mvwaddnstr (WKK, row, col, buf, maxcar);
	      wrefresh (WKK);
	      s_n = 0;
	      immedok (WKK, FALSE);
	      return (-1);
	    case KEY_DC:
	      if (i < maxcar)
		{
		  rr = WKK->_cury;
		  cc = WKK->_curx;
		  memmove (s + i, s + i + 1, maxcar - i - 1);
		  s[maxcar - 1] = ' ';
		  waddnstr (WKK, s + i, maxcar - i);
		  wmove (WKK, rr, cc);
		  wrefresh (WKK);
		}
	      break;
	    default:
	      if (c < 256 && c > 127)
		goto accented;
	      tkk = c;
	      if (chefun (c) != 0)
		{
		  wattrset (WKK, attr);
		  mvwaddnstr (WKK, row, col, buf, maxcar);
		  wrefresh (WKK);
		  immedok (WKK, FALSE);
		  return (1099);
		}
	      break;
	    }
	}			/* end if function key touched */
      else
	{
	  if (!iscntrl (c) && i < maxcar)
	    {
	    accented:if (numin)
		{
		  if (!isdigit (c))
		    {
		      if (c == '.' || c == '-' || c == ' ');
		      else
			continue;
		    }
		}
	      if (upcase)
		{
		  c = toupper (c);
		}
	      s[i] = c;
	      wechochar (WKK, ((s_n) ? (chtype) ' ' : (chtype) c));
	      ++i;
	      if (i == maxcar && reb)
		break;
	      continue;
	    }			/* end if isprint */
	  else
	    {
	      switch (c)
		{
		case KEY_BACKSPACE:
		  if (i > 0)
		    {
		      --i;
		      s[i] = ' ';
		      cc = WKK->_curx - 1;
		      wmove (WKK, WKK->_cury, cc);
		      wechochar (WKK, (chtype) ' ');
		      wmove (WKK, WKK->_cury, cc);
		      wrefresh (WKK);
		    }
		  break;
		case 127:
		  if (i > 0)
		    {
		      --i;
		      s[i] = ' ';
		      cc = WKK->_curx - 1;
		      wmove (WKK, WKK->_cury, cc);
		      wechochar (WKK, (chtype) ' ');
		      wmove (WKK, WKK->_cury, cc);
		      wrefresh (WKK);
		    }
		  break;
		case 27:
		  wattrset (WKK, attr);
		  mvwaddnstr (WKK, row, col, buf, maxcar);
		  wrefresh (WKK);
		  s_n = 0;
		  immedok (WKK, FALSE);
		  return (-1);
		  break;
		case 24:
		  memset (s, ' ', (unsigned) maxcar);
		  mvwaddnstr (WKK, row, col, s, maxcar);
		  wmove (WKK, row, col);
		  wrefresh (WKK);
		  i = 0;
                  break;
                case CTRL_U:
                    llama_calcu(); redrawwin (win[vwa]); wrefresh (win[vwa]);
                break;

		default:
		  beep ();
		  break;
		}
	    }
	}			/* if not function key */
    }				/* end of while entering characters */
  if (!tin)
    {
      s[maxcar] = 0;
      for(j=maxcar-1;j>=0;j--)
        if(s[j] != ' ') break;
        else s[j] = 0;
    }
  memmove (buf, s, maxcar);
  memset(s,' ',maxcar);
  wattrset (WKK, attr);
  immedok (WKK, FALSE);
  if (s_n == 0)
    {
      mvwaddnstr (WKK, row, col, s, maxcar);
      mvwaddnstr (WKK, row, col, buf, (tin == 0 ? j+1 : maxcar));
    }
  else
    s_n = 0;
  wrefresh (WKK);
  return (i);
}
