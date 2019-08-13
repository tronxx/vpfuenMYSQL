/* Estructura principal de un archivo secuencial indexado */
       struct vkstr {
       short int ksz;
       short int kstarts;
       long kpos;
       char rep;
       } __attribute__ ((packed));
   struct vsam {
       short int vnum;
       char vtk;
       long vpk;
       long pad1;
       short int vrsz[4];
       short int vpad1;
       long vhora;
       struct vkstr strk[5];
       } __attribute__ ((packed)); 
#define VSAM sizeof(struct vsam)
   struct prnrec {
       char qsq[2];
       short int qnum;
       } __attribute__ ((packed));
#define PRNREC sizeof(struct prnrec)
   struct klug {
       char lgq;
       short int nkeys __attribute__ ((packed));
       long nantl;
       } __attribute__ ((packed));
#define KLUG sizeof(struct klug)
   struct apuns {
   long anter;
   long proxi;
   long cuant;
   } __attribute__ ((packed));
#define APUNS sizeof(struct apuns)

