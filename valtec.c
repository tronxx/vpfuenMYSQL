/* Programa para determinar el valor de una tecla */
#include "stdio.h"
#include "stdlib.h"
#include <ctype.h>
#include <sys/dev.h>

main()
{
 unsigned int c;
	setraw();
	while(1) {
		c=getchar();
		printf("%3u ",c);
		if(c==4) exit(0);
	}
 setckd();
}
setraw()
{
	dev_mode(0,0,_DEV_MODES);
}

setckd()
{
	dev_mode(0,_DEV_MODES,_DEV_MODES);
}
