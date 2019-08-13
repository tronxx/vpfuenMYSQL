/* hello.cgi.c - Hello, world CGI */
#include <stdio.h>
int main() {
   printf("Content-Type: text/html\n\n");
   printf("<html> <head>\n");
   printf("<title>Hello, World!</title>\n");
   printf("</head>\n");
   printf("<body>\n");
   printf("<h1>Hello, World!</h1>\n");
   printf("</body> </html>\n");
}
