/* tida.c = date and time related instructions */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *getad();

tida(char *ptr)
{
 char *ptr1,typ,s[12];
 extern char usf;
 long *l1,*l2,*l,totsec(),futdate(),pasdate(),tidis();
 int *ii, inc_z;
 static long hora;
 static int actual=1;
 switch(*ptr) {
  case 'f': /* format */
   ++ptr;
   if(*ptr == '1') usf=1;
   else usf=0;
  break;
  case 'd': /* days between two dates format YYMMDD */
            /* &dbuf,pos,bus,pos,buf,pos; integer 1stdate 2nddate */
   ++ptr;
    l = (long*) getad(ptr); while(*ptr++ != ',');
   l1 = (long*) getad(ptr); while(*ptr++ != ',');
   l2 = (long*) getad(ptr);
   *l = tidis(*l1,*l2);
  break;
  case 'l': /* &lfechas,fechaD; */
   ++ptr; ptr1 = getad(ptr); while(*ptr++ != ','); l = getad(ptr);
   memmove(s,ptr1,8); s[8]=0; *l = totsec(s);
  break;
  case '+':/* &+lbuf,pos,lbuf,pos,ibuf,pos; */
  case '-':
   typ=*ptr++;
   l=(long*)getad(ptr); while(*ptr++ != ',');
   l1=(long*)getad(ptr); while(*ptr++ != ',');
   ii=(int*)getad(ptr);
   if(typ=='+') *l = futdate(*l1,*ii);
   else *l = pasdate(*l1,*ii);
   break;
 case 's': // Add Some Time to a date &sdate,type,inc;
     ++ptr;
     l=(long*)getad(ptr); while(*ptr++ != ',');
     typ=*++ptr; while(*ptr++ != ',');
     inc_z = valu (ptr);
     *l = AddTime(typ, *l, inc_z);
  break;
  case 'a': /* Use actual time */
   actual=1;
  break;
  case 'b': /* Use time in Buffer nn, position ppp */
   actual=0;++ptr; hora= *(long*)getad(ptr);
  break;
  case 'p':  /* pasar hora a buffer de 26 o mas espacios */
   ++ptr; typ = *ptr;
   ptr1= (char*)getad(ptr+1);
   if(actual) time(&hora);
   putdate(ptr1,hora,typ);
  break;
  case 'c': /* Pasa hora de ahora a buffer */
   ++ptr; time((long*)getad(ptr));
  break;
  case 't': /* &tlong1,long2; long1=fecha vieja long2=recibe fecha nueva */
  break;
 }
} /* fin de tida.c */
