/* Funcion para comparar dos valores que pueden ser de varios tipos */
/* Modificada agosto 28 de 1995: (...)0(...)(...)1(...)&>..  */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <decimal.h>
//#include <xxcvt.h>
  void err(char *,...);
  void *getad(char *);
  static int cmp2(char *);
  extern int valu(char *);
  int instr(char *,char *,int);

int cmp(char *ptr)
{
 int res[20], niv=0;
 ++ptr;
 for(;;) { if(niv<0) err("Instruction !( in error");
  res[niv] = cmp2(ptr);
  while(*ptr++ != ')'); while(*ptr++ != ')');
s: switch(*ptr) {
    case '&': res[niv-1]=(res[niv] && res[niv-1]); --niv; ++ptr; goto s;
    case '|': res[niv-1]=(res[niv] || res[niv-1]); --niv; ++ptr; goto s;
    case '<': case '>': return(res[0]);
    case '(': ++niv; break;
   } /* end of switch */
 } /* end of for */
}

cmp2(char *ptr)
{
        void *getad(),*lqs;
    int f,cm,res;
    char *s1, *s2, typ;
    double k,d;
    ++ptr;
    if(*ptr == 's') { /* string comparison */
        s1 = (char*)getad(ptr);
        while(*ptr++ != ')');
        cm = atoi(ptr); while(*ptr++ != '(');
        if(*ptr=='$') {++ptr; s2 = ptr; }
        else {
        s2 = (char*)getad(ptr);
        }
        while(*ptr++ != ',');
        if(*ptr == 'l') f=strlen(s2);
        else f = valu(ptr);
        if(cm==6) {
         return(instr(s2,s1,f));
        }
        else res = strncmp(s1,s2,f);
        if((res == 0) && (cm == 0 || cm == 3 || cm == 4)) return(1);
        if((res > 0) && (cm == 1 || cm == 3 || cm == 5)) return(1);
        if((res < 0) && (cm == 2 || cm == 4 || cm == 5)) return(1);
        return(0);
    }
    else { /* value comparison */
        typ = *ptr; lqs = getad(ptr);
        switch(typ) {
          case 'i': k = (double) *(short*)lqs; break;
          case 'l': case 'D': k = (double) *(long*)lqs; break;
          case 'f': k = (double) *(float*)lqs; break;
          case 'd': k = (double) *(double*)lqs; break;
//          case 'a': k=QXXPTOD(lqs,16,2); break;
          case 'c': k = (double) *(char*)lqs; break;
        }
        while(*ptr++ != ')'); cm = atoi(ptr); while(*ptr++ != '(');
        if(*ptr == '$') {++ptr; d = atof(ptr);}
        else {
                typ = *ptr; lqs = getad(ptr);
         switch(typ) {
          case 'i': d = (double) *(short*)lqs; break;
          case 'l': d = (double) *(long*)lqs; break;
          case 'f': d = (double) *(float*)lqs; break;
          case 'd': d = (double) *(double*)lqs; break;
//          case 'a': d = QXXPTOD(lqs,16,2); break;
          case 'c': d = (double) *(char*)lqs; break;
          }
        }
        if((k == d) && (cm == 0 || cm == 3 || cm == 4)) return(1);
        if((k > d) && (cm == 1 || cm == 3 || cm == 5)) return(1);
        if((k < d) && (cm == 2 || cm == 4 || cm == 5)) return(1);
        return(0);
    }
} /* fin de cmp2.c */

instr(s1,s2,n) char *s1,*s2; int n;
{
 int l;
 l=strlen(s2);
 n = n - l;
 while(strncmp(s1,s2,l) != 0) { ++s1; if(--n == 0) return(0); }
 return(1);
}


