/* To Read serial ports */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

static int fd[10];
static struct termios oldtio[10],newtio[10];

int vsetser(char *dispo,int nser,short BAUDRATE)
{

   fd[nser] = open(dispo, O_RDWR | O_NOCTTY );
   if (fd[nser] <0) {perror(dispo); exit(-1); }
   
   tcgetattr(fd[nser],&oldtio[nser]);
   bzero(&newtio[nser], sizeof(newtio[nser]));
   
   newtio[nser].c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
   
   newtio[nser].c_oflag = 0;
   
   newtio[nser].c_lflag = ICANON;
   
   newtio[nser].c_cc[VINTR]    = 0;     /* Ctrl-c */
   newtio[nser].c_cc[VQUIT]    = 0;     /* Ctrl-\ */
   newtio[nser].c_cc[VERASE]   = 0;     /* del */
   newtio[nser].c_cc[VKILL]    = 0;     /* @ */
   newtio[nser].c_cc[VEOF]     = 4;     /* Ctrl-d */
   newtio[nser].c_cc[VTIME]    = 0;     
   newtio[nser].c_cc[VMIN]     = 1;     
   newtio[nser].c_cc[VSWTC]    = 0;     /* '\0' */
   newtio[nser].c_cc[VSTART]   = 0;     /* Ctrl-q */
   newtio[nser].c_cc[VSTOP]    = 0;     /* Ctrl-s */
   newtio[nser].c_cc[VSUSP]    = 0;     /* Ctrl-z */
   newtio[nser].c_cc[VEOL]     = 0;     /* '\0' */
   newtio[nser].c_cc[VREPRINT] = 0;     /* Ctrl-r */
   newtio[nser].c_cc[VDISCARD] = 0;     /* Ctrl-u */
   newtio[nser].c_cc[VWERASE]  = 0;     /* Ctrl-w */
   newtio[nser].c_cc[VLNEXT]   = 0;     /* Ctrl-v */
   newtio[nser].c_cc[VEOL2]    = 0;     /* '\0' */
   
   tcflush(fd[nser], TCIFLUSH);
   tcsetattr(fd[nser],TCSANOW,&newtio[nser]);
   
return fd[nser];
}


int vreadser(int nser,char *buf)
{
  int res;
      res = read(fd[nser],buf,255);
      buf[res]=0;
	  return res;
}

void vcloseser(int nser)
{
 tcsetattr(fd[nser],TCSANOW,&oldtio[nser]);
}
