/* Modified to be compilable under LINUX April 1997 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <string.h>
/* Estructura principal de un archivo secuencial indexado */
   struct vkstr {
       short int ksz;
       short int kstarts;
       long kpos;
       char rep;
       }__attribute__ ((packed));
   struct vsam {
       short int vnum;
       char vtk;
       long vpk;
       long pad1;
       short int vrsz[4];
       short int vpad1;
       long vhora;
       struct vkstr strk[5];
       }__attribute__ ((packed));
#define VSAM sizeof(struct vsam)
   struct prnrec {
       char qsq[2];
       short int qnum;
       }__attribute__ ((packed));
#define PRNREC sizeof(struct prnrec)
   struct klug {
       char lgq;
       short int nkeys;
       long nantl;
       }__attribute__((packed));
#define KLUG sizeof(struct klug)
   struct apuns {
   long anter;
   long proxi;
   long cuant;
   }__attribute__ ((packed));

#define APUNS sizeof(struct apuns)
   int k1[255],k2,lev;
   long mayor,menor,recin,clusan[255],busca(),inserta(),frsp(),vfnext(),
        vfante(),inclu();


void exclu(char *esp,int i,int cam);
void libera(int fd,struct vsam *vs,int sz,long lug);
void baja(int fd,int k,struct vsam *vs);
void ponap(int fd,struct vsam *vs,char *buf,long lug);
void leerec(int fd,struct vsam *vs,char *buf,long lug);
void insef(char *dat,char *bkey,int cam,int tk1);
void inse(char *bkey,int cam,char *dat,int k,int tk);
void renu(int fd,long lug,char *esp,int cam);
void toma(int fd,long lug,char *esp,int cam);
long busca(int fd,char key[],int lla,struct vsam *vs)

#define CAM vs->strk[lla].ksz
#define BUFER esp+KLUG+(i*(CAM+8))
#define COM strncmp(key,BUFER,CAM)
{
   int i,b;
   char esp[544+KLUG];
   long *nm[5],*may[5],rec;
   struct klug *loc;
   rec = vs->strk[lla].kpos;
   loc = (struct klug *)esp;
   for(i=1;i<5;i++) {
       nm[i] = (long*) (esp+KLUG+(CAM+8)*i-8);
      may[i] = (long*) (esp+KLUG+(CAM+8)*i-4);
   }
   i=b=k2=0;
   lev=0;
   mayor = menor = recin = 0l;
   while(rec != 0l) {
       lseek(fd,rec,0);
       if((read(fd,esp,KLUG+(CAM+8)*4)) != (KLUG+(CAM+8)*4)) {
           printf("Read error from cluster %ld\n",rec);
           exit(0);
       }
       clusan[lev]= rec;
       recin = rec;
       if(loc->nkeys == 0) {
           k1[0]=1;
           return(-1l);
       }
       rec = 0l;
       for(i=0;
       (i<loc->nkeys) && ((b=COM) >=0);
       i++)
           if(b==0) { k2 = i+1;
               return(*nm[i+1]);
           }
       rec = (i==0) ? loc->nantl : *may[i];
       menor = (i==0) ? 0l: *nm[i];
       mayor = (i<loc->nkeys) ? *nm[i+1]: 0l;
       k2 = i+1;
       k1[lev++]=i+1;
   }
   --lev;
   return(-1l);
}

long inclu(int fd,struct vsam *vs,char *buf)
{
   long nw,rec,lug,k4,*nm;
   int i,cam;
   char key[136],esp[KLUG+544];
   struct prnrec *prn;
   struct apuns *ap[5];
   for(i=0;i<vs->vtk;i++)
      if(vs->strk[i].rep == 0)
          if(busca(fd,buf+PRNREC+APUNS*vs->vtk+vs->strk[i].kstarts,
              i,vs) != -1) return(0);
   prn= (struct prnrec *)buf;
   nw = frsp(fd,vs,PRNREC+APUNS*vs->vtk+vs->vrsz[0]);
   prn->qsq[0]=0x1e;
   prn->qsq[1]=0;
   prn->qnum=vs->vrsz[0];
   for(i=0;i<vs->vtk;i++){
       ap[i]=(struct apuns *)(buf+PRNREC+APUNS*i);
       ap[i]->anter = 0l;
       ap[i]->proxi = 0l;
       ap[i]->cuant = 0l;
   }
   lseek(fd,nw,0);
   write(fd,buf,PRNREC+APUNS*vs->vtk+vs->vrsz[0]);
   for(i=0;i<vs->vtk;i++){
      cam = vs->strk[i].ksz;
      lseek(fd,nw,0); read(fd,buf,PRNREC+APUNS*vs->vtk+vs->vrsz[0]);
      memmove(key,buf+PRNREC+APUNS*vs->vtk+vs->strk[i].kstarts,cam);
      if((lug = busca(fd,key,i,vs)) == -1l)
         lug=inserta(fd,key,i,vs,nw);
      if(vs->strk[i].rep == 0) {
         if(mayor != 0l) {
             leerec(fd,vs,buf,mayor);
             menor = ap[i]->anter;
             ap[i]->anter = nw;
             ponap(fd,vs,buf,mayor);
             leerec(fd,vs,buf,nw);
             ap[i]->proxi = mayor;
             ap[i]->anter = menor;
             ap[i]->cuant = 0l;
             ponap(fd,vs,buf,nw);
             if(menor != 0l) {
                leerec(fd,vs,buf,menor);
                ap[i]->proxi = nw;
                ponap(fd,vs,buf,menor);
             }
             else {
               lseek(fd,0l,0);
               vs->vpk = nw;
               write(fd,vs,VSAM);
             }
             continue;
         }
         if(menor != 0l) {
            leerec(fd,vs,buf,menor);
            mayor = ap[i]->proxi;
            ap[i]->proxi = nw;
            ponap(fd,vs,buf,menor);
            leerec(fd,vs,buf,nw);
            ap[i]->proxi = mayor;
            ap[i]->anter = menor;
            ap[i]->cuant = 0l;
            ponap(fd,vs,buf,nw);
            if(mayor != 0l) {
               leerec(fd,vs,buf,mayor);
               ap[i]->anter = nw;
               ponap(fd,vs,buf,mayor);
            }
            continue;
         }
         lseek(fd,0l,0);vs->vpk = nw;write(fd,vs,VSAM);
     }
     if(vs->strk[i].rep != 0) {
        rec = busca(fd,key,i,vs);
        nm = (long*)(esp+KLUG+(cam+8)*k2-8);
        toma(fd,recin,esp,cam);
        *nm = nw;
        renu(fd,recin,esp,cam);
        if (rec != nw) {
           leerec(fd,vs,buf,rec);
           ap[i]->anter = nw;
           k4 = ap[i]->cuant;
           ponap(fd,vs,buf,rec);
           leerec(fd,vs,buf,nw);
           ap[i]->proxi = rec;
           ap[i]->anter = 0l;
           ap[i]->cuant = k4 + 1l;
           ponap(fd,vs,buf,nw);
       }
      }
   }
   return(nw);
}
/* Funcion para insertar una nueva llave en secuencial indexado */
long inserta(int fd,char *key,int k,struct vsam *vs,long nw)
{
   int cam,i;
   long nwk,*nm,*may;
   char esp[552],bkey[136],*dat;
   struct klug *loc;
   dat = esp+KLUG;
   loc = (struct klug *)esp;
   cam = vs->strk[k].ksz;
   nm = (long*)(bkey+cam);
   may = (long*)(bkey+cam+4);
   memmove(bkey,key,cam);
   *nm=nw;*may=0l;
   nwk=0l;
   for(i=lev;i >= 0;i--) {
       toma(fd,clusan[i],esp,cam);
       if(loc->nkeys < 4) {
           inse(bkey,cam,dat,k1[i],loc->nkeys);
           loc->nkeys +=1;
           renu(fd,clusan[i],esp,cam);
           return(nw);
       }
       else {
           insef(dat,bkey,cam,k1[i]);
           loc->nkeys = 2;
           renu(fd,clusan[i],esp,cam);
           memmove(dat,dat+(cam+8)*2,(cam+8)*2);
           nwk = frsp(fd,vs,KLUG+(cam+8)*4);
           loc->nantl = (i==lev) ? 0l : *may;
           renu(fd,nwk,esp,cam);
           *may=nwk;
       }
   }
   loc->nkeys = 1;
   loc->nantl = clusan[i+1];
   memmove(dat,bkey,cam+8);
   nwk = frsp(fd,vs,KLUG+(cam+8)*4);
   renu(fd,nwk,esp,cam);
   vs->strk[k].kpos = nwk;
   lseek(fd,0l,0);
   write(fd,vs,VSAM);
   return(nw);
}

void toma(int fd,long lug,char *esp,int cam)
{
     lseek(fd,lug,0);
     read(fd,esp,KLUG+(cam+8)*4);
}
void renu(int fd,long lug,char *esp,int cam)
{
       lseek(fd,lug,0);
       write(fd,esp,KLUG+(cam+8)*4);
}

void inse(char *bkey,int cam,char *dat,int k,int tk)
{
   int i;
   for(i=tk;i != (k-1);i--)
       memmove(dat+i*(cam+8),dat+(i-1)*(cam+8),cam+8);
   memmove(dat+i*(cam+8),bkey,cam+8);
}

void insef(char *dat,char *bkey,int cam,int tk1)
{
   char tb[136];
    switch(tk1) {
       case 3:
           return;
       case 1:
          memmove(tb,dat+cam+8,cam+8);
          memmove(dat+cam+8,dat,cam+8);
          memmove(dat,bkey,cam+8);
          memmove(bkey,tb,cam+8);
          return;
       case 2:
          memmove(tb,dat+cam+8,cam+8);
          memmove(dat+cam+8,bkey,cam+8);
          memmove(bkey,tb,cam+8);
          return;
       case 4:
          memmove(tb,dat+(cam+8)*2,cam+8);
          memmove(dat+(cam+8)*2,bkey,cam+8);
          memmove(bkey,tb,cam+8);
          return;
       case 5:
          memmove(tb,dat+(cam+8)*2,cam+8);
          memmove(dat+(cam+8)*2,dat+(cam+8)*3,cam+8);
          memmove(dat+(cam+8)*3,bkey,cam+8);
          memmove(bkey,tb,cam+8);
          return;
   }
}

/* Funcion para devolver el espacio libre proximo */
long frsp(int fd,struct vsam *vs,int sz)
{
   long nw,na,ni;
   int sp;
   if(vs->pad1 == 0l) {nw=lseek(fd,0l,2); return(nw);}
   nw = vs->pad1;
   na = ni = 0l;
   while(nw != 0l) {
       lseek(fd,nw,0);
       read(fd,&ni,sizeof(long));
       read(fd,&sp,sizeof(int));
       if(sp == sz) {
           if(nw == vs->pad1) {
               vs->pad1 = ni;
               lseek(fd,0l,0);
               write(fd,vs,VSAM);
               return(nw);
           }
           else {
               lseek(fd,na,0);
               write(fd,&ni,sizeof(long));
               return(nw);
           }
       }
       na = nw;
       nw = ni;
   }
   nw=lseek(fd,0l,2); return(nw);
}
/* Funcion para obtener el siguiente elemento de un sec indx */
long vfnext(int k,char *buf)
{
   struct apuns *ap[5];
   ap[k] = (struct apuns *)(buf+PRNREC+APUNS*k);
   return(ap[k]->proxi);
}

long vfante(int k,char *buf)
{
   struct apuns *ap[5];
   ap[k] = (struct apuns *)(buf+PRNREC+APUNS*k);
   return(ap[k]->anter);
}
/* funion para tomar un record de archivo secuencial indexado */
void leerec(int fd,struct vsam *vs,char *buf,long lug)
{
   struct prnrec *prn; prn=(struct prnrec *)buf;
   lseek(fd,lug,0);
   read(fd,buf,PRNREC+APUNS*vs->vtk);
   if(prn->qsq[0] != 0x1e) {
    fprintf(stderr,"CORRUPT READING. EITHER TRY AGAIN OR REBUILD FILE\n");
    exit(0);
   }
}

/* Funcion para escribir un record de archivo secuencial indexado */
void ponap(int fd,struct vsam *vs,char *buf,long lug)
{
   lseek(fd,lug,0);
   write(fd,buf,PRNREC+APUNS*vs->vtk);
}
/* Funcion para dar de baja un record entero */
void vdel(int fd,struct vsam *vs,char *buf,long lug)
{
   int i;
   long anteri,proxim,*nm;
   char esp[KLUG+544];
   struct apuns *ap[5];
   if(buf[0] == -1) return;
   for(i=0;i<vs->vtk;i++) {
       ap[i]=(struct apuns *)(buf+PRNREC+APUNS*i);
       leerec(fd,vs,buf,lug);
       anteri = ap[i]->anter;
       proxim = ap[i]->proxi;
       if(proxim != 0l) {
           leerec(fd,vs,buf,proxim);
           ap[i]->anter = anteri;
           ponap(fd,vs,buf,proxim);
       }
       if(anteri != 0l) {
           leerec(fd,vs,buf,anteri);
           ap[i]->proxi=proxim;
           ponap(fd,vs,buf,anteri);
       }
       lseek(fd,lug,0); read(fd,buf,PRNREC+APUNS*vs->vtk+vs->vrsz[0]);
       if(vs->strk[i].rep == 0) {
           if(anteri == 0l) {
               lseek(fd,0l,0);
               vs->vpk=proxim;
               write(fd,vs,VSAM);
           }
       }
       else {
           if((anteri==0l) && (proxim !=0l)) {
               busca(fd,buf+vs->strk[i].kstarts+PRNREC+APUNS*vs->vtk,
               i,vs);
               toma(fd,recin,esp,vs->strk[i].ksz);
               nm = (long*)(esp+KLUG+(k2*(vs->strk[i].ksz+8)-8));
               *nm = proxim;
               renu(fd,recin,esp,vs->strk[i].ksz);
           }
       }
       if(((anteri==0l) && (proxim==0l)) || (vs->strk[i].rep==0)) {
           busca(fd,buf+vs->strk[i].kstarts+PRNREC+APUNS*vs->vtk,
           i,vs);
           baja(fd,i,vs);
       }
   }
   libera(fd,vs,PRNREC+APUNS*vs->vtk+vs->vrsz[0],lug);
   buf[0]=0xff;
}
/* Funcion para dar de baja a una llave del archivo sec indx */
void baja(int fd,int k,struct vsam *vs)
{
   int i,cam,r;
   long sig,*may,*myr[5];
   char esp[544],*dat,bkey[136];
   struct klug *loc;
   loc = (struct klug *)esp;
   dat = esp+KLUG;
   cam=vs->strk[k].ksz;
   may = (long*)(bkey+cam+4);
   for(i=1;i<5;i++) {
       myr[i] = (long*)(esp+KLUG+(cam+8)*i-4);
   }
   for(i=0;i<544;i++) esp[i]=0;
   for(i=0;i<136;i++) bkey[i]=0;
   k1[lev]=k2;
   toma(fd,recin,esp,cam);
   memmove(bkey,dat+(k2-1)*(cam+8),cam+8);
   if(*may == 0l) {
       exclu(esp,k2,cam);
       renu(fd,recin,esp,cam);
   }
   else {
       k1[lev] += 1;
       sig = *may;
       while(sig != 0l) {
           toma(fd,sig,esp,cam);
           clusan[++lev] = sig;
           k1[lev]=1;
           sig = loc->nantl;
       }
       memmove(bkey,dat,cam+4);
       exclu(esp,1,cam);
       renu(fd,clusan[lev],esp,cam);
       toma(fd,recin,esp,cam);
       memmove(dat+(k2-1)*(cam+8),bkey,cam+4);
       renu(fd,recin,esp,cam);
   }
   if(lev==0) return;
   for(i=lev;i > 0;i--) { r=0;
       toma(fd,clusan[i],esp,cam);
       if(loc->nkeys >= 2) return;
       else {
           toma(fd,clusan[i-1],esp,cam);
           sig = loc->nantl;
           if((k1[i-1] > loc->nkeys) && (loc->nkeys > 1))
      r=forma(fd,vs,esp,*myr[k1[i-1]-2],clusan[i-1],clusan[i],i,cam);
           else if((k1[i-1] == 1) || ((k1[i-1]==2) && (loc->nkeys==1)))
            r=forma(fd,vs,esp,loc->nantl,clusan[i-1],*myr[1],i,cam);
           else
       r=forma(fd,vs,esp,clusan[i],clusan[i-1],*myr[k1[i-1]],i,cam);
       }
   }
   if(r==1) {
   vs->strk[k].kpos = sig;
   lseek(fd,0l,0);
   write(fd,vs,VSAM);
   }
}

forma(int fd,struct vsam *vs,char *esp,long c1,long c2,long c3,
		int i,int cam)
{
   struct klug *loc;
   char *dat,bkey[136];
   int ky1,ky3,d;
   long sig,*m[5],*mau,sig1;
   loc = (struct klug*)esp; dat = (char*)(esp+KLUG);
   for(d=1;d<5;d++) m[d] = (long*)(dat+(cam+8)*d-4);
   for(d=0;d<136;d++) bkey[d]=0;
   mau=(long*)(bkey+cam+4);
   toma(fd,c1,esp,cam);
   ky1 = loc->nkeys;
   sig1=*m[ky1];
   toma(fd,c3,esp,cam);
   ky3 = loc->nkeys;sig = loc->nantl;
   d = (i==lev) ? 4 : 8;
   switch(ky1) {
       case 3:
       case 4:
       toma(fd,c2,esp,cam);
       memmove(bkey,dat+(cam+8)*(k1[i-1]-2),cam+d);
       toma(fd,c3,esp,cam);
       if(i < lev) {*mau=loc->nantl;loc->nantl=sig1;}
       inse(bkey,cam,dat,loc->nkeys,1);loc->nkeys += 1;
       renu(fd,c3,esp,cam);
       toma(fd,c1,esp,cam);
       memmove(bkey,dat+(cam+8)*(loc->nkeys-1),cam+d);loc->nkeys -= 1;
       if(i<lev) *mau=c3;
       renu(fd,c1,esp,cam);
       toma(fd,c2,esp,cam);
       memmove(dat+(cam+8)*(k1[i-1]-2),bkey,cam+d);
       renu(fd,c2,esp,cam);
       return(0);
       case 2:
       toma(fd,c2,esp,cam);
       if(loc->nkeys == 1) {
           toma(fd,c1,esp,cam);
           lseek(fd,c2+(long)KLUG,0);
           read(fd,esp+KLUG+(cam+8)*2,cam+d);
           lseek(fd,c3+(long)KLUG,0);
           read(fd,esp+KLUG+(cam+8)*3,cam+d);loc->nkeys = 4;
           if(i < lev) *m[3] = sig;
           renu(fd,c1,esp,cam);
           libera(fd,vs,KLUG+(cam+8)*4,c2);
           libera(fd,vs,KLUG+(cam+8)*4,c3);
           return(1);
       }
       else {
           toma(fd,c1,esp,cam);
           lseek(fd,c2+(long)(KLUG+(cam+8)*(k1[i-1]-2)),0);
           read(fd,esp+KLUG+(cam+8)*2,cam+d);
           lseek(fd,c3+(long)KLUG,0);
           read(fd,esp+KLUG+(cam+8)*3,cam+d);loc->nkeys = 4;
           if(i < lev) *m[3] = sig;
           renu(fd,c1,esp,cam);
           toma(fd,c2,esp,cam);
           exclu(esp,k1[i-1]-1,cam);
           renu(fd,c2,esp,cam);
           libera(fd,vs,KLUG+(cam+8)*4,c3);
           return(0);
       }
       case 1:
       toma(fd,c2,esp,cam);
       if((loc->nkeys == 1) && (ky3 == 2)){
           toma(fd,c1,esp,cam);
           lseek(fd,c2+(long)KLUG,0);
           read(fd,esp+KLUG+(cam+8),cam+d);
           lseek(fd,c3+(long)KLUG,0);
           read(fd,esp+KLUG+(cam+8)*2,(cam+8)*2);
           loc->nkeys=4;
           if(i < lev) *m[2] = sig;
           renu(fd,c1,esp,cam);
           libera(fd,vs,KLUG+(cam+8)*4,c2);
           libera(fd,vs,KLUG+(cam+8)*4,c3);
           return(1);
       }
       if((loc->nkeys == 1) && (ky3 > 2)) {
           toma(fd,c1,esp,cam);
           lseek(fd,c2+(long)KLUG,0);
           read(fd,esp+KLUG+(cam+8),cam+d);
           if(i < lev) *m[2] = sig;
           loc->nkeys += 1;renu(fd,c1,esp,cam);
           toma(fd,c3,esp,cam);
           memmove(bkey,dat,cam+d);
           exclu(esp,1,cam);
           if(i<lev) loc->nantl=*mau;
           renu(fd,c3,esp,cam);
           toma(fd,c2,esp,cam);
           if(i<lev) *mau=c3;
           memmove(dat,bkey,cam+d);
           renu(fd,c2,esp,cam);
           return(0);
       }
       if((loc->nkeys > 1) && (ky3 == 2)) {
           toma(fd,c1,esp,cam);
           lseek(fd,c2+(long)(KLUG+(cam+8)*(k1[i-1]-1)),0);
           read(fd,esp+KLUG+(cam+8),cam+d);
           lseek(fd,c3+(long)KLUG,0);
           read(fd,esp+KLUG+(cam+8)*2,(cam+8)*2);loc->nkeys = 4;
           if(i<lev) *m[2] = sig;
           renu(fd,c1,esp,cam);
           libera(fd,vs,KLUG+(cam+8)*4,c3);
           toma(fd,c2,esp,cam);
           exclu(esp,k1[i-1],cam);renu(fd,c2,esp,cam);
           return(0);
       }
       if((loc->nkeys > 1) && (ky3 > 2)) {
           toma(fd,c1,esp,cam);
           lseek(fd,c2+(long)(KLUG+(cam+8)*(k1[i-1]-1)),0);
           read(fd,esp+KLUG+(cam+8),cam+d);
           if(i < lev) *m[2] = sig;
           loc->nkeys += 1;renu(fd,c1,esp,cam);
           toma(fd,c3,esp,cam);
           if(i<lev) {loc->nantl=*m[1];*m[1]=c3;}
           memmove(bkey,dat,cam+d);
           exclu(esp,1,cam);renu(fd,c3,esp,cam);
           toma(fd,c2,esp,cam);
           memmove(dat+(cam+8)*(k1[i-1]-1),bkey,cam+d);
           renu(fd,c2,esp,cam);
           return(0);
       }
   }
 return(0);
}
/* Funcion para liberar espacio que se ha desocupado */
void libera(int fd,struct vsam *vs,int sz,long lug)
{
   long sit;
   int tam;
   sit = vs->pad1;tam=sz;
   lseek(fd,lug,0);
   write(fd,&sit,sizeof(long));
   write(fd,&tam,sizeof(int));
   vs->pad1 = lug;
   lseek(fd,0l,0);
   write(fd,vs,VSAM);
}
/* Funcion para excluir un elemento de un cluster */
void exclu(char *esp,int i,int cam)
{
   struct klug *loc;
   char *dat;
   int l;
   loc = (struct klug *)esp;dat = (char*)(esp+KLUG);
   for(l=i; l != loc->nkeys;l++)
       memmove(dat+(cam+8)*(l-1),dat+(cam+8)*l,cam+8);
   loc->nkeys -= 1;
}
