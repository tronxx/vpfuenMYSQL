/* Rutina para colocar comas a cantidades
	por Franz J Fortuny Enero de 1995 - paquete VAG */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *coma(char *s)
{
	static char s2[64];
	int pospun,pospun1,i=0;
	strcpy(s2,s);
	pospun = strcspn(s,".") - 1;
	pospun1 = pospun;
	do {
	   s2[pospun1] = s[pospun];
	   if(++i == 3 && isdigit(s[pospun-1]))
	     { s2[--pospun1] = ','; i=0; }
	   --pospun1;
	} while(isdigit(s[--pospun]));
	if(s[pospun] != ' ') s2[pospun1] = s[pospun];
	return((char*)s2);
}
