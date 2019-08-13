/* vpsort.c = program to sort input of any size
   in any quantity
   by Franz J Fortuny. (Started on June 30, 1989).
   (Last modification: July 12, 1989)
   (Error at end of long sort and merge, corrected on: )
   Revised for Watcom C QNX 4.2 and IBM AS/400 8 Jan 1995
   Revisions made on April 1997 for the Linux Debian OS
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define OPSIZ 534
#include "vsamstr.h"

void end (char *,...);
void inskey ();
void vsamread (int , char *);
void getmem ();
struct key
  {
    char *keyval, fl;
    long place;
    struct key *menor, *mayor;
  }
 *strus, *vthis, *tocomp, *nextstrus, *anter, **ars;

struct params
  {
    char fix, eor, ord[15], skipfirst, kact, typ[15], otyp[15], *form[15],
     *oform[15];
    int size, fils, starts[15], exten[15], ofils, ost[15], oext[15], vsize;
  }
p;

struct vsam vs;
struct apuns *ap[5];

int fdi[25], fi, e, maxim, cars, fsh, keysize, totemp, lev, tosize, pass;
unsigned tokeys, donekeys, max_mem;
char  *inbuf, *keys, *nextkey, nooutput, *indname, newfp[25],
  destand, vindex, Vsam, *dat, Outf, ll, *kactval, sbuf[256];
long pos, *lg, allkeys;
FILE *fs, *fo, *fps[50], *tmpf, *formafp (), *merge ();

void main (int argc, char *argv[])
{
  FILE *getfil ();
  int i, a, filopen = 0;
  char *ptr;
  time_t tbegin, tend, tdif;
  fs = NULL;
  maxim = 0;
  keysize = 0;
  lev = 0;
  allkeys = 0l;
  p.fix = 0;
  p.eor = '\n';
  destand = 0;
  pass = 0;
  p.size = 0;
  p.fils = 0;
  p.starts[0] = 0;
  p.exten[0] = 12;
  p.skipfirst = 0;
  vindex = 0;
  Vsam = 0;
  donekeys = 0;
  nooutput = 0;
  Outf = 0;
  ll = 0;
  fsh = 0;			/* flushing out */
  max_mem = 0xfffff;
/* if(argc==0) { shell("/cmds/more /expl/vpsort"); exit(0); } */
  for (i = 1; i < argc; i++)
    {
      if (*(ptr = argv[i]) == '+')
	{
	  ++ptr;
	  switch (*ptr)
	    {
	    case 'f':		/* record is fixed size */
	      p.fix = 1;
	      ++ptr;
	      p.size = atoi (ptr);
	      tosize = p.size;
	      break;
	    case 'e':		/* eor */
	      ++ptr;
	      p.eor = atoi (ptr);
	      break;
	    case 'S':		/* skip */
	      p.skipfirst = 1;
	      break;
	    case 'i':		/* leave index at end */
	      vindex = 1;
	      ++ptr;
	      if (*ptr == 'n')
		nooutput = 1;
	      ++ptr;
	      indname = ptr;
	      break;
	    case 'm':		/* maximum memory to request from system */
	      max_mem = atoi (++ptr);
	      break;
	    case 'v':		/* file(s) is(are) vsam vpg files */
	      Vsam = 1;
	      ++ptr;
	      p.kact = atoi (ptr);
	      p.fix = 1;
	      ++ptr;
	      ++ptr;
	      kactval = ptr;
	      break;
	    case 'O':		/* output fields */
	      Outf = 1;
	      ++ptr;
	      for (e = 0; *ptr != 0; e++)
		{
		  p.ost[e] = atoi (ptr);
		  while (*ptr++ != ':');
		  if (!isdigit (*ptr))
		    {
		      p.otyp[e] = *ptr;
		      ++ptr;
		      p.oform[e] = ptr;
		      ++ptr;
		      ++ptr;
		      dat = ptr;
		    }
		  else
		    p.oext[e] = atoi (ptr);
		  while (*ptr++ != ',')
		    if (*ptr == 0)
		      break;
		  if (p.otyp[e] != 0)
		    zfin (dat);
		}
	      p.ofils = e;
	      break;
	    case 'F':		/* fields */
	      ++ptr;
	      for (e = 0; *ptr != 0; e++)
		{
		  p.starts[e] = atoi (ptr);
		  p.typ[e] = 0;
		  while (*ptr++ != ':');
		  if (*(ptr - 2) == 'd')
		    p.ord[e] = 1;
		  else
		    p.ord[e] = 0;
		  if (!isdigit (*ptr))
		    {
		      p.typ[e] = *ptr;
		      ++ptr;
		      p.form[e] = ptr;
		      p.exten[e] = atoi (ptr + 1);
		    }
		  else
		    p.exten[e] = atoi (ptr);
		  a = p.starts[e] + p.exten[e];
		  if (a > maxim)
		    maxim = a;
		  keysize += p.exten[e];
		  while (*ptr++ != ',')
		    if (*ptr == 0)
		      break;
		  if (p.typ[e] != 0)
		    zfin (p.form[e]);
		}
	      p.fils = e;
	      break;
	    case 'l':
	      ll = 1;
	      break;
	    }
	}
      else
	++filopen;
    }
  if (keysize == 0)
    {
      keysize = 10;
      maxim = 10;
      p.fils = 1;
      p.starts[0] = 0;
      p.exten[0] = 10;
    }
  ++keysize;
  e = 0;
  if (p.size != 0)
    maxim = p.size;
  if (Vsam)
    maxim = maxim + PRNREC + (APUNS * 5);
  ptr = (char *) calloc (filopen + 4, OPSIZ);
  if ((inbuf = (char *) calloc (maxim + 10, sizeof (char))) == NULL)
      end ("Can't obtain %d bytes for input inbuffer\n", maxim + 10);
  lg = (long *) (inbuf + sizeof (long));
  getmem ();
  free (ptr);
  tocomp = strus;
  vthis = strus;
  nextstrus = strus;
  nextkey = keys;
  time (&tbegin);
  for (i = 1; i < argc; i++)
    {
      if (*(argv[i]) == '+')
	continue;
      if (!p.fix)
	{
	  if ((fs = fopen (argv[i], "r")) == NULL)
	    end ("Can't open file '%s'.\n", argv[i]);
	  fps[e++] = fs;
	}
      else
	{
	  if ((fi = open (argv[i], O_RDONLY)) == -1)
	    end ("Can't open file '%s' (locker)\n", argv[i]);
	  if (Vsam)
	    vsamread (fi, argv[i]);
	  else
	    dat = inbuf;
	  fdi[e++] = fi;
	}
      if (p.fix)
	do_sortf (fi);
      else
	do_sortv (fs);
    }
  if (e == 0)
    {
      fo = getfil ();
      fs = stdin;
      destand = 1;
      do_sortv (fs);
      fclose (fo);
      sprintf (newfp, "vin%x", getpid ());
      if ((fps[e++] = fopen (newfp, "r")) == NULL)
	end ("Can't reopen file %s\n", newfp);
    }
  inform ();
  if (fsh == 1 || vindex == 1)
    {
      i = fsh;
      fsh = 1;
      tmpf = formafp (totemp++, "w");
      outsorteda ();
      fclose (tmpf);
      if (totemp > 1)
	fs = merge ();
      if (!nooutput)
	outmerge (fs);
      fclose (fs);
      destand = 0;
      fsh = i;
      if (fsh == 1 && vindex == 1)
	{
	  fs = fopen (indname, "w");
	  fclose (fs);
	  sprintf (inbuf, "mv vs%04d%x %s",
		   599 + totemp - 1, getpid (), indname);
	  system (inbuf);
	}
      else if (vindex == 1)
	{
	  fs = fopen (indname, "w");
	  fclose (fs);
	  sprintf (inbuf, "mv vs0000%x %s", getpid (), indname);
	  system (inbuf);
	}
      fsh = 0;
    }
  else
    {
      fprintf (stderr, "Generating output file...\n");
      outsorteda ();
    }
  if (destand)
    {
      fclose (fps[0]);
      system ("rm vin*");
    }
  time (&tend);
  tdif = tend - tbegin;
  fflush (stdout);
  fprintf (stderr, "Time taken: %.8s\n", 11 + ctime (&tdif));
}				/* end of vpsort main */

formkey ()			/* form the key from the inbuffer data */
{
  int i;
  char *ptr;
  char *dobtost ();
  vthis = nextstrus;
  ++nextstrus;
  vthis->keyval = nextkey;
  vthis->fl = e - 1;
  vthis->place = pos;
  ptr = vthis->keyval;
  vthis->menor = 0;
  vthis->mayor = 0;
  for (i = 0; i < p.fils; i++)
    {
      if (p.typ[i] != 0)
	memcpy (ptr, dobtost (inbuf + p.starts[i], i), p.exten[i]);
      else
	memcpy (ptr, inbuf + p.starts[i], p.exten[i]);
      if (p.ord[i])
	descend (ptr, p.exten[i]);
      ptr = ptr + p.exten[i];
    }
  *ptr++ = 0;
  nextkey = ptr;
  inskey ();
  if (++donekeys == tokeys)
    {
      fsh = 1;
      inform ();
      ++pass;
      tmpf = formafp (totemp++, "w");
      outsorteda ();
      fclose (tmpf);
      clearall ();
      nextkey = keys;
      tocomp = strus;
      vthis = strus;
      nextstrus = strus;
      donekeys = 0;
    }
}				/* formkey */

void
inskey ()			/* get pointers of keys */
{
   int r;
  tocomp = strus;
  if (vthis == tocomp)
    {
      return;
    }
  while (tocomp != 0)
    {
      anter = tocomp;
      if ((r = strncmp (vthis->keyval, tocomp->keyval, keysize)) < 0)
	{
	  tocomp = tocomp->menor;
	  continue;
	}
      tocomp = tocomp->mayor;
    }
  if (r < 0)
    anter->menor = vthis;
  else
    anter->mayor = vthis;
}				/* inskey */

do_sortf (fi)
     int fi;
{
  if (p.skipfirst && !Vsam)
    read (fi, inbuf, p.size);
  while (vfread (fi) != 0)
    {
      if (p.skipfirst && !Vsam)
	if (*lg == -1l)
	  continue;
      formkey ();
      if (Vsam)
	pos = ap[p.kact]->proxi;
    }
}

do_sortv (fs)
     FILE *fs;
{
  pos = 0l;
  while (bringcars (fs) != -1)
    {
      formkey ();
      pos = ftell (fs);
    }
}

bringcars (fs)
     FILE *fs;			/* get chars. that belong to key */
{
   int c, i;
   char *ptr;
  ptr = inbuf;
  i = 0;
  while ((c = getc (fs)) != p.eor)
    {
      if (c == EOF)
	return (-1);
      if (i < maxim)
	{
	  *ptr++ = c;
	  i++;
	}
      if (destand)
	putc (c, fo);
    }
  if (destand)
    putc (c, fo);
  *ptr = 0;
  return (i);
}

#define PK putout(vthis)
/* outsortedd()
   {
   vthis = strus;
   while(vthis->menor != 0) vthis = vthis->menor;
   anter = vthis;
   vthis = strus;
   for(;;) {
   while(vthis->mayor != 0) {
   ars[lev++]=vthis;
   vthis = vthis->mayor;
   }
   PK;
   if(vthis == anter) return(0);
   while(vthis->menor == 0) {
   vthis = ars[--lev];
   PK;
   if(vthis == anter) return(0);
   }
   vthis = vthis->menor;
   ars[lev] = vthis;
   }
   }  */
outsorteda ()
{
  vthis = strus;
  while (vthis->mayor != 0)
    vthis = vthis->mayor;
  anter = vthis;
  vthis = strus;
  for (;;)
    {
      while (vthis->menor != 0)
	{
	  ars[lev++] = vthis;
	  vthis = vthis->menor;
	}
      PK;
      if (vthis == anter)
	return (0);
      while (vthis->mayor == 0)
	{
	  vthis = ars[--lev];
	  PK;
	  if (vthis == anter)
	    return (0);
	}
      vthis = vthis->mayor;
      ars[lev] = vthis;
    }
}

putout (s)
     struct key *s;		/* place the key or the record out */
{
  static char prim = 0;
  if (fsh)
    {				/* sorted to temporary file for later merge */
      fwrite (&s->fl, sizeof (char) + sizeof (long), 1, tmpf);
      fwrite (s->keyval, keysize, 1, tmpf);
    }
  else
    {				/* actual sorted record to be placed out */
      if (p.fix)
	{
	  lseek (fdi[s->fl], s->place, 0);
	  if (!prim)
	    {
	      prim = 1;
	      if (p.skipfirst)
		{
		  memset (inbuf, 0, p.size);
		  fwrite (inbuf, p.size, 1, stdout);
		}
	    }
	  read (fdi[s->fl], inbuf, tosize);
	  if (!Outf)
	    fwrite (dat, p.size, 1, stdout);
	  else
	    formout (dat);
	}
      else
	{
	  fseek (fps[s->fl], s->place, 0);
	  outrec (s);
	}
    }
}

outrec (s)
     struct key *s;
{
   int c;
  FILE *fp;
  fp = fps[s->fl];
  while ((c = getc (fp)) != p.eor)
    {
      putchar (c);
    }
  putchar ('\n');
}

FILE * getfil () { FILE *fp;  int c; sprintf (newfp, "vin%x",
getpid ()); if ((fp = fopen (newfp, "w")) == NULL) end ("Can't create
temporary input file %s.\n", newfp); return (fp); }

FILE *
formafp (n, md)
     int n;
     char *md;
{
  FILE *fp;
  sprintf (newfp, "vs%04d%x", n, getpid ());
  if ((fp = fopen (newfp, md)) == NULL)
    end ("Can't open temporary file %s.\n", newfp);
  return (fp);
}

long *loc;
char *filsor;
char *thekey;
int takin;
FILE *
merge ()
{
  int i;
  FILE *f1, *f2, *ff;
  clearall ();
  takin = sizeof (char) + sizeof (long) + keysize;
  filsor = (char *) inbuf;
  loc = (long *) (inbuf + sizeof (char));
  thekey = inbuf + sizeof (char) + sizeof (long);
  nextstrus = strus;
  nextkey = keys;
  ars[0] = nextstrus++;
  ars[1] = nextstrus;
  ars[0]->keyval = nextkey;
  nextkey = nextkey + keysize;
  ars[1]->keyval = nextkey;
  i = 0;
  f1 = formafp (0, "r");
  for (i = 1; i < totemp; i++)
    {
      f2 = formafp (i, "r");
      ff = formafp (599 + i, "w");
      fprintf (stderr, "Merging files %d and %d into %d\n", i - 1, i, 599 + i);
      passmerge (ff, f1, f2);
      fclose (ff);
      fclose (f1);
      fclose (f2);
      if (i > 1)
	{
	  f1 = formafp (599 + i - 1, "w");
	  fclose (f1);
	}
      f1 = formafp (i - 1, "w");
      fclose (f1);
      f1 = formafp (599 + i, "r");
    }
  f2 = formafp (i - 1, "w");
  fclose (f2);
  return (f1);
}				/* merge */

passmerge (ff, f1, f2)
     FILE *ff, *f1, *f2;
{
  getkey (f1, 0);
  getkey (f2, 1);
  for (;;)
    {
      if (comp ())
	{
	  putmerge (ff, 1);
	  if (getkey (f2, 1) == -1)
	    {
	      putmerge (ff, 0);
	      addtomerge (ff, f1);
	      break;
	    }
	  continue;
	}
      else
	{
	  putmerge (ff, 0);
	  if (getkey (f1, 0) == -1)
	    {
	      putmerge (ff, 1);
	      addtomerge (ff, f2);
	      break;
	    }
	  continue;
	}
    }
}				/* passmerge */

addtomerge (FILE * ff, FILE * fp)
{
  while (fread (inbuf, takin, 1, fp) != 0)
    fwrite (inbuf, takin, 1, ff);
}				/* addtomerge */

putmerge (fp, n)
     FILE *fp;
     int n;
{
  fwrite (&ars[n]->fl, sizeof (char) + sizeof (long), 1, fp);
  fwrite (ars[n]->keyval, keysize, 1, fp);
}				/* putmerge */

comp ()
{
  int r;
  r = strncmp (ars[0]->keyval, ars[1]->keyval, keysize);
  if (r > 0)
    return (1);
  else
    return (0);
  /* if(p.ord) {
     if(r <= 0) return(1);
     }
     else {
     if(r > 0) return(1);
     }
     return(0); */
}				/* comp() */

getkey (FILE * fp, int n)
{
  if (fread (inbuf, sizeof (char) + sizeof (long) + keysize, 1, fp) == 0)
      return (-1);
  ars[n]->fl = *filsor;
  ars[n]->place = *loc;
  memcpy (ars[n]->keyval, thekey, keysize);
  return (0);
}				/* getkey */

outmerge (fs)
     FILE *fs;
{
  fprintf (stderr, "Generating output file...\n");
  fsh = 0;
  while (fread (inbuf, takin, 1, fs) != 0)
    {
      ars[0]->fl = *filsor;
      ars[0]->place = *loc;
      putout (ars[0]);
    }
  fclose (fs);
  if (!vindex)
    {
      sprintf (inbuf, "rm vs*%x", getpid ());
      system (inbuf);
    }
}

void getmem ()
{
  unsigned int tomem, perkey;
  char *ptr;
  perkey = sizeof (struct key) + keysize + sizeof (int);
  tomem = max_mem;
  if ((ptr = (char *) calloc (tomem, 1)) == NULL)
    end ("System is not giving %d bytes of memory to work with. Try less.",
	 max_mem);
/* ptr = calloc(1,1); free(ptr);
   tomem = ptr; tomem = 0xffff - tomem;
   for(;(ptr=calloc(tomem,sizeof(char))) == NULL;--tomem) ; */
  tokeys = tomem / perkey;
  --tokeys;
  fprintf (stderr, "Usable Memory: (from %u) %u bytes (%u keys)\n", 
	   ptr, tomem, tokeys);
  fflush (stderr);
  free (ptr);
  strus = (struct key *) calloc (tokeys, sizeof (struct key));
  ars = (struct key **) calloc (tokeys, sizeof (int));
  keys = (char *) calloc (tokeys, keysize);
  if (strus == NULL || keys == NULL || ars == NULL)
    end ("Structures (%u) or Keys (%u) or Levels (%u)\nhave no memory.\n",
	 strus, keys, ars);
}

clearall ()
{
  memset (strus, 0, tokeys * sizeof (struct key));
  memset (keys, 0, tokeys * keysize);
  memset (ars, 0, tokeys * sizeof (int));
}

inform ()
{
  if (fsh)
    allkeys = allkeys + donekeys;
  else
    allkeys = donekeys;
  fprintf (stderr, "Keys done: %ld (file: %d pass %d)\n", allkeys, e, pass);
}

vfread (fi)
     int fi;
{
  if (!Vsam)
    {
      pos = lseek (fi, 0l, 1);
      return (read (fi, inbuf, p.size));
    }
  else
    {
      if (pos == 0l)
	return (0);
      lseek (fi, pos, 0);
      return (read (fi, inbuf, p.vsize));
    }
}

void vsamread (int fd, char *s)
{
  int vset, i;
  long busca ();
  lseek (fd, 0l, 0);
  if (read (fd, (char*) &vs, VSAM) != VSAM)
    {
      end ("Indexed file header is probably corrupt!\n");
    }
  if (vs.vnum != 0x1c02)
    end ("File %s is not a VPG indexed file!\n", s);
  vset = PRNREC + (vs.vtk * APUNS);
  p.size = vs.vrsz[0];
  p.vsize = p.size + vset;
  p.fix = 1;
  dat = inbuf + vset;
  for (i = 0; i < vs.vtk; i++)
    {
      ap[i] = (struct apuns *) (inbuf + PRNREC + (i * APUNS));
    }
  for (i = 0; i < p.fils; i++)
    {
      p.starts[i] += vset;
    }
  tosize = p.vsize;
  if (maxim < tosize)
    end ("Input buffer is too small! Specify size with +fSSS.\n");
  if (vs.strk[p.kact].rep == 0)
    pos = vs.vpk;
  else
    {
      if ((pos = busca (fd, kactval, p.kact, &vs)) == -1)
	end ("No records found with key '%s'!\n", kactval);
    }
}

formout (s)
     char *s;
{
  static long lines = 0;
  int i, *e;
  char *b, *ctime ();
  long *l;
  float *f;
  double *d;
  if (ll)
    printf ("%6ld ", ++lines);
  for (i = 0; i < p.ofils; i++)
    {
      if (p.otyp[i] == 0)
	fwrite (s + p.ost[i], p.oext[i], 1, stdout);
      else
	{
#define SPO (s+p.ost[i])
	  switch (p.otyp[i])
	    {
	    case 'd':
	      printf (p.oform[i], *(double *) SPO);
	      break;
	    case 'i':
	      printf (p.oform[i], *(short *) SPO);
	      break;
	    case 'e':
	      printf (p.oform[i], *(int *) SPO);
	      break;
	    case 'l':
	      printf (p.oform[i], *(long *) SPO);
	      break;
	    case 'f':
	      printf (p.oform[i], *(float *) SPO);
	      break;
	    case 'c':
	      printf (p.oform[i], *(char *) SPO);
	      break;
	    case 'u':
	      printf (p.oform[i], *(unsigned *) SPO);
	      break;
	    case 'A':
	      printf ("%24.24s", ctime ((long *) SPO));
	      break;
	    case 'B':
	      printf ("%6.6s %4.4s",
		      4 + ctime ((long *) SPO), 20 + ctime ((long *) SPO));
	    case 'C':
	      printf ("%8.8s", 8 + ctime ((long *) SPO));
	    case 'D':
	      printf ("%5.5s", 8 + ctime ((long *) SPO));
	    }
	}
      putchar (' ');
    }
  putchar ('\n');
}

char *
dobtost (b, n)
     char *b;
     int n;
{
  static char s[40];
  int *i;
  long *l;
  float *f;
  double *d;
  switch (p.typ[n])
    {
    case 'd':
      sprintf (s, p.form[n], *(double *) b);
      break;
    case 'i':
      sprintf (s, p.form[n], *(short *) b);
      break;
    case 'e':
      sprintf (s, p.form[n], *(int *) b);
      break;
    case 'l':
      sprintf (s, p.form[n], *(long *) b);
      break;
    case 'f':
      sprintf (s, p.form[n], *(float *) b);
      break;
    case 'c':
      sprintf (s, p.form[n], *b);
      break;
    case 'u':
      sprintf (s, p.form[n], *(unsigned *) b);
      break;
    }
  return (s);
}

zfin (s)
     char *s;
{
   char *ptr;
  ptr = s;
  while (*ptr++ != 0)
    {
      if (*ptr == ',')
	{
	  *ptr = 0;
	  break;
	}
    }
}

long
busca (fd, key, lla, vs)
     int lla, fd;
     char key[];
     struct vsam *vs;
#define CAM vs->strk[lla].ksz
#define BUFER esp+KLUG+(i*(CAM+8))
#define COM strncmp(key,BUFER,CAM)
{
   int i, b;
  char esp[544 + KLUG];
  long *nm[5], *may[5], rec, recin;
  struct klug *loc;
  rec = vs->strk[lla].kpos;
  loc = (struct klug *) esp;
  for (i = 1; i < 5; i++)
    {
      nm[i] = (long *) (esp + KLUG + (CAM + 8) * i - 8);
      may[i] = (long *) (esp + KLUG + (CAM + 8) * i - 4);
    }
  i = b = 0;
  recin = 0l;
  while (rec != 0l)
    {
      lseek (fd, rec, 0);
      if ((read (fd, esp, KLUG + (CAM + 8) * 4)) != (KLUG + (CAM + 8) * 4))
	{
	  printf ("Read error from cluster %ld\n", rec);
	  exit (0);
	}
      recin = rec;
      if (loc->nkeys == 0)
	{
	  return (-1l);
	}
      rec = 0l;
      for (i = 0;
	   (i < loc->nkeys) && ((b = COM) >= 0);
	   i++)
	if (b == 0)
	  {
	    return (*nm[i + 1]);
	  }
      rec = (i == 0) ? loc->nantl : *may[i];
    }
  return (-1l);
}

descend (ptr, n)
     char *ptr;
     int n;
{
   int i;
   char c;
  for (i = 0; i < n; i++)
    {
      c = *ptr;
      *ptr++ = ~c;
    }
}

/* rutina de error */
#include <stdarg.h>

void
end (char *format,...)
{
  va_list arglist;

  fprintf (stderr, "Problem:\n");

  va_start (arglist, format);

  vfprintf (stderr, format, arglist);
  fflush (stderr);

  va_end (arglist);

  exit (1);
}
