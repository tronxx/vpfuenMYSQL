/* quitar control-M de un file */
#include <stdio.h>
#include <stdlib.h>

void main(int argc,char *argv[])
{
  FILE *fp1, *fp2;
  unsigned int c;
  char sx[256];
  fp1 = fopen(argv[1],"rb");
  fp2 = fopen("newqm","wb");
  while((c = getc(fp1)) != EOF) {
   if(c == 13) continue;
   putc(c,fp2);  
  }
  fclose(fp1); fclose(fp2); 
  sprintf(sx,"mv newqm %s",argv[1]);
  system(sx);
}