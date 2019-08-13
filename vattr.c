/* Function to print string in alternate attribute */

#include <curses.h>
extern int vrow,vcol,vwa, colc;
extern unsigned attr, attr1;
extern WINDOW *win[];
void vattr(char *s,int ln)
{
 int alter = 0;
/* if(attr == attr1) {
  waddstr(win[vwa],s);
  wrefresh(win[vwa]);
  return;
 }*/
 
 while(ln > 0) {
   if(*s == '`') {
     if(alter == 0) {
         alter = 1;
         wattrset(win[vwa], attr1);
     }
     else { 
       alter = 0;
       wattrset(win[vwa], attr);
     }
     wattron (win[vwa], COLOR_PAIR (colc));
     --ln; ++s; continue;
   }
   waddch(win[vwa],*s); ++s; --ln;
 }
 wrefresh(win[vwa]);
 return;
}
