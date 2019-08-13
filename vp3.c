/* VPG Source Pre-Compiler, by Franz J Fortuny - Started February 6 1988
Version 4.00 - March 1989
March 10 1989 - Modified to EXPAND symbols in program contexts
 form t1:453
January 7, 1995 - Modified for QNX4 and IBM AS/400
*/
#include <stdio.h>
#include <stdlib.h>
#define MAXLIN 256
#define LEVELS 30
#define POSITIONS 40
 char line[MAXLIN],typ,finins,sr[80],markif,arre;
 int pos,siz,instr,pins,generr,nv,renglo,levl,loop[LEVELS],kas[LEVELS];
 int iflev,ssiz[30],sub,memo,tamvar;
 unsigned int *lab; void addsym();
 struct symbol {
  struct symbol *sy_ante;
  char *sy_symbol;
  int sy_pos,sy_siz;
  char *stval,bfn,sy_typ,sy_arr;
  struct symbol *sy_prox;
 } *sym, *anter, *this, *next, *proxi, *nsym[30];
#define SYMBOL sizeof(struct symbol)
#define ININC lab[pins++] = (int) ftell(obj)
#define Printf fprintf(stdout,
 extern long ftell();
 long lpos[LEVELS][POSITIONS],nowlug,ifloc[LEVELS];
 FILE *obj,*fo[30];

 void clnspc(FILE *);
 void extsym3(FILE *); /* extract the symbols in a line */
 void extsym4(FILE *); /* extract the symbols in a line */
 void end(char *format, ... );
void main(int argc,char *argv[])
{
 FILE *src,*fin;
 char s1[60],s2[60],*ptr,*strchr(),filname[60];
 int ts,re,i;
 if(argc==0) {
  printf(stdout, "VP: vpg source text pre-compiler. Use: vp program.p [+sNNNN] [+d/direc/]\n");
  exit(0);
 }
 s2[0]=0; ts=0;
 for(i=1;i<argc;i++) {
  ptr=argv[i];
  if(*ptr == '+') {
   ++ptr;
   switch(*ptr) {
    case 's': ++ptr; ts=atoi(ptr); break;
    case 'd': ++ptr; strcpy(s2,ptr); break;
    default:
     end("Unknown option %c\n",*ptr);
    break;
   }
  }
 }
 generr=0;
 if(ts == 0) ts=1536;
 if((sym=calloc(SYMBOL,ts)) == NULL) end("Can't obtain memory for symbols.");
 next = sym; instr=0; pins=0;
 if((src=fopen(argv[1],"r")) == NULL) end("Can't open source file %s",argv[1]);
 getsymbols(src); fprintf(stdout,"Total Instructions: %d\n",instr);
 rewind(src);
 ++instr;
 if((lab=calloc(sizeof(int),instr)) == NULL)
  end("Can't obtain memory for instructions' pointers\n");
 strcpy(s1,argv[1]);
 if((ptr=strchr(s1,'.')) == NULL)
  end("Bad source name... %s\n",s1);
 ++ptr; *ptr=0;
 strcat(s2,s1);
 strcat(s2,"vp");
 if((obj=fopen(s2,"w+")) == NULL)
  end("Can't open vpg object file %s.\n",s2);
 fwrite(&instr,sizeof(int),1,obj);
 fwrite(lab,sizeof(int),instr,obj);
 getins(src); fclose(src); fclose(obj); obj=fopen(s2,"r+");
 fseek(obj,(long)sizeof(int),0);
 fwrite(lab,sizeof(int),instr,obj);
 perror("Status");
 fprintf(stdout,"Recognized VAG code errors: %d\n",generr);
 if(iflev != 0) fprintf(stdout,"If -- then -- else... (missing dot or bar): %d\n",iflev);
 if(levl != 0) fprintf(stdout,"Loops not closed: %d\n",levl);
 /* fopen(filname,"w");  */
}

/* get the instructions and write them to new file */
getins(fp) FILE *fp;
{
 int c,val,i;
 while((c=getc(fp)) != '}') {
 sw1: if(c=='}') break;
  if(c==EOF) {
   if(nv > 0) {
    fclose(fp); fp=fo[--nv];
    continue;
   }
   else end("No END of Program Mark");
  }
  if(isdigit(c)) {
   tomins(fp);
   continue;
  }
  switch(c) {
   case '#': /* diversion */
    c=getc(fp);
    switch(c) {
     case 'i': /* include another file */
      i=0;
      while((sr[i++] = getc(fp)) != ';') ;
      sr[--i]=0;
      fo[nv++]=fp;
      if((fp=fopen(sr,"r")) == NULL)
       end("Can't open Diversion File %s(2)\n",sr);
     break;
     case 'd': /* define a symbol */
      tomins(fp);
     break;
    }
   break;
   case ' ': case '\n': case '\r': case '\t':
    while(isspace(c=getc(fp)));
    goto sw1;
   break;
   case '*':
    while((c=getc(fp)) != '*');
   break;
   case '[':
    tomins(fp);
   break;
   case 'M':
    tomins(fp);
   break;
   case '{':  if(levl==LEVELS) end("\nToo many levels...\n");
    loop[levl++] = pins;
   break;
   case ')': ININC; val=loop[--levl];
    putc('g',obj); fwrite(&val,sizeof(int),1,obj);
    repos(pins);
   break;
   case '\\':
    ININC;
    putc(c,obj);
    while((c=getc(fp)) != ';') { if(c=='\\') c=0; putc(c,obj); }
   break;
   case 'F':
    ININC;
    putc(c,obj);
    c=getc(fp);
    switch(c) {
	 case 's': putc(c,obj); c=getc(fp); goto desymbol;
     case 'p':
     case 'k':
     case 'S':
     case 'L':
      goto desymbol;
     break;
     case 'O': putc(c,obj); c=getc(fp);
      if(c!='(') goto others;
      putc(c,obj); putaval(fp); c=getc(fp); goto others;
     break;
     case 'P': putc(c,obj);
      extsym4(fp);
      extsym4(fp);
      val=intsym(fp); c=getc(fp);
      fwrite(&val,sizeof(int),1,obj);
      val=intsym(fp); c=getc(fp);
      fwrite(&val,sizeof(int),1,obj);
     break;
	 case 'r': putc(c,obj); extsym4(fp); extsym4(fp);
 	  val=intsym(fp); c=getc(fp);
      fwrite(&val,sizeof(int),1,obj);
 	 break;
	 case 'w': putc(c,obj); extsym4(fp); extsym4(fp);
	 break;
     case 'c': putc(c,obj); extsym4(fp); c=getc(fp); goto others;
     case 'G': goto glabs; break;
     case '!':
      putc(c,obj);
      val=intsym(fp); c=getc(fp);
      fwrite(&val,sizeof(int),1,obj);
      c=getc(fp);
      goto others;
     break;
     default:
      goto others;
     break;
    }
   break;
  case 't':
      ININC;
      putc(c,obj);
      c=getc(fp);
      switch(c) {
        case 'k':
        case 'b':
        case '0': goto glabs; break;
        case 't': putc(c,obj); c=getc(fp); goto desymbol; break;
        case 'R':
        case 'S':
        case 'B':
        case '@': goto desymbol; break;
        case 's': putc(c,obj); c=getc(fp);
          switch(c) {
            case 'P': goto desymbol; break;
            default: goto others; break;
          }
          break;
        default: goto others; break;
      }
   break;
   case 'y':
    ININC;
    putc(c,obj);
    val=intsym(fp); c=getc(fp);
    if(markif) {
     val=pins; fwrite(&val,sizeof(int),1,obj);
     goto glabs3;
    }
    fwrite(&val,sizeof(int),1,obj);
    val=intsym(fp); c=getc(fp);
    fwrite(&val,sizeof(int),1,obj);
   break;
   case 'g':
   case 'G':
   case 'r':
   case 'n':
   case 'N':
   case 'Q':
    ININC;
  glabs:
    putc(c,obj);
    val=intsym(fp); c=getc(fp);
    fwrite(&val,sizeof(int),1,obj);
    if(markif) { ININC;
  glabs2:
     putc('g',obj);
  glabs3:
     ifloc[iflev++] = ftell(obj);
     val=0; fwrite(&val,sizeof(int),1,obj);
     markif=0;
    }
   break;
   case '|': ININC;
    nowlug=ftell(obj); fseek(obj,ifloc[--iflev],0);
    val=pins; fwrite(&val,sizeof(int),1,obj);
    fseek(obj,nowlug,0);
   goto glabs2;
   break;
   case '.':
    nowlug=ftell(obj); fseek(obj,ifloc[--iflev],0);
    val=pins; fwrite(&val,sizeof(int),1,obj);
    fseek(obj,nowlug,0);
   break;
   case '!':
    ININC;
    putc(c,obj);
if2: while((c=getc(fp)) != ')') {
     if(isspace(c)) continue;
     if(c=='~'|| c=='¬') c=0;
     putc(c,obj);
     if(c==',' || c=='(' ||c=='[') {putaval(fp); continue; }
    } putc(c,obj);
    while((c=getc(fp)) != '(' ) if(isspace(c)) continue; else putc(c,obj);
    putc(c,obj);
    putaval(fp);
    if((c=getc(fp)) == ',') {
     memo=0xff; putc(c,obj); putaval(fp);
    }
    else ungetc(c,fp);
    while((c=getc(fp)) != ')') if(isspace(c)) continue; else putc(c,obj);
     putc(c,obj);
     while(c=getc(fp)) {
         if(isspace(c)) continue;
         if(c== '>' || c == '<') goto glabs;
         if(c=='(') {ungetc(c,fp); goto if2; }
         putc(c,obj);
     }
   break;
   case '?':
    ININC;
    putc(c,obj); c=getc(fp);
    if(c=='b') {
     putc(c,obj);
     extsym4(fp);
    }
    else putc(c,obj);
    while((c=getc(fp)) != ';') {
     if(isspace(c)) continue;
     if(isdigit(c)) {
      ungetc(c,fp);
      val=intsym(fp); c=getc(fp);
      c=atoi(line);
      fwrite(&c,sizeof(char),1,obj);
      fwrite(&val,sizeof(int),1,obj);
      if(finins) break;
      continue;
     }
     switch(c) {
      case '^':
       c=getc(fp);
       c = c - 64;
       fwrite(&c,sizeof(char),1,obj);
       val=intsym(fp); c=getc(fp);
       fwrite(&val,sizeof(int),1,obj);
      break;
      case '[':
       c=getc(fp);
       fwrite(&c,sizeof(char),1,obj);
       val=intsym(fp); c=getc(fp);
       fwrite(&val,sizeof(int),1,obj);
      break;
      default:
       fprintf(stdout,"Instruction ? (Number %d) - Improprerly formed.\n",pins-1);
       ++generr;
      break;
     }
     if(finins) break;
    }
    c= -1; fwrite(&c,sizeof(char),1,obj);
   break;
   case '&':
    ININC;
    putc(c,obj);
    c=getc(fp);
    switch(c) {
     case 'a': goto others; break;
     case 'p': putc(c,obj); c=getc(fp); goto desymbol;
     default: goto desymbol;
    }
   break;
   case '$':
   case 'L': ININC; putc(c,obj); c=getc(fp); goto desymbol;
   case '>': ININC; putc(c,obj); putc(getc(fp),obj); c=getc(fp); goto desymbol;
   case '(':
   case 'A':
   case 'd':
   case 'P':
   case 'S':
   case 'z':
   case 'i':
   case 'Z':
   case 's':
   case '@':
   case '`':
   case 'p':
    ININC;
desymbol:
    putc(c,obj);
    extsym3(fp);
   break;
   case 'H':
   case 'k':
   case 'q':
   case 'W':
   case 'O':
   case 'o':
   case 'J':
    ININC;
    putc(c,obj);
    while((c=getc(fp)) != ',') {
     putc(c,obj);
     if(c==';') goto termin1;
    }
    goto desymbol3;
    termin1: break;
   case '^':
   case 'x': ININC; putc(c,obj); c=getc(fp);
   if(c=='(' || c=='=') {
    putc(c,obj); putaval(fp); c=getc(fp);
   }
   goto others;
/*   case 'O': case 'o': ININC; putc(c,obj); c=getc(fp);
    if(c=='(') {
     putc(c,obj); putaval(fp);
    }
    if(c == '@')  goto others;
    else putc(c,obj);
    while((c=getc(fp)) != ',') {
     if(c=='~') c=0; putc(c,obj); } putc(c,obj);
    c=getc(fp);
    if(c=='v') { putc(c,obj); c=getc(fp); }
    putc(c,obj); extsym3(fp);
   break; */
/*   case 'q': ININC; putc(c,obj); c=getc(fp);
    switch(c) {
     case 'R': case 'e': goto others;
     case 'r': case 'y': goto desymbol3;
     case 'l':
      putc(c,obj); putaval(fp); c=getc(fp); putc(c,obj);
      putaval(fp); c=getc(fp); putc(c,obj);
	  while((c=getc(fp)) != ',') {if(c=='~') c=0; putc(c,obj);}
     goto glabs;
     case 's': putc(c,obj); putaval(fp); c=getc(fp); putc(c,obj); putaval(fp);
               c=getc(fp); putc(c,obj); putaval(fp); c=getc(fp);
     goto glabs;
    }
   break;*/
   case 'h': ININC; putc(c,obj); c=getc(fp);
    switch(c) {
     case 'o': putc(c,obj); while((c=getc(fp)) != ',') putc(c,obj);
      putc(c,obj); putaval(fp); c=getc(fp);
      goto glabs;
     break;
     case 'f': case 's': goto desymbol3;
     case 'a': case 'm': case 'n': case 'p': case 't': case 'b': case 'C':
     case 'A': goto others;
    }
    break;
   case 'E': ININC; putc(c,obj); c=getc(fp); goto desymbol3;
   case 'K':
   case '<':
   case 'm': ININC; goto desymbol3;
   case 'c':
    ININC;
    putc(c,obj);
    c=getc(fp);
desymbol3:
    putc(c,obj);
    extsym3(fp);
   break;
   default:
    ININC;
  others:
    tomout(fp,c);
   break;
  } /* end of switch */
 } /* end of while */
 ININC;
 putc('}',obj);
} /* end of getins */

void extsym3(fp) FILE *fp; /* extract the symbols in a line */
{
 int c;
 putaval(fp);
 while((c=getc(fp)) != ';') {
  if(isspace(c)) continue;
  if(c=='~') c=0;
  putc(c,obj);
  if(c==','|| c=='(' || c=='[') {putaval(fp); continue; }
 }
 putc(';',obj);
}
void extsym4(fp) FILE *fp; /* extract the symbols in a line */
{
 int c;
 putaval(fp);
 while((c=getc(fp)) != ',') {
  if(isspace(c)) continue;
  if(c=='~') c=0;
  putc(c,obj);
  if( c=='(' || c=='[') {putaval(fp); continue; }
 }
 putc(',',obj);
}

putaval(FILE *fp) /* translate symbol into value */
{
 int val,c;
 if((val=intsym2(fp)) == -1) return 0;
 if(typ == 'C' && arre) { fprintf(obj,"s%d:",memo); arre=0; return(0); }
 if(arre) { fprintf(obj,"%c%d:",typ,memo); arre=0; return(0); }
 if(tamvar) {tamvar=0; fprintf(obj,"%d",siz); return(0); }
 if(typ != 'm' && memo != 0xff) fprintf(obj,"%c%d:%d",typ,memo,val);
 else fprintf(obj,"%d",val);
 return(0);
}

intsym2(fp) FILE *fp; /* function to interpret value of symbol */
{
 int c,n=0,r;
 char d,posit=0;
 clnspc(fp);
 if(!isalpha(c=getc(fp))) {
  switch(c) {
   case ';': ungetc(c,fp); return -1;
   case '"':
  putc(c,obj);
  while((c=getc(fp)) != '"') putc(c,obj);
  putc(0,obj);
  return -1;
   case '#': memo = 0xff; return(siz); break;
   case '$':
    putc(c,obj);
    while((c=getc(fp)) != ',') {
     if(c == ';' || c == ']' || c==')') break;
     putc(c,obj);
    }
    ungetc(c,fp); return(-1);
   break;
   default: putc(c,obj); return(-1); break;
  }
 }
 line[n++]=c;
 while((c=getc(fp)) != ',') {
  if(c==';' || c==')' || c=='(' || c==']'|| c == '[') break;
  line[n++]=c;
 }
 ungetc(c,fp);
 line[n]=0;
 if(strcmp(line,"l") == 0) {putc('l',obj); return(-1); }
 if(line[n-1] == '&') { posit=1; --n; line[n]=0; goto cont1; }
 if(line[n-1] == '#') { tamvar=1; --n; line[n]=0; }
cont1:
 this = sym;
 while(this != 0) {
  r=strcmp(line,this->sy_symbol);
  if(r==0) {
   if(this->stval != 0) {
    r=strlen(this->stval);
    for(n=0;n<r;n++) {if(this->stval[n] == '~') putc(0,obj); else putc(this->stval[n],obj);}
    return(-1);
   }
   siz=this->sy_siz;
   arre = this->sy_arr;
   if(posit) {memo = 0xff; arre=0;}
   else memo = this->bfn;
   typ=this->sy_typ;
   return(this->sy_pos);
  }
  if(r>0) { this = this->sy_prox; continue; }
  this = this->sy_ante;
 }
 fprintf(stdout,"Symbol %s - Not defined.\n",line);
 ++generr;
 return(0);
} /* end of intsym */

intsym(fp) FILE *fp; /* function to interpret value of symbol */
{
 int c,n=0,r,getpos();
 finins=0;
 while((c=getc(fp)) != ',') {
  if(c==';' || c==')' || c=='(' || c==']') break;
  line[n++]=c;
 }
 ungetc(c,fp);
 if(c==';') finins=1;
 line[n]=0;
 if(line[0]=='+' || line[0]== '-') { return(getpos(line[0])); }
 if(line[0]=='|') { markif=1; return(pins+1); }
 this = sym;
 while(this != 0) {
  r=strcmp(line,this->sy_symbol);
  if(r==0) {siz=this->sy_siz; return(this->sy_pos);}
  if(r>0) { this = this->sy_prox; continue; }
  this = this->sy_ante;
 }
 fprintf(stdout,"Symbol %s - Not defined.\n",line);
 ++generr;
 return(0);
} /* end of intsym */

tomins(fp) FILE *fp;
{
 int c;
 while((c=getc(fp)) != ';');
}

tomout(fp,c) FILE *fp; int c;
{
 putc(c,obj);
 while((c=getc(fp)) != ';') {
  if(c == '~') c=0;
  putc(c,obj);
 }
 putc(';',obj);
}

/* function to read all the symbols in VPG text program */
getsymbols(fp) FILE *fp;
{
 int c,n=0,i,c1; char s[10];
  while((c=getc(fp)) != '}') {
  sw1:
  if(c=='}') break;
  if(c==EOF) {
   if(nv > 0) {
    fclose(fp); fp = fo[--nv];
    continue;
   }
   else end("No END OF PROGRAM MARK\n");
  }
  n=0;
 if(isdigit(c)) {
  line[n++]=c;
  while((c=getc(fp)) != ';') line[n++]=c;
  line[n]=0;
/*  fprintf(stdout,"%s\n",line); */
  proclabel();
  continue;
 }
  switch(c) {
   case ' ':  case '{': case '.': while((c=getc(fp)) == ' '); goto sw1; break;
   case '\t': while((c=getc(fp)) == '\t'); goto sw1; break;
   case '\n': ++renglo; while((c=getc(fp)) == '\n') ++renglo; goto sw1; break;
   case '\r': ++renglo; while((c=getc(fp)) == '\r') ++renglo; goto sw1; break;
   case '*': while((c=getc(fp)) != '*') if(c=='\n') ++renglo; break;
   case '[': /* name of a label - get position */
    memo = 0xff; typ = 'm';
    while((c=getc(fp)) != ';') line[n++]=c;
    line[n]=0;
    proclabel();
   break;
   case 'M': /* here come defined symbols */
    pos=0; typ=0; i=0;
    while(extsym(fp) != 0){ addsym(); pos += siz; }
   break;
   case '#': /* diversion instruction */
    c=getc(fp); i=0; n=0;
    if(c=='i') /* include another file */ {
     while((sr[i++] = getc(fp)) != ';') ;
     fo[nv++]=fp; sr[--i] = 0;
     if((fp=fopen(sr,"r")) == NULL) {
      end("Include File Can't be opened (%s)\n",sr);
     }
     break;
    }
    if(c=='d') /* define a symbol to a string */ { memo = 0xff;
     while((line[n++] = getc(fp)) != ' ') ; line[n-1]=0;
     proclabel();
     n=0; while((line[n++] = getc(fp)) != ';') ; line[n-1]=0;
     if((this->stval=malloc(strlen(line)+1)) == NULL) {
      end("No more memory at instruction %s\n",line);
     }
     strcpy(this->stval,line);
     break;
    }
   break;
   case 'F': c=getc(fp); if(c=='G') goto chk1; else goto fin2;
/*   case 'q': c=getc(fp); if(c=='s' || c=='l') goto chk1; else goto fin2;*/
   case 't': c=getc(fp); if(c=='0' || c=='k') goto chk1; else goto fin2;
   case ')': case '|': while( isspace((c=getc(fp))) ); ++instr; goto sw1; break;
   case 'g':
   case 'G':
   case 'r':
   case 'n':
   case 'N':
   case 'Q':
   case '!':
   case 'h':
chk1:
    while((c=getc(fp)) != ';') c1 = c;
    if( c1 == '|' ) instr +=2;
    else ++instr;
   break;
   case 'f': /* get active memory */
    ++instr; n=0; while((c=getc(fp)) != ';') s[n++]=c;
    s[n]=0; memo = atoi(s);
   break;
   default:
/*  fprintf(stdout,"%4d %c\n",instr,c); */
fin2:
    while((c=getc(fp)) != ';');
    ++instr;
   break;
  }
 }
 ++instr;
}

extsym(fp) FILE *fp;
{
 int c,n=0,n1=0;
 char s[10];
nxsym: n=0; n1=0; arre=0;
 while(isspace(c=getc(fp)));
 if(c== ';') return(0);
 if(c== '*') { while((c=getc(fp)) != '*'); goto nxsym; }
 if(c== '}') {
  this = nsym[--sub]; this->sy_siz = ssiz[sub]; this->sy_typ= 's';
  if(sub > 0) ssiz[sub-1] += ssiz[sub];
  siz=0; goto nxsym;
 }
 line[n++]=c;
 while(!isspace(c=getc(fp))) line[n++]=c;
 line[n]=0;
 while(isspace(c=getc(fp)));
 typ=c;
 if(c=='{') { arre=0;
  siz=0; addsym(); ssiz[sub]=0; nsym[sub++] = this; goto nxsym;
 }
 if(c=='s') {
  while((c=getc(fp)) != ',') s[n1++]=c;
  s[n1]=0;
  siz=atoi(s);
 }
 else {
  switch(c) {
   case 'i': siz = sizeof(short); break;
   case 'e': siz = sizeof(int); break;
   case 'd': siz = sizeof(double); break;
   case 'D':
   case 'l': siz = sizeof(long); break;
   case 'f': siz = sizeof(float); break;
   case 'c':
   case 'C': siz = sizeof(char); break;
   case 'm': siz = 0; break;
  }
  if(isdigit(c=getc(fp))) {
   n1=0; s[n1++]=c; while(isdigit(c=getc(fp))) s[n1++]=c;
   s[n1]=0; siz *= atoi(s);
   arre = 1;
  }
  else arre = 0;
  ungetc(c,fp);
  while((c=getc(fp)) != ',');
 }
 if(sub > 0) ssiz[sub-1] += siz;
 return(n);
}

void addsym() /* add a new symbol to table */
{
 int r;
 this = sym;
 if(this->sy_symbol == 0) {
  getmem();
  return;
 }
 while(this != 0) {
  r=strcmp(line,this->sy_symbol);
  if(r == 0) {
   fprintf(stdout,"%s - Duplicate Symbol\n",this->sy_symbol);
   ++generr;
   return;
  }
  if(r > 0) {
   anter = this;
   this = this->sy_prox;
   continue;
  }
  if(r < 0) {
   proxi = this;
   this = this->sy_ante;
   continue;
  }
 }
 this = next;
 getmem();
 if(r > 0) anter->sy_prox = this;
 else proxi->sy_ante = this;
 return;
}

getmem() /* get memory for symbol and copy into symbol */
{
 if((this->sy_symbol = malloc(strlen(line)+1)) == NULL) {
  fprintf(stdout,"At symbol %s:\n",line);
  end("Can't obtain any more memory");
 }
 strcpy(this->sy_symbol,line);
 this->sy_pos = pos; this->sy_siz = siz;
 this->sy_ante=0; this->sy_prox=0; this->sy_arr = arre;
 this->stval = 0; this->bfn = memo; this->sy_typ = typ;
 /* fprintf(stdout,"Symbol %s - at %d \n",this->sy_symbol,this->sy_pos); */
 ++next;
}

proclabel() /* label in line - include it and get position */
{
 pos = instr;
 addsym();
}

getpos(c) /* store position of pending goto */
char c;
{
 if(c == '-') return(loop[levl-1]);
 if(kas[levl-1]==POSITIONS) end("\nToo many positions...\n");
 lpos[levl-1][kas[levl-1]++] = ftell(obj);
 return(0);
}

repos(val) /* put proper value of breaks */
 int val;
{
 register int i;
 nowlug=ftell(obj);
 for(i=0;i<kas[levl];i++) {
  if(i==POSITIONS) end("Too many positions...\n");
  fseek(obj,lpos[levl][i],0);
  fwrite(&val,sizeof(int),1,obj);
 }
 kas[levl]=0;
 fseek(obj,nowlug,0);
}

void clnspc(FILE *fp)
{ int c; do c=getc(fp); while(isspace(c)); ungetc(c,fp); }

#include <stdarg.h>
void end(char *format, ... )
{
	va_list arglist;

	va_start(arglist,format);

	vfprintf(stderr,format,arglist);

	va_end(arglist);

	exit(1);
}
