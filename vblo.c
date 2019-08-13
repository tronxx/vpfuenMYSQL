/*
  vblo.c = rutinas de bloqueo para codigo complejo
           de acceso a disco en ambiente multi-usuario.
  Sintaxis dentro de VPG:
   Ba; * activar sistema de bloque en multi-usuario *
   Bc; * cerrar o desactivar sistema de bloqueo *
   Bb; * bloquear o preparar para codigo complejo *
   Bu; * desbloquear o liberar para codigo complejo *

   por Franz J Fortuny - Mayo 28 de 1989
   Adapted for Watcom C 386 QNX 4.2 Jan 14 1995
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#define CENTFILE "/vpgblock"
 struct flock fbk;

void vblo(act) char act;
{
 time_t sec;
 static int fb = -1;
 fbk.l_whence = SEEK_SET; fbk.l_start = 0l; fbk.l_len = 0l;
 switch(act) {
  case 'a': /* set blocking active */
   if((fb=open(CENTFILE,O_RDWR | O_CREAT,S_IRWXU | S_IRWXG | S_IRWXO)) == -1) {
 	perror("Problems opening or creating the file"); exit(0);
   }
   time(&sec);     
   if(write(fb,&sec,sizeof(time_t)) != sizeof(time_t))
    { perror("Can't write blocking record of file /vpgblock"); exit(0); }
   if(lseek(fb,0L,0) == -1L)
    { perror("Can not seek to pos 0 of vpgblock file."); exit(0); }
  break;
  case 'c': /* close blocking file */
   if(fb != -1) {close(fb); fb = -1;}
  break;
  case 'b': /* block file */
	fbk.l_type = F_WRLCK;
	if(fcntl(fb,F_SETLKW,&fbk) == -1)
    {perror("Complex Disk Access Code Lock is failing"); exit(0); }
  break;
  case 'u':/* unblock file */
	fbk.l_type = F_UNLCK;
	if(fcntl(fb,F_SETLK,&fbk) == -1)
   {perror("Complex Disk Access Code UnLock is failing"); exit(0); }
  break;
 }
}
