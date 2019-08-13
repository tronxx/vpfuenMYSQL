#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "vsamstr.h"
int mues = 1;
main (argc, argv)
     int argc;
     char *argv[];
{
  struct vsam v;
  struct apuns *ap;
  int fp;
  char *buf, *dat, k;
  long rec = 0l, lug;
  int size, i;
  if (argc == 1)
    {
      fprintf (stdout, "Program to extract key values from records in a BASEM file.\n");
      fprintf (stdout, "Usage:\n");
      fprintf (stdout, "    exbas file\n");
      exit (0);
    }
  if ((fp = open (argv[1], O_RDONLY)) == 0)
    {
      fprintf (stderr, "Can't open %s\n", argv[1]);
      exit (0);
    }
  if (argc == 3)
    mues = 0;
  if (mues)
    fprintf (stdout, "VSAM Indexed file: %s\n", argv[1]);
  read (fp, &v, VSAM);
  /*printf("vnum %d vtk %d vpk %ld vpad1 %ld\n",v.vnum,v.vtk,v.vpk,v.vpad1);
     exit(0); */
  size = v.vrsz[0] + PRNREC + APUNS * (int) v.vtk;
  buf = (char *) calloc (size, 1);
  dat = buf + PRNREC + APUNS * v.vtk;
  ap = buf + PRNREC;
  for (i = 0; i < v.vtk; i++)
    {
      if (v.strk[i].rep == 0)
	k = i;
    }
  lug = v.vpk;
  while (lug != 0l)
    {
      lseek (fp, lug, SEEK_SET);
      read (fp, buf, size);
      if (mues)
	{
	  fprintf (stdout, "%10ld ", lug);
	  for (i = 0; i < v.vtk; i++)
	    {
	      fwrite (dat + v.strk[i].kstarts, v.strk[i].ksz, 1, stdout);
	      putchar (' ');
	    }
	  putchar ('\n');
	}
      rec++;
      lug = ap[k].proxi;
    }
  /*printf("lug %ld vrsz %d vtk %d\n",lug,v.vrsz[0],v.vtk); */
  fprintf (stdout, "Total Number of records: %ld (%s)\n", rec, argv[1]);
  close (fp);
}
