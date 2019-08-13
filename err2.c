/* rutina de error */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void err(char *format, ... )
{
        va_list arglist;

        fprintf(stderr,"\n\n\n\nProblem:\n");

        va_start(arglist,format);

        vfprintf(stderr,format,arglist);

        va_end(arglist);

        exit(0);
}
