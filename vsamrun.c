/*
    Section to be included to use VSAM functions
    by Franz J Fortuny  ---  March 1985
*/

/* vsamrun.c -- to be included if VSAM */

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
     if(*(ptr+1) == 'l')
      if(ar[aa].a_type) err("Operacin no valida en indexado\n");
      else {
       /* irse al ultimo registro */
       ar[aa].a_lug = lseek(ar[aa].a_fd,0l,SEEK_END);
       ar[aa].a_lug -= (long)ar[aa].a_sz;
       seq = 1; break;
      }
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
	case 'k':		/* Fijar el tipo de llave para accesos */
	  ar[aa].a_k = *(++ptr) - '0';
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
	case 'T':		/* set active file to the beginning */
	  if (ar[aa].a_type)
	    ar[aa].a_lug = ar[aa].a_vsam->vpk;
	  else
	    ar[aa].a_lug = (long) ar[aa].a_sz;
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
