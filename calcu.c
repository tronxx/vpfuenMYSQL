/* Programa que sirve para emular una calculadora con control de pantalla
   DRBR 04-Feb-2002
*/

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern WINDOW *win[25];
extern int vwa, colc;
extern char *coma (char *);
void llama_calcu();
double calcu(double val1, double val2, char oper);



void llama_calcu ()
{
  WINDOW *wincalc, *opers;
  char c, showtot_z='N', simov_z = 'N';
  unsigned int car_z=0, lastop_z='+';
  double val1=0, val2=0, valtmp_z=0;
  int priop_z=1, ncar_z, ren_z, col_z;
  char formato_z[10], strval_z[80], valtec_z[80];
  sprintf(formato_z, "%%15.2f");
  //initscr(); cbreak(); noecho();
  ren_z = 1; col_z = 60;
  wincalc = newwin (10, 20, ren_z, col_z);
  opers = newwin (5, 18, ren_z+4, col_z+1);
  keypad(wincalc, TRUE);
  idcok (wincalc, TRUE);
  meta(wincalc, TRUE);
  //meta(opers, TRUE);
  if (colc) wattron (wincalc, COLOR_PAIR (colc));
  scrollok(wincalc, TRUE);
  scrollok(opers, TRUE);
  mvwhline(wincalc, 3, 0, ACS_HLINE, 20);
  box (wincalc, 0, 0);
  wclear(opers);

  mvwprintw(wincalc, 2, 1, "X para salir");
  wrefresh(wincalc);
  car_z=0; ncar_z=0;
  while( toupper(car_z) != 'X' && toupper(car_z) != 'Q') {
    car_z=mvwgetch(wincalc, 1, 1);
    // printf("Tecleaste:%d", car_z);
    switch (car_z) {
#ifdef _MOVWIN_
        case KEY_LEFT:
            if(col_z > 1) {
                col_z--;
                mvwin (wincalc, ren_z, col_z);
                mvwin (opers, ren_z+4, col_z+1);
                wrefresh(wincalc);
                wrefresh(opers);
            }
            break;
        case KEY_RIGHT:
            if(col_z < 60 ) {
                col_z++; 
                mvwin (wincalc, ren_z, col_z);
                mvwin (opers, ren_z+4, col_z+1);
                wrefresh(wincalc);
                wrefresh(opers);
            }
            break;
        case KEY_DOWN:
            if(ren_z < 24) {
                ren_z++; 
                mvwin (wincalc, ren_z, col_z);
                mvwin (opers, ren_z+4, col_z+1);
                wrefresh(wincalc);
                wrefresh(opers);
            }
            break;
        case KEY_UP:
            if(ren_z > 1) {
                ren_z--;
                mvwin (wincalc, ren_z, col_z);
                mvwin (opers, ren_z+4, col_z+1);
                wrefresh(opers);
                wrefresh(wincalc);
            }
            break;
#endif
        case KEY_BACKSPACE:
        case KEY_DC:
          if(ncar_z) { valtec_z[ncar_z-1]=0; ncar_z--; } break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
        case '.':
          valtec_z[ncar_z] = car_z; ncar_z++; break;
        case 'c':
        case 'C':
            memset(valtec_z, 0, 80); ncar_z=0; lastop_z = '+'; val1=0;
            wclear(opers); simov_z = 'N';
            break;
        case 'D':
        case 'd':
          if(ncar_z) { valtec_z[ncar_z-1]=0; ncar_z--; } break;
        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
        case '\n':
        case KEY_ENTER:
        case KEY_COMMAND:
        case 13:
         sprintf(strval_z, formato_z, valtmp_z);
         strcpy(strval_z, coma(strval_z));
         mvwprintw(wincalc, 1, 1, "%15s", strval_z);
         if(simov_z == 'S') wscrl(opers, 1);
         mvwprintw(opers, opers->_maxy, 0, "%14s%c", strval_z, lastop_z);
         wrefresh(opers);
         simov_z = 'S';
         //box (wincalc, 0, 0);
         mvwprintw(wincalc, 2, 1, "X para salir      ");
          val1=calcu(val1, atof(valtec_z), lastop_z);
          if(car_z != '=' && car_z != '\n' && car_z != KEY_COMMAND)
              lastop_z = car_z;
          valtmp_z = val1;
          sprintf(strval_z, formato_z, valtmp_z);
          strcpy(strval_z, coma(strval_z));
          wscrl(opers, 1);
          mvwprintw(opers, opers->_maxy, 0, "%14s%c", strval_z, '=');
          memset(valtec_z, 0, 80); ncar_z=0;
          if(car_z == '=' || car_z == KEY_ENTER  ||
             car_z == '\n' || car_z == KEY_COMMAND || car_z == 13
          ) lastop_z = '+';
          showtot_z = 'S';
          break;
    }
    if(showtot_z == 'N') valtmp_z = atof(valtec_z);
    sprintf(strval_z, formato_z, valtmp_z);
    strcpy(strval_z, coma(strval_z));
    mvwprintw(wincalc, 1, 1, "%15s", strval_z);
    wrefresh(opers); wrefresh(wincalc);
    showtot_z='N';
  }
  delwin (wincalc);
//  endwin();
}


double calcu(double valor1, double valor2, char oper)
{
   switch (oper) {
     case '+': valor1 = valor1 + valor2; break;
     case '-': valor1 = valor1 - valor2; break;
     case '*': valor1 = valor1 * valor2; break;
     case '/': valor1 = valor1 / valor2; break;
     default : valor1 = -1;
   }
   return (valor1);
}
