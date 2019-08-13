/* Version 7.50 - December 18 1986 QNX Multi-User */
/* February 4 1987 - 256 changed to 512 on variable sur[] */
/* Jan 7 1988 - instr. = added */
/* Sept. 10 1988 - instructions referred to by number */
/* March 9 1989 - getad modified to use buf:pos */
/* March 29 1989 - Message passing instructions added */
/* modified for Watcom C QNX 32 bit 4.2 Jan 16 1995 */
/* Modified to use the ncurses library of screen routines april 1997 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <curses.h>
#include <signal.h>
#include "vsamstr.h"
#include "archi.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "texfi.h"
#include <errno.h>
/*#include <sys/dev.h> */
#define BEL beep()
#define MP while(*ptr++ != ',')
#define REFR if(vido) wrefresh(win[vwa])
void err (char *,...);
void vattr (char *, int);
ARCHI *ar;
extern long mayor, menor;

char stop, usf = 1, vido = 0, reb;
void *Memory;
int kivo, krow[256], kcol[256], numin, datin, MAXLOC, ARFILES, ins;
int dk[0x256], fk[0x256], surk[256], lok, newp, tzgo[256], tbgo[256], seq,
  tin;
unsigned attr, attr1;
struct posics
  {
    char row, col;
  }
 *local;
int vwa = 0, vrow, vcol;	/* active window */
WINDOW *win[25];		/* array of pointers to windows */
/*struct _dev_info_entry dvinfo; */
#define vls 0
#define vrs 1
#define vts 2
#define vbs 3
#define vtl 4
#define vtr 5
#define vbl 6
#define vbr 7
char *dy, *mt;
int sH, sW;
void
main (int argc, char *argv[])
{
  char *ptr, **lab, *prog;
  char s[256], *cc, finar = 0, typ, *ptr1, *desti, typ1, caps = 0;
  void *getad (), *getad1 ();
  void nada (), vunlock (), putval ();
  unsigned boxcar[] =
  {'|', '|', '-', '-', '+', '+', '+', '+'};
  int i, fa, aa = 0, r = 0, start, forma, largo, ae, ae1, *ii, nivo = 0,
    ffaa = 1, handle (), toins;
  int sur[256], inp = 0, paro, regder, qptr = 0;
  int vinitcurses = 0;
  int bgy, bgx, nlines, ncols;
  short *ss;
  unsigned tid;
  extern int s_n;
  long sz, *l, busca (), atol (), inclu (), vdadd (), tidis (), futdate (),
    pasdate (), totsec ();
  double dl, *dd, calcd ();
  float fl, *ff;
  REPOR tex[10];
  FILE *fgh, *filins;
  ARFILES = 40;
  MAXLOC = 100;
  ar = calloc (ARFILES, sizeof (ARCHI));
  local = calloc (MAXLOC, sizeof (struct posics));
  strcpy (s, argv[1]);
  Memory = s;
  for (i = 0; i < 10; i++)
    {
      tex[i].x_sit = 0;
      tex[i].x_fp = NULL;
      tex[i].x_tlp = 0;
    }
  tex[1].x_fp = stdout;
  tex[2].x_fp = stderr;
  tex[0].x_fp = stdin;
  tex[0].x_sit = 2;
  signal (SIGINT, &handle);	/*Cambiado en misce.c 950114 */
  /* dev_info (0, &dvinfo); */
  dy = "DomLunMarMieJueVieSab";
  mt = "EneFebMzoAbrMayJunJulAgoSepOctNovDic";
otvpg:ffaa = 1;
  tin = 1;
  tex[0].x_s = calloc (256, 1);
  if ((fgh = fopen (s, "rb")) == NULL)
    err ("File %s can't be opened", s);
  fseek (fgh, 0l, SEEK_END);
  sz = ftell (fgh);
  rewind (fgh);
  if ((prog = calloc ((int) sz, 1)) == NULL)
    err ("Can't get %d bytes of memory", (int) sz);
  fread (prog, (int) sz, 1, fgh);
  fclose (fgh);
  toins = *(int *) prog;
  lab = (void *) (prog + sizeof (int));
  for (i = 0; i < toins; i++)
    lab[i] = lab[i] + (unsigned int) prog;
  for (i = 0; i < 256; i++)
    {
      dk[i] = 0;
      surk[i] = 0;
      fk[i] = 0;
    }
  ptr = lab[0];
  paro = toins - 2;
  kivo = 0;
  newp = 0;
  ins = 0;
e1:while (*(ptr = lab[ins]) != '}')
    {
      if (stop)
	{
	  stop = 0;
	  ins = paro;
	  goto e1;
	}
      switch (*ptr)
	{
	case 'g':		/* goto seguido de expresion numerica letrero = NN; */
	  ins = *(int *) (ptr + 1);
	  goto e1;
	case '(':		/* comienza operacion de asignacion */
	  typ = *(++ptr);
	  desti = getad (ptr);
	  while (*ptr++ != '=');
	  switch (typ)
	    {
	    case 'd':
	      *(double *) desti = calcd (ptr);
	      break;
	    case 'i':
	      *(short *) desti = (short) calcd (ptr);
	      break;
	    case 'D':
	    case 'l':
	      *(long *) desti = (long) calcd (ptr);
	      break;
	    case 'f':
	      *(float *) desti = (float) calcd (ptr);
	      break;
	    case 'c':
	      *(char *) desti = (char) calcd (ptr);
	      break;
	    case 's':
	      ++ptr;
	      if (*ptr == '$')
		ptr1 = ptr + 1;
	      else
		{
		  ptr1 = getad (ptr);
		}
	      while (*ptr++ != ',');
	      switch (*ptr)
		{
		case 'i':
		  i = *(short *) getad (ptr);
		  break;
		case 'l':
		  i = strlen (ptr1);
		  break;
		default:
		  i = atoi (ptr);
		  break;
		}
	      while (i-- != 0)
		*(desti++) = *(ptr1++);
	      break;
	    }
	  break;
	case '@':		/* round to amount */
	  ++ptr;
	  dd = (double *) getad (ptr);
	  while (*ptr++ != ',');
	  dl = atof (ptr);
	  if (*dd > 0.0)
	    *dd = *dd * dl + .5;
	  else
	    *dd = *dd * dl - .5;
	  modf (*dd, dd);
	  *dd = *dd / dl;
	  break;
	case '!':		/* Operacion de comparacion !nn,-1(tb,NNN)=<>!(tb,NNN); */
	  if (cmp (ptr))
	    {
	      if (*(lab[ins + 1] - sizeof (int) - 1) == '<')
		  sur[nivo++] = ins + 2;
	      ins = *(int *) (lab[ins + 1] - sizeof (int));
	      goto e1;
	    }
	  break;
	case 'r':		/* Leer al buffer la informacion del archivo */
	  ++ptr;
	  forma = vdreadl (aa);
	  if (forma <= 0)
	    {
	      ins = *(int *) ptr;
	      goto e1;
	    }
	  if (forma == 1)
	    {
	      ins = qptr;
	      goto e1;
	    }
	  break;
	case 'N':		/* Leer al buffer - forzar lock */
	  ++ptr;
	  forma = vdlread (aa);
	  if (forma <= 0)
	    {
	      ins = *(int *) ptr;
	      goto e1;
	    }
	  break;
	case 'n':		/* Leer al buffer sin lock */
	  ++ptr;
	  forma = vdread (aa);
	  if (forma <= 0)
	    {
	      ins = *(int *) ptr;
	      goto e1;
	    }
	  break;
	case 'w':		/* escribir el buffer al archivo */
	  vdwrite (aa);
	  break;
	case 'a':		/* avanzar el archivo a siguiente posicion */
	  seq = 1;
	  if (ar[aa].a_act != 0)
	    if (ar[aa].a_lug != 0l)
	      vunlock (aa);
	  if (ar[aa].a_type)
	    ar[aa].a_lug = *(long *) (ar[aa].a_buf + PRNREC + APUNS * ar[aa].a_k + 4);
	  else
	    ar[aa].a_lug += (long) ar[aa].a_sz;
	  break;
	case 'b':		/* avanzar el archivo a posicion anterior backwards */
	  if (ar[aa].a_act != 0)
	    if (ar[aa].a_lug != 0l)
	      vunlock (aa);
	  if (ar[aa].a_type)
	    ar[aa].a_lug = *(long *) (ar[aa].a_buf + PRNREC + APUNS * ar[aa].a_k);
	  else
	    ar[aa].a_lug -= (long) ar[aa].a_sz;
	  break;
	case 'A':		/* avanzar el archivo de acuerdo a otro campo en otro ar */
	  seq = 0;
	  if (ar[aa].a_act != 0)
	    if (ar[aa].a_lug != 0l)
	      vunlock (aa);
	  ar[aa].a_lug = *(long *) getad (++ptr);
	  break;
	case 'd':		/* determine position of active file and keep it in memory */
	  *(long *) getad (++ptr) = ar[aa].a_lug;
	  break;
	case 'P':		/* PICK posicion de una llave */
	case 'S':		/* Search for a key = or similar */
	  typ = *ptr;
	  if (*(++ptr) == '$')
	    desti = ptr + 1;
	  else
	    {
	      desti = getad (ptr);
	    }
	  if (ar[aa].a_act != 0)
	    if (ar[aa].a_lug != 0l)
	      vunlock (aa);
	  lseek (ar[aa].a_fd, 0l, 0);
	  read (ar[aa].a_fd, ar[aa].a_vsam, VSAM);
	  ar[aa].a_lug = busca (ar[aa].a_fd, desti, ar[aa].a_k, ar[aa].a_vsam);
	  if (typ == 'S')
	    if (ar[aa].a_lug < 0l)
	      ar[aa].a_lug = (menor == 0l) ? mayor : menor;
	  break;
	case 'f':		/* determinar archivo */
	  aa = atoi (ptr + 1);
	  break;
	case 'p':		/* imprimir un campo determinado = p[silfdc]NNN,NNN/%f */
	  typ = *(++ptr);
	  ptr1 = getad (ptr);
	  while (*ptr++ != ',');
	  if (typ == 's')
	    forma = valu (ptr);
	  else
	    forma = 0;
	  impf (tex[ffaa].x_fp, typ, ptr1, forma, ptr);
	  REFR;
	  break;
	case 'k':		/* Fijar el tipo de llave para accesos */
	  ar[aa].a_k = *(++ptr) - '0';
	  break;
	case 's':		/* dar ene espacios */
	  produ (' ', tex[ffaa].x_fp, valu (++ptr));
	  REFR;
	  break;
	case 'l':		/* avance de linea en reporte */
	  if (vido)
	    {
	      vrow++;
	      vcol = 0;
	      if (vrow == sH)
		{
		  scroll (win[vwa]);
		  vrow = sH - 1;
		  REFR;
		}
	    }
	  else
	    {
	      putc ('\n', tex[ffaa].x_fp);
	      fflush (tex[ffaa].x_fp);
	    }
	  if (tex[ffaa].x_sit == 1)
	    {
	      tex[ffaa].x_tlp++;
	      if (tex[ffaa].x_tlp == tex[ffaa].x_fl)
		{
		  regder = ins + 1;
		  ins = tex[ffaa].x_lbf;
		  goto e1;
		fifoo:if (tex[ffaa].x_tlp > tex[ffaa].x_lpp)
		    err ("Instr. %d: report lines incosistent.", ins);
		  while (tex[ffaa].x_tlp != tex[ffaa].x_lpp)
		    {
		      putc ('\n', tex[ffaa].x_fp);
		      tex[ffaa].x_tlp++;
		    }
		  fflush (tex[ffaa].x_fp);
		header:tex[ffaa].x_tlp = 0;
		  tex[ffaa].x_pg++;
		  ins = tex[ffaa].x_lbh;
		  goto e1;
		}
	    }
	  break;
	case 'c':		/* El caracter que le siga imprimirlo n c*20; */
	  ++ptr;
	  typ = *ptr++;
	  produ (typ, tex[ffaa].x_fp, valu (ptr));
	  REFR;
	  break;
	case '`':		/* imprimir un caracter de control */
	  if (vido)
	    break;
	  i = valu (ptr + 1);
	  fprintf (tex[ffaa].x_fp, "%c", i);
	  if (tex[ffaa].x_sit == 1)
	    if (i == 12)
	      {
		regder = ins + 1;
		goto header;
	      }
	  while (*ptr != ';')
	    if (*ptr++ == ',')
	      fprintf (tex[ffaa].x_fp, "%c", valu (ptr));
	  break;
	case '\\':		/* imprimir el string que se contenga hasta otro */
	  ++ptr;
	  i = 0;
	  while ((s[i++] = *ptr++) != '\\');
	  s[i - 1] = 0;
	  if (vido)
	    {
	      vattr (s, strlen (s));
	    }
	  else
	    {
	      fwrite (s, strlen (s), 1, tex[ffaa].x_fp);
	      fflush (tex[ffaa].x_fp);
	    }
	  break;
	case 'I':		/* incluir o agregar un record a un archivo */
	  if (ar[aa].a_lug != 0l)
	    vunlock (aa);
	  ar[aa].a_lug = vdadd (aa);
	  break;
	case 'D':		/* borrar un record de un archivo */
	  vddel (aa);
	  break;
	case 'L':		/* Siguiente caracter operacion con liga de archivos */
	  ++ptr;
	  typ = *ptr;
	  ++ptr;
	  start = (int) getad (ptr);
	  i = atoi (ptr + 1);
	  while (*ptr++ != ',');
	  forma = (int) getad (ptr);
	  ae = atoi (ptr + 1);
	  switch (typ)
	    {
	    case 'N':		/* Ligar Record Nuevo - Agregarlo */
	      vdadd (ae);
	      vdlink (i, ae, start, forma);
	      break;
	    case 'L':		/* Solamente ligar a principal */
	      vdlink (i, ae, start, forma);
	      break;
	    case 'D':		/* desligar y DELETE el ligado */
	      vdunlink (i, ae, start, forma);
	      vddel (ae);
	      break;
	    case 'U':		/* desligar unlinking, sin matar record */
	      vdunlink (i, ae, start, forma);
	      break;
	    }
	  vdwrite (aa);
	  break;
	case '=':		/* igualar buffers */
	  ++ptr;
	  ae = atoi (ptr);
	  if (ar[ae].a_type)
	    err ("Instr %d: 2nd buffer can't belong to VSAM file", ins);
	  if (ar[ae].a_dat != 0)
	    free (ar[ae].a_dat);
	  ar[ae].a_dat = ar[aa].a_dat;
	  ar[ae].a_k = 98;
	  ar[ae].a_buf = ar[ae].a_dat;
	  break;
	case 'z':		/* llenar con caracteres iguales regiones de memoria */
	  ++ptr;
	  typ = valu (ptr);
	  while (*ptr++ != ',');
	  ae = atoi (ptr + 1);
	  ptr1 = getad (ptr);
	  i = (int) (ptr1 - ar[ae].a_dat);
	  while (*ptr++ != ',');
	  if (*ptr == 'l')
	    i = ar[ae].a_sz - i;
	  else
	    i = valu (ptr);
	  while (i-- != 0)
	    *ptr1++ = typ;
	  break;
	case 'm':		/* separacion de memoria para trabajo */
	  i = atoi (ptr + 1);
	  if (ar[aa].a_buf != 0)
	    free (ar[aa].a_buf);
	  if ((ar[aa].a_buf = calloc (i, 1)) == NULL)
	    err ("No memory m buffer instr. %d, buffer %d", ins, aa);
	  ar[aa].a_sz = i;
	  ar[aa].a_type = 0;
	  ar[aa].a_fd = 0;
	  ar[aa].a_k = 99;
	  ar[aa].a_dat = ar[aa].a_buf;
	  break;
	case 'Q':		/* Lock un record decir a donde se va si no se pudo */
	  qptr = *(int *) (++ptr);
	  break;
	case 'u':		/* unlock record of active file previously locked */
	  vunlock (aa);
	  break;
	case 'O':		/* Open o abrir un archivo */
	case 'o':		/* abrir solo para leer */
	  ar[aa].a_act = (*ptr == 'o') ? 0 : 1;
	  ++ptr;
	  switch (*ptr)
	    {
	    case '(':
	      ++ptr;
	      ptr1 = getad (ptr);
	      break;
	    default:
	      ptr1 = ptr;
	      break;
	    }
	  while (*ptr++ != ',');
	  desti = ptr;
	  ar[aa].a_k = 0;
	O1:errno = 0;
	  if (ar[aa].a_act == 0)
	    ar[aa].a_fd = open (ptr1, O_RDONLY);
	  else
	    ar[aa].a_fd = open (ptr1, O_RDWR);
/*#ifdef CREARSI */
	  if (ar[aa].a_fd == -1)
	    {
	      if (errno != ENOENT)
		err ("El archivo NO PUEDE ABRIRSE por causa #: %d  \nAvisar URGENTE!\n  ",
		     errno);
	      if (*ptr != 'v')
		{
		  forma = atoi (ptr + 1);
		  fgh = fopen (ptr1, "wb");
		  if ((ar[aa].a_buf = calloc (forma, 1)) == NULL)
		    err ("No memory for file buffer %d instr. %d", aa, ins);
		  fwrite (ar[aa].a_buf, forma, 1, fgh);
		  fclose (fgh);
		  free (ar[aa].a_buf);
		  ar[aa].a_buf = 0;
		  goto O1;
		}
	      else
		{
		  if (*(ptr + 1) != 'k')
		    err ("Undefined indexed file %d.", aa);
		  forma = (*(ptr + 2) - '0');
#define VT ar[aa].a_vsam
		  if ((VT = calloc (VSAM, 1)) == NULL)
		    err ("No memory for file header %d instr %d", aa, ins);
		  VT->vnum = 7170;
		  while (*ptr++ != ',');
		  VT->vtk = atoi (ptr);
		  while (*ptr++ != ',');
		  VT->vpk = 0l;
		  VT->vpad1 = 0;
		  VT->pad1 = 0l;
		  for (i = 0; i < 4; i++)
		    VT->vrsz[i] = atoi (ptr);
		  /*time(&VT->vhora); */ fa = VSAM;
		  for (i = 0; i < VT->vtk; i++)
		    {
		      while (*ptr++ != ',');
		      VT->strk[i].kstarts = atoi (ptr);
		      while (*ptr++ != ',');
		      VT->strk[i].ksz = atoi (ptr);
		      VT->strk[i].kpos = (long) fa;
		      fa += KLUG + (VT->strk[i].ksz + 8) * 4;
		      if (forma == i)
			VT->strk[i].rep = 0;
		      else
			VT->strk[i].rep = 1;
		    }
		  fgh = fopen (ptr1, "wb");
		  s[0] = 0x1e;
		  for (i = 1; i < 256; i++)
		    s[i] = 0;
		  fwrite (VT, VSAM, 1, fgh);
		  for (i = 0; i < VT->vtk; i++)
		    fwrite (s, KLUG + (VT->strk[i].ksz + 8) * 4, 1, fgh);
		  fclose (fgh);
		  free (VT);
		  ptr = desti;
		  goto O1;
		}
	    }
/* #endif CREARSI */
	  if (ar[aa].a_fd == -1)
	    err ("El archivo %s NO EXISTE", ptr1);
	  if (*ptr == 'v')
	    {
	      if ((ar[aa].a_vsam = calloc (VSAM, 1)) == NULL)
		err ("No memory for index headr of file %d (instr %d)", aa, ins);
	      read (ar[aa].a_fd, ar[aa].a_vsam, VSAM);
	      ar[aa].a_sz = ar[aa].a_vsam->vrsz[0];
	      ar[aa].a_lug = ar[aa].a_vsam->vpk;
	      ar[aa].a_type = 1;
	      if ((ar[aa].a_buf = calloc (PRNREC + APUNS * ar[aa].a_vsam->vtk
					  + ar[aa].a_sz, 1)) == NULL)
		err ("No memory for buffer %d instr. %d", aa, ins);
	      ar[aa].a_dat = ar[aa].a_buf + PRNREC + APUNS * ar[aa].a_vsam->vtk;
	      ar[aa].a_k = 0;
	    }
	  else
	    {
	      ar[aa].a_sz = atoi (++ptr);
	      ar[aa].a_lug = (long) ar[aa].a_sz;
	      ar[aa].a_type = 0;
	      if ((ar[aa].a_buf = calloc (ar[aa].a_sz, 1)) == NULL)
		err ("No memory for buffer %d instr. %d", aa, ins);
	      ar[aa].a_dat = ar[aa].a_buf;
	    }
	  ar[aa].a_max = lseek (ar[aa].a_fd, 0l, 2);
	  break;
	case 'i':		/* input data from the screen */
	  lok = ins;
	  krow[kivo] = vrow;
	  kcol[kivo] = vcol;
	  tzgo[kivo] = inp;
	  tbgo[kivo] = paro;
	  typ = *(++ptr);
	  ptr1 = getad (ptr);
	  while (*ptr++ != ',');
	  if (typ == 's')
	    {
	      forma = valu (ptr);
	      numin = 0;
	      datin = 0;
	      i = takein (ptr1, forma, caps);
	    }
	  else
	    {
	      if (!isdigit (*(ptr + 1)))
		forma = atoi (ptr + 2);
	      else
		forma = atoi (ptr + 1);
	      putval (s, typ, ptr1, ptr);
	      numin = 1;
	      if (typ == 'D')
		{
		  datin = 1;
		  numin = 0;
		}
	      i = takein (s, forma, caps);
	    }
	  if ((i == -1) && (inp != 0))
	    {
	      ins = inp;
	      goto e1;
	    }
	  if (i == 1099)
	    {
	      ins = newp;
	      goto e1;
	    }
	  if (typ == 's')
	    break;
	  s[forma] = 0;
	  if (typ == 'D')
	    {
	      *(long *) ptr1 = totsec (s);
	      break;
	    }
	  while (*ptr++ != ',');
	  switch (typ)
	    {
	    case 'd':
	      *(double *) ptr1 = atof (s);
	      break;
	    case 'i':
	      *(short *) ptr1 = atoi (s);
	      break;
	    case 'l':
	      *(long *) ptr1 = atol (s);
	      break;
	    case 'f':
	      *(float *) ptr1 = (float) atof (s);
	      break;
	    }
	  putval (s, typ, ptr1, ptr);
	  mvaddnstr (krow[kivo], kcol[kivo], s, forma);
	  break;
	case 'G':		/* ir a una subrutina */
	  sur[nivo++] = ins + 1;
	  ins = *(int *) (ptr + 1);
	  goto e1;
	case 'R':		/* Regresar de una Sub-rutina */
	  ins = sur[--nivo];
	  goto e1;
	case ']':		/* Return from Function Sub-routine */
	  --kivo;
	  wmove (win[vwa], krow[kivo], kcol[kivo]);
	  inp = tzgo[kivo];
	  paro = tbgo[kivo];
	  ins = surk[kivo];
	  goto e1;
	case 't':		/* seguido de otro comando para terminal */
	  ++ptr;
	  switch (*ptr)
	    {
	    case 'k':		/* check for characters waiting tkFCE */
	      /*if (dev_ischars (0) != 0)
	         {
	         ins = *(int *) (ptr + 1);
	         goto e1;
	         } */
	      break;
	    case 'w':		/* window scroll */
	      wscrl (win[vwa], 1);
	      REFR;
	      break;
	    case 'S':		/* save image */
	      /* tSb,r,c,l,lg; tS0,12,40,10,20; */
	      ++ptr;
	      vwa = atoi (ptr);
	      if (win[vwa] != NULL)
		{
		  sH = win[vwa]->_maxy + 1;
		  sW = win[vwa]->_maxx + 1;
		  redrawwin (win[vwa]);
		  wrefresh (win[vwa]);
		  vido = 1;
		  break;
		}
	      MP;
	      bgy = atoi (ptr);
	      MP;
	      bgx = atoi (ptr);
	      MP;
	      nlines = atoi (ptr);
	      MP;
	      ncols = atoi (ptr);
	      if ((win[vwa] = newwin (nlines, ncols, bgy, bgx)) == NULL)
		err ("Error creating new window");
	      sH = win[vwa]->_maxy + 1;
	      sW = win[vwa]->_maxx + 1;
	      idlok (win[vwa], TRUE);
	      scrollok (win[vwa], TRUE);
	      idcok (win[vwa], TRUE);
	      clearok (win[vwa], TRUE);
	      vido = 1;
	      break;
	    case 'R':		/* destroy unneccesary window */
	      ++ptr;
	      vwa = atoi (ptr);
	      if (win[vwa] == NULL || vwa == 0)
		break;
	      delwin (win[vwa]);
	      win[vwa] = NULL;
	      vido = 0;
	      break;
	    case 'c':		/* terminal cooked old setckd() */
	      echo ();
	      nocbreak ();
	      nl ();
	      vido = 0;
	      break;
	    case 'r':		/* terminal cruda raw (old setraw() */
	      if (!vinitcurses)
		{
		  vinitcurses = 1;
		  vido = 1;
		  vwa = 0;
		  win[vwa] = initscr ();
		  keypad (stdscr, TRUE);
		  (void) nonl ();	/* tell curses not to do NL->CR/NL on output */
		  (void) cbreak ();	/* take input chars one at a time, no wait for \n */
		  (void) noecho ();	/* don't echo input */
		  sH = win[vwa]->_maxy + 1;
		  sW = win[vwa]->_maxx + 1;
		  idlok (win[vwa], TRUE);
		  scrollok (win[vwa], TRUE);
		  idcok (win[vwa], TRUE);
		  clearok (win[vwa], TRUE);
		  if (has_colors ())
		    {
		      start_color ();
		      init_pair (COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
		      init_pair (COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
		      init_pair (COLOR_RED, COLOR_RED, COLOR_BLACK);
		      init_pair (COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
		      init_pair (COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
		      init_pair (COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
		      init_pair (COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
		      init_pair (COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
		    }
		}
	      else
		{
		  nonl ();
		  cbreak ();
		  noecho ();
		  vido = 1;
		}
	      break;
	    case 'u':		/* upper case or lower case */
	      caps = *(++ptr) - '0';
	      break;
	    case '@':		/* colocar cursor en pantalla */
	      ++ptr;
	      i = valu (ptr);
	      while (*ptr++ != ',');
	      forma = valu (ptr);
	      vrow = i;
	      vcol = forma;
	      wmove (win[vwa], vrow, vcol);
	      break;
	    case 'a':		/* change attribue */
	      ++ptr;
	      switch (*ptr)
		{
		case '0':	/* return to normal */
		  wattrset (win[vwa], A_NORMAL);
		  attr = A_NORMAL;
		  break;
		case '1':	/* inverse */
		  wattrset (win[vwa], A_REVERSE);
		  attr = A_REVERSE;
		  break;
		case '2':	/* underlines */
		  wattrset (win[vwa], A_UNDERLINE);
		  attr = A_UNDERLINE;
		  break;
		case '3':	/* blink */
		  wattrset (win[vwa], A_BLINK);
		  attr = A_BLINK;
		  break;
		case '=':	/* alternate */
		  attr1 = attr;
		  break;
		case 'c':	/* color */
		  ++ptr;
		  wattrset (win[vwa], COLOR_PAIR (valuh (ptr)));
		  break;
		}
	      break;
	    case 'i':		/* insertar linea hacia abajo */
	      winsertln (win[vwa]);
	      break;
	    case 'd':		/* delete line up */
	      wdeleteln (win[vwa]);
	      break;
	    case 'p':		/* escribir cars protegidos */
	      wattrset (win[vwa], A_STANDOUT);
	      attr = A_STANDOUT;
	      break;
	    case 'n':		/* escribir caracteres no protegidos */
	      wattrset (win[vwa], A_NORMAL);
	      attr = A_NORMAL;
	      break;
	    case 'f':		/* borrar la pantalla */
	      wclear (win[vwa]);
	      break;
	    case 'l':		/* borrar hasta final de linea */
	      wclrtoeol (win[vwa]);
	      break;
	    case 'e':		/* borrar hasta final de la pantalla */
	      wclrtobot (win[vwa]);
	      break;
	    case 'H':		/* hacer caso a proteccion */
	      break;
	    case 'I':		/* ignorar la proteccion */
	      break;
	    case 'P':		/* leer el archivo de pantalla indicado */
	      pan (ptr + 1);
	      break;
	    case 'v':		/* video on or off */
	      ++ptr;
	      if (*ptr == '1')
		vido = 1;
	      else
		vido = 0;
	      break;
	    case '0':		/* transfer control to position indicated on 0 */
	      inp = *(int *) (++ptr);
	      break;
	    case 'M':		/* make a box */
/*            ++ptr;
   start = valu (ptr);
   while (*ptr++ != ',');
   forma = valu (ptr); */
	      wborder (win[vwa],
		       boxcar[vls], boxcar[vrs],
		       boxcar[vts], boxcar[vbs],
		       boxcar[vtl], boxcar[vtr],
		       boxcar[vbl], boxcar[vbr]);
	      break;
	    case 'B':		/* make a box - traditional */
	      ++ptr;
	      start = valu (ptr);
	      while (*ptr++ != ',');
	      forma = valu (ptr);
	      whline (win[vwa], boxcar[vts], start);
	      wvline (win[vwa], boxcar[vls], forma);
	      wmove (win[vwa], vrow + forma - 1, vcol);
	      whline (win[vwa], boxcar[vbs], start);
	      wmove (win[vwa], vrow, vcol + start - 1);
	      wvline (win[vwa], boxcar[vrs], forma);
	      wmove (win[vwa], vrow, vcol);
	      break;
	    case 'D':		/* define box characters */
	      ++ptr;
	      memmove (boxcar, ptr, 6);
	      break;
	    case 'b':		/* on break transfer control to */
	      forma = *(int *) (++ptr);
	      if (forma == 0)
		signal (SIGINT, &nada);
	      else
		paro = forma;
	      break;
	    case 's':		/* ts0 = make NULL termin. strings ts1 = pad strings */
	      ++ptr;
	      switch (*ptr)
		{
		case '0':
		case '1':
		  tin = atoi (ptr);
		  break;
		case '2':
		  s_n = 1;
		  break;
		case '3':	/* flush input */
		  reb = 0;
		  break;
		case '4':
		  reb = 1;
		  break;
		case '5':	/* keyboard on */
		  break;
		case '6':	/* keyboard off */
		  break;
		case '7':	/* aux port on */
		  break;
		case '8':	/* aux port off */
		  break;
		case 'f':
		  ++ptr;
		  ARFILES = atoi (ptr);
		  if (ar != NULL)
		    free (ar);
		  if ((ar = calloc (ARFILES, sizeof (ARCHI))) == NULL)
		    err ("No memory for %d db files instr. %d\n", ARFILES, ins);
		  for (i = 0; i < ARFILES; i++)
		    {
		      ar[i].a_type = 0;
		      ar[i].a_k = 0;
		      ar[i].a_buf = 0;
		    }
		  break;
		case 'p':
		  ++ptr;
		  i = atoi (ptr);
		  if (local != NULL)
		    free (local);
		  if ((local = calloc (i, sizeof (struct posics))) == NULL)
		      err ("Inst %d (tsp...;) is failing.\nNo memory for %d db structures\n",
			   ins, i);
		  MAXLOC = i;
		  break;
		case 'r':	/* advance index file */
		  recup (aa);
		  break;
		case 'P':	/* get size of file */
		  ++ptr;
		  sizfil (aa, ptr);
		  break;
		}
	      break;
	    case 'L':		/* get screen locations */
	      ++ptr;
	      if (isdigit (*ptr))
		{
		  i = atoi (ptr);
		  vrow = local[i].row;
		  vcol = local[i].col;
		  wmove (win[vwa], vrow, vcol);
		}
	      else
		switch (*ptr)
		  {
		  case '+':
		  case '-':
		    relocs (ptr);
		    break;
		  default:
		    getlocs (ptr);
		    break;
		  }
	      break;
	    case 't':
	      ++ptr;
	      typ = *ptr;
	      ss = (short *) getad (ptr);
	      switch (typ)
		{
		case 't':	/* task number */
		  *ss = (short) getpid ();
		  break;
		case 'd':	/* device number */
		  err ("td; not implemented under this OS");
		  break;
		}
	      break;
	    }
	  if (vido)
	    wrefresh (win[vwa]);
	  break;
	case 'C':		/* Cerrar un archivo previamente abierto */
	  if (ar[aa].a_buf != NULL && ar[aa].a_k != 98)
	    free (ar[aa].a_buf);
	  else
	    err ("Inst C; (%d) Closing unopened file f%d;\n", ins, aa);
	  if (ar[aa].a_type)
	    free (ar[aa].a_vsam);
	  close (ar[aa].a_fd);
	  ar[aa].a_fd = 0;
	  ar[aa].a_buf = 0;
	  break;
	case 'F':		/* Comando relativo a i/o file */
	  ++ptr;
	  switch (*ptr)
	    {
	    case 'f':		/* Activar i/o file */
	      ffaa = atoi (ptr + 1);
	      break;
	    case 'O':		/* abrir un archivo y forma */
	      ptr1 = ++ptr;
	      if (*ptr1 == '(')
		{
		  ptr1 = getad (ptr1 + 1);
		}
	      while (*ptr++ != ',');
	      tex[ffaa].x_fp = fopen (ptr1, ptr);
	      /*set_ascii_file(tex[ffaa].x_fp); */
	      if (*ptr == 'r')
		{
		  if ((tex[ffaa].x_s = calloc (256, 1)) == NULL)
		    err ("Inst %d (FO%s;) No memory for stream buffer", ins, ptr1);
		  tex[ffaa].x_sit = 2;
		}
	      break;
	    case 'P':		/* Dar los parametros de de reporte */
	      if (tex[ffaa].x_sit == 2)
		err ("Inst %d (FP..;) Report file opened for input\n", ins);
	      tex[ffaa].x_sit = 1;
	      ++ptr;
	      tex[ffaa].x_lpp = valu (ptr);
	      while (*ptr++ != ',');
	      tex[ffaa].x_tlp = 0;
	      tex[ffaa].x_pg = 0;
	      tex[ffaa].x_fl = valu (ptr);
	      while (*ptr++ != ',');
	      tex[ffaa].x_lbh = *(int *) ptr;
	      ptr += (sizeof (int));
	      tex[ffaa].x_lbf = *(int *) ptr;
	      regder = ins + 1;
	      goto header;
	    case 't':		/* End of header routine */
	      ins = regder;
	      goto e1;
	    case 'e':		/* End of footer routine */
	      goto fifoo;
	    case 'p':		/* print page number */
	      ++ptr;
	      fprintf (tex[ffaa].x_fp, ptr, tex[ffaa].x_pg);
	      break;
	    case 'W':		/* write the string in a i/o file buffer */
	      ++ptr;
	      ptr1 = tex[atoi (ptr)].x_s;
	    W1:if (vido)
		mvwaddnstr (win[vwa], vrow, vcol, ptr1, strlen (ptr1));
	      else
		fwrite (ptr1, strlen (ptr1), 1, tex[ffaa].x_fp);
	      REFR;
	      break;
	    case 'G':		/* input a line from open i/o file for 'r'eading */
	      if (fgets (tex[ffaa].x_s, 254, tex[ffaa].x_fp) == NULL)
		{
		  ins = *(int *) (++ptr);
		  goto e1;
		}
	      tex[ffaa].x_s[strlen (tex[ffaa].x_s) - 1] = 0;
	      break;
	    case 'k':		/* copy line of stream to buffer Fkpos,[VAL][(buf,pos)][l] */
	      ++ptr;
	      ptr1 = getad (ptr);
	      while (*ptr++ != ',');
	      if (*ptr == 'l')
		i = strlen (tex[ffaa].x_s);
	      else
		i = valu (ptr);
	      memmove (ptr1, tex[ffaa].x_s, i);
	      break;
	    case 'r':		/* Frbuffer,size,EOF; read size characters into buffer */
	      ++ptr;
	      ptr1 = getad (ptr);
	      while (*ptr++ != ',');
	      i = valu (ptr);
	      while (*ptr++ != ',');
	      if (fread (ptr1, i, 1, tex[ffaa].x_fp) == 0)
		{
		  ins = *(int *) ptr;
		  goto e1;
		}
	      break;
	    case 'w':
	      /* Fwbuffer,size; write size character to active file from buffer */
	      ++ptr;
	      ptr1 = getad (ptr);
	      while (*ptr++ != ',');
	      i = valu (ptr);
	      fwrite (ptr1, i, 1, tex[ffaa].x_fp);
	      break;
	    case 'c':		/* center the string in buffer o constant */
	      ++ptr;
	      forma = valu (ptr);
	      while (*ptr++ != ',');
	      if (*ptr == '\\')
		ptr1 = (ptr + 1);
	      else
		ptr1 = tex[atoi (ptr)].x_s;
	      start = strlen (ptr1);
	      start = (forma - start) / 2;
	      produ (' ', tex[ffaa].x_fp, start);
	      REFR;
	      goto W1;
	    case '!':		/* goto nn if the string and that in buffer are =  */
	      ++ptr;
	      ii = (int *) ptr;
	      ptr = ptr + sizeof (int);
	      if (strcmp (tex[ffaa].x_s, ptr) == 0)
		{
		  ins = *ii;
		  goto e1;
		}
	      break;
	    case 'T':		/* rewind activated input (i/o) file */
	      rewind (tex[ffaa].x_fp);
	      break;
	    case 'E':		/* position i/o file to the end */
	      fseek (tex[ffaa].x_fp, 0l, 2);
	      break;
	    case 'S':		/* Seek the position indicated in buffer */
	      ++ptr;
	      l = (long *) getad (ptr);
	      fseek (tex[ffaa].x_fp, *l, 0);
	      break;
	    case 'L':		/* link i/o file to a buffer position */
	      ++ptr;
	      l = (long *) getad (ptr);
	      *l = ftell (tex[ffaa].x_fp);
	      break;
	    case 'C':		/* Cerrar archivo de i/o */
	      if (ffaa < 3)
		goto CC1;
	      if (tex[ffaa].x_sit == 2)
		free (tex[ffaa].x_s);
	      fclose (tex[ffaa].x_fp);
	      tex[ffaa].x_fp = 0;
	    CC1:tex[ffaa].x_sit = 0;
	      tex[ffaa].x_tlp = 0;
	      tex[ffaa].x_pg = 0;
	      break;
	    case 'd':		/* term_load(tex[ffaa].x_fp); Fdnot available */
	      break;
	    case 's':
	      ++ptr;
	      vstr (ptr);
	      break;
	    default:
	      goto sal3;
	      break;
	    }
	  break;		/* terminan instruc. F */
	case '&':		/* Instrucciones Relativas a la hora */
	  tida (ptr + 1);
	  break;
	case 'x':		/* Ejecutar el comando de uniflex que sigue */
	  if (*(++ptr) == '(')
	    ptr1 = getad (ptr + 1);
	  else
	    ptr1 = ptr;
	  /*   signal(2,1); */
	  if (vido)
	    {
	      reset_shell_mode ();
	    }
	  system (ptr1);
	  if (vido)
	    {
	      reset_prog_mode ();
	    }
/*         signal(2,handle); */
	  break;
	case 'Z':		/* sleep for nn seconds */
	  sleep (valu (ptr + 1));
	  /*for(i=atoi(ptr+1)*3000;i > 0; i--); */
	  break;
	case 'T':		/* set active file to the beginning */
	  if (ar[aa].a_type)
	    ar[aa].a_lug = ar[aa].a_vsam->vpk;
	  else
	    ar[aa].a_lug = (long) ar[aa].a_sz;
	  break;
	case '?':		/* ?g goto ?G GOSUB */
	  typ = *(++ptr);
	  if (typ == 'b')
	    {
	      sur[nivo++] = ins + 1;
	      ins = rout (ptr);
	      goto e1;
	    }
	  lok = ins;
	  krow[kivo] = vrow;
	  kcol[kivo] = vcol;
	  tzgo[kivo] = inp;
	  tbgo[kivo] = paro;
	  ptr1 = ++ptr;
	re1:while ((i = wgetch (win[vwa])) != 3)
	    {
	      if (chefun (i))
		{
		  ins = newp;
		  goto e1;
		}
	      if (i == 27 && nivo > 0 && typ == 'G')
		{
		  ins = sur[--nivo];
		  goto e1;
		}
	      while ((i != *ptr))
		{
		  ptr = ptr + sizeof (char) + sizeof (int);
		  if (*ptr == -1)
		    {
		      ptr = ptr1;
		      BEL;
		      goto re1;
		    }
		}
	      if (typ == 'G')
		sur[nivo++] = ins + 1;
	      ins = *(int *) (ptr + 1);
	      goto e1;
	    }
	re2:ins = paro;
	  goto e1;
	  break;
	case 'y':		/* ask yes or no - go where indicated yes,no */
	  lok = ins;
	  krow[kivo] = vrow;
	  kcol[kivo] = vcol;
	  tzgo[kivo] = inp;
	  tbgo[kivo] = paro;
	  while ((i = wgetch (win[vwa])) != 3)
	    {
	      if (chefun (i))
		{
		  ins = newp;
		  goto e1;
		}
	      switch (i)
		{
		case 'y':
		case 's':
		case 'Y':
		case 'S':
		case '1':
		  ins = *(int *) (ptr + 1);
		  goto e1;
		case 'n':
		case 'N':
		case '0':
		  ins = *(int *) (ptr + 1 + sizeof (int));
		  goto e1;
		default:
		  BEL;
		  break;
		}
	    }
	  goto re2;
	case '>':		/* >sdbb,pp,bb,pp,length; or >dsbb,pp,bb,pp,%4.3f~; */
	  ++ptr;
	  typ = *ptr;
	  ++ptr;
	  typ1 = *ptr;
	  ++ptr;
	  forma = (int) getad (ptr);
	  while (*ptr++ != ',');
	  if (typ == 's')
	    {
	      largo = valu (ptr);
	      while (*ptr++ != ',');
	      start = (int) getad (ptr);
	    }
	  else
	    {
	      start = (int) getad (ptr);
	      while (*ptr++ != ',');
	    }
	  switch (typ)
	    {
	    case 's':
	      strncpy (s, (char *) forma, largo);
	      s[largo] = 0;
	      switch (typ1)
		{
		case 'd':
		  *(double *) start = atof (s);
		  break;
		case 'i':
		  *(short *) start = atoi (s);
		  break;
		case 'l':
		case 'D':
		  *(long *) start = atol (s);
		  break;
		case 'f':
		  *(float *) start = atof (s);
		  break;
		case 'c':
		  *(char *) start = atoi (s);
		  break;
		}
	      break;
	    case 'd':
	      sprintf (s, ptr, *(double *) forma);
	      goto br;
	    case 'i':
	      sprintf (s, ptr, *(short *) forma);
	      goto br;
	    case 'l':
	    case 'D':
	      sprintf (s, ptr, *(long *) forma);
	      goto br;
	    case 'f':
	      sprintf (s, ptr, *(float *) forma);
	      goto br;
	    case 'c':
	      sprintf (s, ptr, *(char *) forma);
	    br:memmove ((char *) start, s, atoi (ptr + 1));
	      break;
	    }
	  break;
	case 'e':		/* call another vpg program */
	  ++ptr;
	  if (*ptr == '^')
	    {
	      start = 1;
	      strcpy (s, ptr + 1);
	    }
	  else
	    {
	      start = 0;
	      strcpy (s, ptr);
	    }
	  vblo ('c');
	  for (i = start; i < ARFILES; i++)
	    {
	      if (ar[i].a_k == 99)
		{
		  free (ar[i].a_buf);
		  ar[i].a_k = 0;
		  ar[i].a_buf = 0;
		}
	      else
		{
		  if (ar[i].a_buf != 0)
		    {
		      close (ar[i].a_fd);
		      if (ar[i].a_k != 98)	/* added Jan 7 1988 for inst = */
			free (ar[i].a_buf);
		      if (ar[i].a_type)
			free (ar[i].a_vsam);
		      ar[i].a_k = 0;
		      ar[i].a_type = 0;
		      ar[i].a_buf = 0;
		    }
		}
	    }
	  free (tex[0].x_s);
	  for (i = 3; i < 10; i++)
	    {
	      if (tex[i].x_fp != NULL)
		{
		  fclose (tex[i].x_fp);
		  if (tex[i].x_sit == 2)
		    free (tex[i].x_s);
		}
	      tex[i].x_fp = NULL;
	      tex[i].x_tlp = 0;
	      tex[i].x_pg = 0;
	      tex[i].x_sit = 0;
	    }
	  tex[1].x_tlp = 0;
	  tex[1].x_pg = 0;
	  hrut ("hA", 0);
	  free (prog);
	  nivo = 0;
	  goto otvpg;
	case '^':		/* change directoires */
	  dirop (ptr);
	  break;
	case 'K':		/* Determine where program will go when function */
	case '<':
	  typ = *ptr++;
	  while (*ptr != ';')
	    {
	      i = atoi (ptr);
	      while (*ptr++ != ',');
	      if (typ == 'K')
		fk[i] = atoi (ptr);
	      else
		dk[i] = atoi (ptr);
	      while (*ptr++ != ',')
		if (*ptr == ';')
		  break;
	    }
	  break;
	case 'E':		/* Cancel point to go */
	  ++ptr;
	  typ = *ptr++;
	  if (*ptr == '0')
	    {
	      if (typ == 'K')
		memset (fk, 0, sizeof (int) * 0x256);
	      else
		memset (dk, 0, sizeof (int) * 0x256);
	      break;
	    }
	  switch (typ)
	    {
	    case 'K':
	      while (*ptr != ';')
		{
		  fk[atoi (ptr)] = 0;
		  while (*ptr++ != ',')
		    if (*ptr == ';')
		      break;
		}
	      break;
	    case '<':
	      while (*ptr != ';')
		{
		  dk[atoi (ptr)] = 0;
		  while (*ptr++ != ',')
		    if (*ptr == ';')
		      break;
		}
	      break;
	    }
	  break;
	case '$':		/* $sival,smem,pos; - place argument ival on smem,pos */
	  ++ptr;
	  switch (*ptr)
	    {
	    case 'i':		/* move number of arguments to $iimem,pos; */
	      ++ptr;
	      ss = (short *) getad (ptr);
	      *ss = (short) argc - 1;
	      break;
	    case 's':
	      ++ptr;
	      i = valu (ptr);
	      while (*ptr++ != ',');
	      ptr1 = getad (ptr);
	      strcpy (ptr1, argv[i + 1]);
	      break;
	    }
	  break;
	case 'q':		/* a message passing instruction follows */
	  /*++ptr;
	     i = qpas (ptr);
	     if (i == 1)
	     {
	     while (*ptr++ != ',');
	     goto h1;
	     } */
	  err ("q message passing instructions only under QNX");
	  break;
	case 'B':		/* MUCCB code */
	  ++ptr;
	  vblo (*ptr);
	  break;
	case 'h':		/* index search */
	  if (hrut (ptr, aa) == 2)
	    {
	    h1:while (*ptr++ != ',');
	      while (*ptr++ != ',');
	      ins = *(int *) ptr;
	      goto e1;
	    }
	  break;
	default:
	sal3:err ("Instr. %d Unrecognized command: %-3.3s...", ins, ptr);
	  break;
	}			/* fin de switch programa que corre */
      ++ins;
    }				/* fin de while */
  if (vido)
    endwin ();
}				/* fin de main */
