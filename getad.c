/* Function to return an address to be used - for vpg */
/* Franz Fortuny - June 12 1986 */
#include <stdio.h>
#include <ctype.h>
#ifdef VSAMD
#include "vsamstr.h"
#include "archi.h"
#endif

#ifdef SQLGEN
//#include "cli0core.h"
//#include "cli0defs.h"
//#include "cli0env.h"
//#include "cli0ext1.h"
#include "archisq.h"
extern ARCHI *ar;
#endif

void *getad(char *);
void *getad1(int, char*);

void *getad(char *ptr)
{
 int ae;
 if(*ptr == '"') return(ptr+1); 
 ae = atoi(ptr+1); while(*ptr++ != ':');
 return(getad1(ae,ptr));
}

void *getad1(int n,char *ptr)
{
    short *i;
    
    if(isdigit(*ptr)) return(ar[n].a_dat+atoi(ptr));
    ++ptr;
    if(isdigit(*ptr)) return(ar[n].a_dat+atoi(ptr));
    i = (short*)getad(ptr);
    return(ar[n].a_dat+ *i);
}
