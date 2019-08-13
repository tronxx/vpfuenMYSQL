/*
  Program to generate the table definition from a SQL Server
  Usage: sqdef table #  ( # = 1 2 3 4 5 to append to fields )
  by Franz J Fortuny April 29 199
*/
#include <stdio.h>

#include "cli0core.h"
#include "cli0defs.h"
//#include "cli0env.h"
#include "cli0ext1.h"
#include "archisq.h"
#include <ctype.h>

  HENV hEnv;
  HDBC hdbc;
  HSTMT hstmt;
  char tname[256],sufx[16],fmtf[40],fmtl[40],fmti[40], sql_z[512], sql2_z[512];
  void vSQLGet();
  void vSQLGet2();
  void vLoCase(char *);
  int upcase=0,labsiz,row,col;
RETCODE vChk(RETCODE sqres);
void endsql();
void main(int argc,char *argv[])
{
 char sql1[128],*ptr;
 int accio = 0,i;
  if(argc == 1) {
   printf("Use: sqdef table_name f# database user passwd -UfilarcL \n");
   exit(0);
  }
  strcpy(fmtf,"%16.2f");
  strcpy(fmtl,"%11ld");
  strcpy(fmti,"%6d");
  row = 0; col = 14; labsiz = 12;
  for(i=0;i<argc;i++) {
   if(*(ptr = argv[i]) == '-') {
    ++ptr;
    switch(*ptr) {
     case 'U': upcase = 1; break;
     case 'f': ++ptr; strcpy(fmtf,ptr); break;
     case 'i': ++ptr; strcpy(fmti,ptr); break;
     case 'l': ++ptr; strcpy(fmtl,ptr); break;
     case 'a': ++ptr; accio = atoi(ptr); break;
     case 'r': ++ptr; row = atoi(ptr); break;
     case 'c': ++ptr; col = atoi(ptr); break;
     case 'L': ++ptr; labsiz = atoi(ptr); break;
    }
   }
  }
  sprintf(sql1,"select * from %s",argv[1]);
  strcpy(tname,argv[1]);
  strcpy(sufx,argv[2]);
  vChk(SQLAllocEnv(&hEnv));
  vChk(SQLAllocConnect(hEnv,&hdbc));
  vChk(SQLConnect(hdbc,argv[3],SQL_NTS,argv[4],SQL_NTS,argv[5],SQL_NTS));
  vChk(SQLAllocStmt(hdbc,&hstmt));
  vChk(SQLExecDirect(hstmt,sql1,SQL_NTS));
  if(!accio) vSQLGet();
  else vSQLGet2();
  endsql();
}

void endsql() {
  SQLFreeStmt(hstmt,SQL_DROP);
  SQLFreeConnect(hdbc);
  SQLFreeEnv(hEnv);
}


RETCODE vChk(RETCODE sqres)

{
  char buf1[256],buf2[256],buf3[256];
  SDWORD sdw1;
  SWORD sw1;
  if(sqres != SQL_SUCCESS && sqres != SQL_SUCCESS_WITH_INFO) {
   sqres = SQLError(hEnv,
                    hdbc,
                    SQL_NULL_HSTMT,
                    buf1, /* SQL State */
                    &sdw1, /* native error code */
                    buf3, /* error message text */
                    sizeof(buf3), /* size of buffer to receive the error */
                    &sw1); /* variable to receive the value of the message */

   printf("Connection to %s is returning error state:\n\
        %s\nNative Err code %d\n\
        Error text:\n%s\n",tname,buf1,sdw1,buf3);
        endsql();
        exit(0);
   }

return sqres;

}

void vSQLGet2() /* get desc. and print them */
{
  UWORD icol;
  UCHAR cname[60];
  SWORD cnamersz,dType,ctype,cscale,ifnul;
  SDWORD actsize;
  UDWORD cprecis,bufsz=0,thisz;
  char *bptr,typ;
  int bind,i;
  printf("* Instruction to display form for %s *\nqf,",tname);
  icol = 1;
  while ( SQLDescribeCol(hstmt,
                        icol,
                        cname,
                        (SWORD) sizeof(cname),
                        &cnamersz,
                        &dType,
                        &cprecis,
                        &cscale,
                        &ifnul) == SQL_SUCCESS ) {
    switch(dType) {
     case SQL_BIT: thisz = 1; break;

     case SQL_LONGVARBINARY:
     case SQL_LONGVARCHAR: thisz = 10; typ = 'M'; break;

     case SQL_DOUBLE:
     case SQL_FLOAT:
     case SQL_DECIMAL:
     case SQL_NUMERIC: thisz = sizeof(double); typ='d'; break;

     case SQL_REAL: thisz = sizeof(float); typ='f'; break;

     case SQL_INTEGER: thisz = sizeof(long); typ = 'l'; break;

     case SQL_SMALLINT: thisz = sizeof(short int); typ='i'; break;

     case SQL_TINYINT: thisz = sizeof(unsigned char); typ= 'C'; break;

     case SQL_DATE: thisz = sizeof(DATE_STRUCT); typ = 'D'; break;
     case SQL_TIME: thisz = sizeof(TIME_STRUCT); typ = 'H'; break;
     case SQL_TIMESTAMP: thisz = sizeof(TIMESTAMP_STRUCT); typ = 'T'; break;

	  case SQL_BIGINT:
	  case SQL_BINARY:
     case SQL_VARBINARY:
     case SQL_CHAR:
     case SQL_VARCHAR: thisz = cprecis + 1; typ='s'; break;
     default: thisz = cprecis + 1; break;
    }
if(!upcase) vLoCase(cname);
 printf("\n%d,0,\"%*.*s\",0,",row,labsiz,labsiz,cname);
 switch(typ) {
  case 's': printf("%d,%d,%s%s,#,",row,col,cname,sufx); break;
  case 'd':case 'f':
    printf("%d,%d,%s%s,\"%s\",",row,col,cname,sufx,fmtf); break;
  case 'l':
    printf("%d,%d,%s%s,\"%s\",",row,col,cname,sufx,fmtl); break;
  case 'i': case 'C':
    printf("%d,%d,%s%s,\"%s\",",row,col,cname,sufx,fmti); break;
  case 'D':
   printf("%d,%d,%s%s,\"+10\",",row,col,cname,sufx); break;
   break;
/*  case 'T':
   printf("\n%s%s { %sYear%s i, %sMon%s i, %sDay%s i, %sHour%s i, %sMin%s i, %sSec%s i, %sFrac%s l, }",
           cname,sufx,cname,sufx,cname,sufx,cname,sufx,cname,sufx,cname,sufx,cname,sufx,cname,sufx);
   break;*/
/*  case 'H':
   printf("\n%s%s { %sHour%s i, %sMin%s i, %sSec%s i, }",
           cname,sufx,cname,sufx,cname,sufx,cname,sufx);
   break;*/
 }
   bufsz = bufsz + thisz;
    icol++; row++;

  }
printf(";\n",icol-1,bufsz);
}

void vLoCase(char *s)
{
  int i=0;
  while(s[i] != 0) { s[i] = tolower(s[i]); i++; }
}

void vSQLGet() /* get desc. and print them */
{
  UWORD icol;
  UCHAR cname[60];
  SWORD cnamersz,dType,ctype,cscale,ifnul;
  SDWORD actsize;
  UDWORD cprecis,bufsz=0,thisz;
  char *bptr,typ;
  int bind,i,ii;
  printf("* Buffer definitions for table %s *\nM",tname);
  sprintf(sql_z,"insert into %s (",tname);
  icol = 1;
  while ( SQLDescribeCol(hstmt,
                        icol,
                        cname,
                        (SWORD) sizeof(cname),
                        &cnamersz,
                        &dType,
                        &cprecis,
                        &cscale,
                        &ifnul) == SQL_SUCCESS ) {
    switch(dType) {
     case SQL_BIT: thisz = 1; break;

     case SQL_LONGVARBINARY:
     case SQL_LONGVARCHAR: thisz = 10; typ = 'M'; break;

     case SQL_DOUBLE:
     case SQL_FLOAT:
     case SQL_DECIMAL:
     case SQL_NUMERIC: thisz = sizeof(double); typ='d'; break;

     case SQL_REAL: thisz = sizeof(float); typ='f'; break;

     case SQL_INTEGER: thisz = sizeof(long); typ = 'l'; break;

     case SQL_SMALLINT: thisz = sizeof(short int); typ='i'; break;

     case SQL_TINYINT: thisz = sizeof(unsigned char); typ= 'C'; break;

     case SQL_DATE: thisz = sizeof(DATE_STRUCT); typ = 'D'; break;
     case SQL_TIME: thisz = sizeof(TIME_STRUCT); typ = 'H'; break;
     case SQL_TIMESTAMP: thisz = sizeof(TIMESTAMP_STRUCT); typ = 'T'; break;

	  case SQL_BIGINT:
	  case SQL_BINARY:
     case SQL_VARBINARY:
     case SQL_CHAR:
     case SQL_VARCHAR: thisz = cprecis + 1; typ='s'; break;
     default: thisz = cprecis + 1; break;
    }
   if(!upcase) vLoCase(cname);
   strcat(sql_z,sprintf("%s,",cname));

 switch(typ) {
  case 's': printf("\n%s%s  s%d,",cname,sufx,thisz);
  	    strcat(sql2_z,sprintf("%s,#,1,",cname)); break;
  case 'd': case 'l': case 'i': case 'f': case 'C': case 'M':
            printf("\n%s%s  %c,",cname,sufx,typ); 
    	    strcat(sql2_z,sprintf("%s,",cname)); break;
  case 'D':
   printf("\n%s%s { %sYear%s i, %sMon%s i, %sDay%s i, }",
           cname,sufx,cname,sufx,cname,sufx,cname,sufx);
           strcat(sql2_z,sprintf("%s,#,3,",cname));
   break;
  case 'T':
   printf("\n%s%s { %sYear%s i, %sMon%s i, %sDay%s i, %sHour%s i, %sMin%s i, %sSec%s i, %sFrac%s l, }",
           cname,sufx,cname,sufx,cname,sufx,cname,sufx,cname,sufx,cname,sufx,cname,sufx,cname,sufx);
   break;
  case 'H':
   printf("\n%s%s { %sHour%s i, %sMin%s i, %sSec%s i, }",
           cname,sufx,cname,sufx,cname,sufx,cname,sufx);
   break;
 }
   bufsz = bufsz + thisz;
    icol++;

  }
  printf(";\n * %d columns (size for buffer = %d)*\n",icol-1,bufsz);
  strcat(sql_z,")\n values (");
  for (ii=0; ii < icol-2; ii++) strcat(sql_z,"?,");
  strcat(sql_z,"?),\n");
  strcat(sql_z,sql2_z);
  printf("*\n%s; *\n",sql_z);
}

