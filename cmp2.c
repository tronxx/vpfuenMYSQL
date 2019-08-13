/* Funcion para comparar dos valores que pueden ser de varios tipos */
#include <string.h>

cmp(char *ptr)
{
 int res,res1;
 char typ;
 ++ptr;
 res = cmp2(ptr);
 for(;;) {
  while(*ptr++ != ')'); while(*ptr++ != ')');
  if(*ptr == '>' || *ptr == '<') return(res);
  else {
   typ = *ptr;
   switch(*ptr) {
    case '&': /* and */ res = res & cmp2(++ptr); break;
    case '|': /* or */  res = res | cmp2(++ptr); break;
    default:     break;
   } /* end of switch */
  } /* end of else */
 } /* end of for */
}

cmp2(char *ptr)
{
	void *getad(),*lqs;
    int f,cm,res,*ii;
    char *s1, *s2, typ;
    double k,d,atof();
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
