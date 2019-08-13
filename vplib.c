#include <stdio.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <fcntl.h>
#include "vsamstr.h"
#include <errno.h>
#include "archi.h"
#include <string.h>
struct flock vflock;
/* Funciones para manejo de movimientos */
/* Modified for Infodinamica - Check real EXISTENCE of records 880930 */
struct apmov
  {
    long pri;
    long ult;
  };

void fuplock(int n);
void vunlock(int n);
void fulock(int n);

long 
movfrsp (int fp)
{
  long l, l1;
  lseek (fp, 0l, 0);
  read (fp, &l, sizeof (long));
  if (l == 0)
    {
      l = lseek (fp, 0l, 2);
      return (l);
    }
  l1 = l;
  lseek (fp, l, 0);
  read (fp, &l, sizeof (long));
  lseek (fp, 0l, 0);
  write (fp, &l, sizeof (long));
  return (l1);
}

void 
movlib (int fp, long lug)
{
  long l;
  lseek (fp, 0l, 0);
  read (fp, &l, sizeof (long));
  lseek (fp, lug, 0);
  write (fp, &l, sizeof (long));
  l = 0xffffffff;
  write (fp, &l, sizeof (long));
  lseek (fp, 0l, 0);
  write (fp, &lug, sizeof (long));
}
vdread (int n)			/* function to read into the buffer of a file  */
{
  int sz;
  if (ar[n].a_lug <= 0l)
    return (-1);
  if (ar[n].a_type)
    sz = PRNREC + APUNS * ar[n].a_vsam->vtk + ar[n].a_sz;
  else
    sz = ar[n].a_sz;
  return (vdacread (n, sz));
}
vdreadl (n)			/* function to read into the buffer of a file with lock */
     int n;
{
  int sz;
  unsigned dif;
  if (ar[n].a_lug <= 0l)
    return (-1);
  if (ar[n].a_type)
    sz = PRNREC + APUNS * ar[n].a_vsam->vtk + ar[n].a_sz;
  else
    sz = ar[n].a_sz;
  if (ar[n].a_act != 0)
    {
      dif = ar[n].a_dat - ar[n].a_buf;
      lseek (ar[n].a_fd, (long) (ar[n].a_lug + (unsigned) dif), 0);
      sz = sz - (unsigned) dif;
      vflock.l_type = F_WRLCK;
      vflock.l_whence = SEEK_CUR;
      vflock.l_start = 0;
      vflock.l_len = (off_t) sz;
      if (fcntl (ar[n].a_fd, F_SETLK, &vflock) == -1)
	return (1);
      sz = sz + (unsigned) dif;
    }
  return (vdacread (n, sz));
}
vdlread (n)			/* read when it is possible to block */
     int n;
{
  int sz;
  unsigned dif;
  if (ar[n].a_lug <= 0l)
    return (-1);
  if (ar[n].a_type)
    sz = PRNREC + APUNS * ar[n].a_vsam->vtk + ar[n].a_sz;
  else
    sz = ar[n].a_sz;
  if (ar[n].a_act != 0)
    {
      dif = ar[n].a_dat - ar[n].a_buf;
      lseek (ar[n].a_fd, (long) (ar[n].a_lug + (unsigned) dif), 0);
      sz = sz - (unsigned) dif;
      vflock.l_type = F_WRLCK;
      vflock.l_whence = SEEK_CUR;
      vflock.l_start = 0;
      vflock.l_len = (off_t) sz;
      while (fcntl (ar[n].a_fd, F_SETLKW, &vflock) == -1)
	{
	  if (errno == EDEADLK)
	    perror ("Avoiding a deadlock...");
	}
      sz = sz + (unsigned) dif;
    }
  return (vdacread (n, sz));
}
long 
vdadd (int n)			/* function to add a record to file n */
{
  long movfrsp ();
  if (ar[n].a_act == 0)
    return (-1);
  fuplock (n);
  if (ar[n].a_type)
    {
      read (ar[n].a_fd, ar[n].a_vsam, VSAM);
      ar[n].a_lug = inclu (ar[n].a_fd, ar[n].a_vsam, ar[n].a_buf);
    }
  else
    {
      ar[n].a_max = lseek (ar[n].a_fd, 0l, 2);
      ar[n].a_lug = movfrsp (ar[n].a_fd);
      if (ar[n].a_lug < 0 || ar[n].a_lug > ar[n].a_max)
	{
	  fprintf (stderr, "\n\n   Corrupt pointer to free space. File # %d   \n\n", n);
	  exit (0);
	}
      lseek (ar[n].a_fd, ar[n].a_lug, 0);
      write (ar[n].a_fd, ar[n].a_buf, ar[n].a_sz);
    }
  fulock (n);
  ar[n].a_max = lseek (ar[n].a_fd, 0l, 2);
  vdread (n);
  return (ar[n].a_lug);
}
vddel (int n)			/* function to delete record from file */
{
  extern int seq;
  long *l;
  if (ar[n].a_lug <= 0l)
    return (-1);
  if (ar[n].a_act == 0)
    return (-1);
  fuplock (n);
  if (ar[n].a_type)
    {
      read (ar[n].a_fd, ar[n].a_vsam, VSAM);
      vdel (ar[n].a_fd, ar[n].a_vsam, ar[n].a_buf, ar[n].a_lug);
      vdlread (n);
      *(long *) (ar[n].a_dat + sizeof (long)) = -1l;
      vdwrite (n);
    }
  else
    {
      seq = 0;
      movlib (ar[n].a_fd, ar[n].a_lug);
      vdread (n);
    }
  fulock (n);
  return (0);
}
vdwrite (int n)			/* function to update current record in file */
{
  int sz;
  long lug;
  ar[n].a_max = lseek (ar[n].a_fd, 0l, 2);
  if (ar[n].a_lug <= 0l || ar[n].a_lug >= ar[n].a_max)
    {
      fprintf (stderr, "\nTrying to write to an out of range position (%d)\n", n);
      exit (0);
    }
  if (ar[n].a_act == 0)
    return (-1);
  if (ar[n].a_type)
    lug = ar[n].a_lug + (long) PRNREC + APUNS * ar[n].a_vsam->vtk;
  else
    lug = ar[n].a_lug;
  lseek (ar[n].a_fd, lug, 0);
  if ((sz = write (ar[n].a_fd, ar[n].a_dat, ar[n].a_sz)) == -1)
    {
      fprintf (stderr, "\n\n\n\nSYSTEM WRITE ERROR: No. %u\n.Check situation.\n", errno);
      exit (0);
    }
  vunlock (n);
  return (sz);
}
vdlink (int n1, int n2, struct apmov * st1, struct apmov * st2)
 /* instruction to link and add */
{
  long lug;
  if (ar[n1].a_lug <= 0l || ar[n2].a_lug <= 0l)
    return (-1);
  fuplock (n2);
  vdlread (n1);
  vdlread (n2);
  memset (st2, 0, sizeof (struct apmov));
  if (st1->ult == 0l)
    {
      st1->pri = ar[n2].a_lug;
      st1->ult = ar[n2].a_lug;
      vdwrite (n1);
      vdwrite (n2);
    }
  else
    {
      st2->pri = st1->ult;
      vdwrite (n2);
      lug = ar[n2].a_lug;
      ar[n2].a_lug = st1->ult;
      vdlread (n2);
      st2->ult = lug;
      vdwrite (n2);
      ar[n2].a_lug = lug;
      st1->ult = lug;
      vdwrite (n1);
    }
  vdread (n2);
  fulock (n2);
  return (ar[n2].a_sz);
}
vdunlink (int n1, int n2, struct apmov * st1, struct apmov * st2)
 /* function to break chain between records */
{
  long lug, anter, proxi;
  if (ar[n1].a_lug == 0l || ar[n2].a_lug == 0l)
    return (-1);
  fuplock (n2);
  vdlread (n1);
  vdlread (n2);
  anter = st2->pri;
  proxi = st2->ult;
  lug = ar[n2].a_lug;
  if (anter == 0l)
    st1->pri = st2->ult;
  else
    {
      ar[n2].a_lug = anter;
      vdlread (n2);
      st2->ult = proxi;
      vdwrite (n2);
      ar[n2].a_lug = lug;
      vdlread (n2);
    }
  if (proxi == 0l)
    st1->ult = st2->pri;
  else
    {
      ar[n2].a_lug = proxi;
      vdlread (n2);
      st2->pri = anter;
      vdwrite (n2);
    }
  vdwrite (n1);
  ar[n2].a_lug = lug;
  vdlread (n2);
  memset (st2, 0, sizeof (struct apmov));
  fulock (n2);
  return (vdwrite (n2));
}

void vunlock(int n)
{
  int sz;
  unsigned dif;
  if (ar[n].a_type == 1)
    sz = PRNREC + APUNS * ar[n].a_vsam->vtk + ar[n].a_sz;
  else
    sz = ar[n].a_sz;
  dif = ar[n].a_dat - ar[n].a_buf;
  /* lseek(ar[n].a_fd,(long)(ar[n].a_lug+(unsigned)dif),0); */
  vflock.l_type = F_UNLCK;
  vflock.l_whence = SEEK_SET;
  vflock.l_start = ar[n].a_lug + (unsigned long) dif;
  vflock.l_len = (long) sz;
  fcntl (ar[n].a_fd, F_SETLK, &vflock);
  /* lockf(ar[n].a_fd,0,(long)sz); */
  sz = sz - (unsigned) dif;
}
void fuplock(int n)
			/* function to lock upper part of file */
{
  if (ar[n].a_act == 0)
    return;
  lseek (ar[n].a_fd, 0l, 0);
  vflock.l_type = F_WRLCK;
  vflock.l_whence = SEEK_CUR;
  vflock.l_start = 0l;
  if (ar[n].a_type)
    {
      vflock.l_len = (long) VSAM;
    }
  else
    {
      vflock.l_len = (long) ar[n].a_sz;
    }
  while (fcntl (ar[n].a_fd, F_SETLKW, &vflock) == -1);
}
void 
fulock (int n)
{
  lseek (ar[n].a_fd, 0l, 0);
  vflock.l_type = F_UNLCK;
  vflock.l_whence = SEEK_CUR;
  vflock.l_start = 0l;
  if (ar[n].a_type)
    {
      vflock.l_len = (long) VSAM;
    }
  else
    {
      vflock.l_len = (long) ar[n].a_sz;
    }
  fcntl (ar[n].a_fd, F_SETLK, &vflock);
}
vdacread (int n, int sz)
{
  int re;
  extern int seq;
  long *l;
  l = (long *) (ar[n].a_dat + sizeof (long));
read_again:
  lseek (ar[n].a_fd, ar[n].a_lug, 0);
  if ((re = read (ar[n].a_fd, ar[n].a_buf, sz)) == -1)
    {
      if (errno == EINTR || errno == EAGAIN)
	goto read_again;
      fprintf (stderr, "\n\n\n\n\n\nSYSTEM READ ERROR: %u\n.Check situation.\n", errno);
      exit (0);
    }
  if (*l == -1l)
    {
      if (ar[n].a_type)
	{
	  return (-1);
	}
      else
	{
	  if (seq)
	    {
	      if (re == 0)
		{
		  ar[n].a_lug -= (long) ar[n].a_sz;
		  return (0);
		}
	      ar[n].a_lug += (long) ar[n].a_sz;
	      goto read_again;
	    }
	}
    }
  return (re);
}
