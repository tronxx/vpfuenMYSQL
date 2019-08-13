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
  fprintf(fp2, "<html>\n");
  fprintf(fp2, "<body><pre>\n");
  while((c = getc(fp1)) != EOF) {
      putc(c,fp2);
  }
  fprintf(fp2, "</pre></body>\n");
  fprintf(fp2, "</html>\n");
  fclose(fp1); fclose(fp2); 
  sprintf(sx,"mv newqm %s.html",argv[1]);
  system(sx);
}
