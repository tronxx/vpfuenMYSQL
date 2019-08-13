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

  HENV hEnv;
  HDBC hdbc;
  HSTMT hstmt;
  int ff; 
  char tname[256];
  void vSQLGet();

RETCODE vChk(RETCODE sqres);
void endsql();
void main(int argc,char *argv[])
{
 char sql1[128];
  if(argc != 6) {
   printf("Use: sqdef table_name f# database user passwd\n");
   exit(0);
  }
  sprintf(sql1,"select * from %s",argv[1]);
  ff = atoi(argv[2]);
  strcpy(tname,argv[1]);
  vChk(SQLAllocEnv(&hEnv));
  vChk(SQLAllocConnect(hEnv,&hdbc));
  vChk(SQLConnect(hdbc,argv[3],SQL_NTS,argv[4],SQL_NTS,argv[5],SQL_NTS));
  vChk(SQLAllocStmt(hdbc,&hstmt));
  vChk(SQLExecDirect(hstmt,sql1,SQL_NTS));
  vSQLGet();
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

void vSQLGet() /* get desc. and print them */
{
  UWORD icol;
  UCHAR cname[60];
  SWORD cnamersz,dType,ctype,cscale,ifnul;
  SDWORD actsize;
  UDWORD cprecis,bufsz=0,thisz;
  char *bptr,typ;
  int bind;
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

     case SQL_DATE:
     case SQL_TIME:
     case SQL_TIMESTAMP:
	  case SQL_BIGINT:
	  case SQL_BINARY:
     case SQL_VARBINARY:
     case SQL_CHAR:
     case SQL_VARCHAR: thisz = cprecis + 1; typ='s'; break;
     default: thisz = cprecis + 1; break;
    }
if(typ == 's') printf("s%d,",thisz-1);
else printf("%c,",typ);
   bufsz = bufsz + thisz;
    icol++;

  }
  printf(";\n");
}
