/*
 vstrings.c = sequence of instructions to handle
 string manipulation in VPG.
 By Franz J Fortuny - June 7 1989
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

vstr(ptr) char *ptr;
{
 void *getad();
 char a,*st1,*st2;
 int *ii,i; short *ss;
 a = *ptr; ++ptr;
 switch(a) {
  case 'c': /* concatenate two strings */
   /* Fscstring1,string2; */
   st1 = (char*) getad(ptr); while(*ptr++ != ',');
   strcat(st1,(char*)getad(ptr));
  break;
  case 'e': /* extension of non white characters */
   st1 = (char*) getad(ptr); while(*ptr++ != ','); i=valu(ptr); while(*ptr++ != ',');
   *(short*)getad(ptr) = exten(st1+i,i);
  break;
  case 'l': /* length of string */
   /* Fslivar,string; */
   ss = (short*)getad(ptr); while(*ptr++ != ',');
   *ss = strlen((char*)getad(ptr));
  break;
 }
}

exten(char *s,int n)
{
 while(*--s == ' ' && n-- != 0);
 return(n);
}
