/* Reverse Polish Notation Function for VPG text F Fortuny 881029 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define GP getad(ptr)
extern int ins;

void err(char *, ... );

double calcd(char *ptr)
{
    char typ, sgn, *cc;
	void *getad();
    int f,stk=0;
    short *ii;
    long *ll;
    float *ff;
    double  *dd, acum[256];
    while(*ptr != ';') {
     if(*ptr == '(') {
      ++ptr;
      typ= *ptr;
      switch(typ) {
        case 'd': acum[stk++] = *(double*)GP; break;
        case 'i': acum[stk++] = (double)(*(short*)GP); break;
        case 'l': case 'D': acum[stk++] = (double)(*(long*)GP); break;
        case 'f': acum[stk++] = (double) (*(float*)GP); break;
		case 'c': acum[stk++] = (double) (*(char*)GP); break;
        default: /* must be digit */
         acum[stk++]=atof(ptr);
        break;
      } /* end switch */
   while(*ptr++ != ')');
     } /* end if = ( */
     else {
      --stk; if(stk < 0) err("Check calculation instruction %d!\n",ins);
      switch(*ptr) {
       case '+': acum[stk-1] += acum[stk]; break;
       case '-': acum[stk-1] -= acum[stk]; break;
       case '*': acum[stk-1] *= acum[stk]; break;
       case '/': acum[stk-1] /= acum[stk]; break;
       case '^': acum[stk-1] = pow(acum[stk-1],acum[stk]); break;
       case 'e': acum[stk] = log(acum[stk]); stk++; break;
       case 'L': acum[stk] = log10(acum[stk]); stk++; break;
       case 'x': acum[stk] = exp(acum[stk]); stk++; break;
       /*case 'X': acum[stk] = exp10(acum[stk]); stk++; break;*/
       case 's': acum[stk] = sin(acum[stk]); stk++; break;
       case 'c': acum[stk] = cos(acum[stk]); stk++; break;
       case 'S': acum[stk] = asin(acum[stk]); stk++; break;
       case 'r': acum[stk++] = 
			(double)rand() / (double)RAND_MAX; break;
       case '%': acum[stk-1] = (long) acum[stk-1] % (long) acum[stk]; break;
       default: err("CALCULATION: Operand Error! (%c) instr %d\n",*ptr,ins); break;
      } /* end of sign switch */
      ++ptr;
     } /* end else if not ( (sign then) */
    }/* end of while */
 return(acum[0]);
}
