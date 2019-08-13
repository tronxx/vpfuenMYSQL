/*
  This programa checks the error status of a connection
  returns if no error  - -  reports error and exits.
  by Franz J Fortuny April 29 199
*/
#include <stdio.h>

#include "cli0core.h"
#include "cli0defs.h"
//#include "cli0env.h"
#include "cli0ext1.h"
#include "archisq.h"

  extern HENV hEnv;
  extern ARCHI *ar;
  extern int ik;

void err(char *, ...);

RETCODE vChk(RETCODE sqres)

{
  char buf1[256],buf2[256],buf3[256];
  SDWORD sdw1;
  SWORD sw1;
  extern int aa;
  if(sqres != SQL_SUCCESS && sqres != SQL_SUCCESS_WITH_INFO) {
   sqres = SQLError(hEnv,
                    ar[aa].hdbc,
                    SQL_NULL_HSTMT,
                    buf1, /* SQL State */
                    &sdw1, /* native error code */
                    buf3, /* error message text */
                    sizeof(buf3), /* size of buffer to receive the error */
                    &sw1); /* variable to receive the value of the message */

   err("Connection to %s is returning error\
        State: %s -- Native Err code %d\
        Error text: %s",ar[aa].tname,buf1,sdw1,buf3);
   }

return sqres;

}

void vSQLBind(int aa) /* get desc. and bind columns */
{
  UWORD icol;
  UCHAR cname[60];
  SWORD cnamersz,dType,ctype,cscale,ifnul;
  SDWORD actsize;
  UDWORD cprecis,bufsz=0,thisz;
  char *bptr;
  int bind;

  icol = 1;
  while ( SQLDescribeCol(ar[aa].hstmt[ik],
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
     case SQL_LONGVARCHAR: thisz = 10; break;

     case SQL_DOUBLE:
     case SQL_FLOAT:
     case SQL_DECIMAL:
     case SQL_NUMERIC: thisz = sizeof(double); break;

     case SQL_REAL: thisz = sizeof(float); break;

     case SQL_INTEGER: thisz = sizeof(long); break;

     case SQL_SMALLINT: thisz = sizeof(short int); break;

     case SQL_TINYINT: thisz = sizeof(unsigned char); break;

     case SQL_DATE:
     case SQL_TIME:
     case SQL_TIMESTAMP:
	  case SQL_BIGINT:
	  case SQL_BINARY:
     case SQL_VARBINARY:
     case SQL_CHAR:
     case SQL_VARCHAR: thisz = cprecis + 1; break;
     default: thisz = cprecis + 1; break;
    }
    bufsz = bufsz + thisz;
    icol++;
  }

  if(ar[aa].a_sz < bufsz) {
   if(ar[aa].a_buf != NULL)
     { free(ar[aa].a_buf); ar[aa].a_buf = NULL; }
    if((ar[aa].a_buf = (char *) calloc(bufsz,sizeof(char))) == NULL)
     err("Can't get memory for table # %d, key %d, (%s)",aa,ik,ar[aa].tname);
    ar[aa].a_sz = bufsz;
    ar[aa].a_dat = ar[aa].a_buf;
  }
   
  bptr = ar[aa].a_buf;
  icol = 1;
  while (SQLDescribeCol(ar[aa].hstmt[ik],
                        icol,
                        cname,
                        (SWORD) sizeof(cname),
                        &cnamersz,
                        &dType,
                        &cprecis,
                        &cscale,
                        &ifnul) == SQL_SUCCESS ) {
    bind = 1;
    switch(dType) {
     case SQL_BIT: thisz = 1; ctype = SQL_C_CHAR; break;

     case SQL_LONGVARBINARY:
     case SQL_LONGVARCHAR: thisz = 10; bind = 0; break;

     case SQL_DOUBLE:
     case SQL_FLOAT:
     case SQL_DECIMAL:
     case SQL_NUMERIC:
      thisz = sizeof(double);
      ctype = SQL_C_DOUBLE;
      break;

     case SQL_REAL:
      thisz = sizeof(float);
      ctype = SQL_C_FLOAT;
      break;

     case SQL_INTEGER:
      thisz = sizeof(long);
      ctype = SQL_C_SLONG;
      break;

     case SQL_SMALLINT:
      thisz = sizeof(short int);
      ctype = SQL_C_SSHORT;
      break;

     case SQL_TINYINT: thisz = sizeof(unsigned char);
      ctype = SQL_C_CHAR;
      break;

     case SQL_DATE:
     case SQL_TIME:
     case SQL_TIMESTAMP:
	  case SQL_BIGINT:
	  case SQL_BINARY:
     case SQL_VARBINARY:
     case SQL_CHAR:
     case SQL_VARCHAR:
      thisz = cprecis + 1;
      ctype = SQL_C_CHAR;
      break;

     default:
      thisz = cprecis + 1;
      ctype = SQL_C_CHAR;
      break;
    }

    if(bind) {
     vChk(SQLBindCol(ar[aa].hstmt[ik],
                   icol,
                   ctype,
                   bptr,
                   thisz,
                   &actsize));
    }
    bptr = bptr + thisz;
    printf("Columna # %3d tamaño %3d %s\n",icol,thisz,cname);
    icol++;

   }
}
