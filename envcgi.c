/* env.cgi.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
extern char **environ;
int main()
{
   char **p = environ;
   char *v,sz[32];
   int nsz;
   printf("Content-Type: text/html\r\n\r\n");
   printf("<html> <head>\n");
   printf("<title>CGI Environment</title>\n");
   printf("</head>\n");
   printf("<body>\n");
   printf("<h1>CGI Environment</h1>\n");
   while(*p != NULL)
      printf("%s<br>\n",*p++);
   strcpy(sz,getenv("CONTENT_LENGTH"));
   nsz = atoi(sz);
   v = calloc(1,nsz);
   fread(v,nsz,1,stdin);
   printf("<pre>%s</pre>\n",v);
   printf("</body> </html>\n");
}

{
   switch(*ptr) {
    case '+': /* a space */
     *varcon++ = ' ';
    break;
    case '&': /* different field */
     *varcon = 0;
     fld[nf].con = calloc(1,strlen(vc));
     strcpy(fld[nf].con,vc);
     varcon = vc;
    break;
    case '=': /* The value of the field */
     *varcon = 0;
     fld[nf].nam = calloc(1,strlen(vc));
     strcpy(fld[nf].nam,vc);
     varcon = vc;
    break;
    case '%': /* probably a special character */
    break;
    default:
     *varcon++ = *ptr;
    break;
   }
   ++ptr;
   ++i;
}
