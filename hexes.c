#include <stdio.h>
#include <stdlib.h>

main()
{
  char s[200];
  unsigned long val;
  unsigned char uc;
  while(1) {
  gets(s);
  val = strtoul(s,NULL,16);
  uc = (unsigned char)val;
  printf("Valor = %ld Char: %c\n",val,uc);
  }
}