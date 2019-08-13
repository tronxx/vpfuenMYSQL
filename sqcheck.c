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

/*char *sqfr[] = {
"SQL_ACTIVE_CONNECTIONS",
"SQL_ACTIVE_STATEMENTS",
"SQL_DATA_SOURCE_NAME",
"SQL_DRIVER_HDBC",
"SQL_DRIVER_HENV",
"SQL_DRIVER_HLIB",
"SQL_DRIVER_HSTMT",
"SQL_DRIVER_NAME",
"SQL_DRIVER_ODBC_VER",
"SQL_DRIVER_VER",
"SQL_FETCH_DIRECTION",
"SQL_FILE_USAGE",
"SQL_GETDATA_EXTENSIONS",
"SQL_LOCK_TYPES",
"SQL_ODBC_API_CONFORMANCE",
"SQL_ODBC_SAG_CLI_CONFORMANCE",
"SQL_ODBC_VER",
"SQL_POS_OPERATIONS",
"SQL_ROW_UPDATES",
"SQL_SEARCH_PATTERN_ESCAPE",
"SQL_SERVER_NAME",
"SQL_DATABASE_NAME",
"SQL_DBMS_NAME",
"SQL_DBMS_VER",
"SQL_ACCESSIBLE_PROCEDURES",
"SQL_ACCESSIBLE_TABLES",
"SQL_BOOKMARK_PERSISTENCE",
"SQL_CONCAT_NULL_BEHAVIOR",
"SQL_CURSOR_COMMIT_BEHAVIOR".
"SQL_CURSOR_ROLLBACK_BEHAVIOR",
"SQL_DATA_SOURCE_READ_ONLY",
"SQL_DEFAULT_TXN_ISOLATION",
"SQL_MULT_RESULT_SETS",
"SQL_MULTIPLE_ACTIVE_TXN",
"SQL_NEED_LONG_DATA_LEN",
"SQL_NULL_COLLATION",
"SQL_OWNER_TERM",
"SQL_PROCEDURE_TERM",
"SQL_QUALIFIER_TERM",
"SQL_SCROLL_CONCURRENCY",
"SQL_SCROLL_OPTIONS",
"SQL_STATIC_SENSITIVITY",
"SQL_TABLE_TERM",
"SQL_TXN_CAPABLE",
"SQL_TXN_ISOLATION_OPTION",
"SQL_USER_NAME",
"" };*/



  HENV hEnv;
  HDBC hdbc;
  HSTMT hstmt;
  int ff;
  char tname[256];
  void vSQLGet();
  void ShowRes(char *,SWORD);
RETCODE vChk(RETCODE sqres);
void endsql();
void main(int argc,char *argv[])
{
 char sql1[128];
 char buf[512];
 SWORD byavai;
 UWORD iType;
  if(argc != 4) {
   printf("Use: sqcheck database user passwd\n");
   exit(0);
  }
  vChk(SQLAllocEnv(&hEnv));
  vChk(SQLAllocConnect(hEnv,&hdbc));
  vChk(SQLConnect(hdbc,argv[1],SQL_NTS,argv[2],SQL_NTS,argv[3],SQL_NTS));
  vChk(SQLAllocStmt(hdbc,&hstmt));
  iType = SQLGetInfo(hdbc,SQL_CURSOR_COMMIT_BEHAVIOR,buf,sizeof(buf),&byavai);
  ShowRes(&iType,2);
  iType = SQLGetInfo(hdbc,SQL_CURSOR_ROLLBACK_BEHAVIOR,buf,sizeof(buf),&byavai);
  ShowRes(&iType,2);
  endsql();
}

void ShowRes(char *buf,SWORD byavai)
{
 long *inf; short *sinf;
 inf = (long*)buf;
 sinf = (short*)buf;

  if(byavai == 2) { /* a short value */
   printf("%u (short)\n",sinf);
   return;
  }
  if(byavai == 4) {
   printf("%u (long)\n",inf);
   return;
  }
  if(byavai > 0) {
   printf("%s\n",buf);
   return;
  }
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

/*
SQL_ACTIVE_CONNECTIONS
SQL_ACTIVE_STATEMENTS
SQL_DATA_SOURCE_NAME
SQL_DRIVER_HDBC
SQL_DRIVER_HENV
SQL_DRIVER_HLIB
SQL_DRIVER_HSTMT
SQL_DRIVER_NAME
SQL_DRIVER_ODBC_VER
SQL_DRIVER_VER
SQL_FETCH_DIRECTION
SQL_FILE_USAGE
SQL_GETDATA_EXTENSIONS
SQL_LOCK_TYPES
SQL_ODBC_API_CONFORMANCE
SQL_ODBC_SAG_CLI_CONFORMANCE
SQL_ODBC_VER
SQL_POS_OPERATIONS
SQL_ROW_UPDATES
SQL_SEARCH_PATTERN_ESCAPE
SQL_SERVER_NAME
SQL_DATABASE_NAME
SQL_DBMS_NAME
SQL_DBMS_VER
SQL_ACCESSIBLE_PROCEDURES
SQL_ACCESSIBLE_TABLES
SQL_BOOKMARK_PERSISTENCE
SQL_CONCAT_NULL_BEHAVIOR
SQL_CURSOR_COMMIT_BEHAVIOR
SQL_CURSOR_ROLLBACK_BEHAVIOR
SQL_DATA_SOURCE_READ_ONLY
SQL_DEFAULT_TXN_ISOLATION
SQL_MULT_RESULT_SETS
SQL_MULTIPLE_ACTIVE_TXN
SQL_NEED_LONG_DATA_LEN
SQL_NULL_COLLATION
SQL_OWNER_TERM
SQL_PROCEDURE_TERM
SQL_QUALIFIER_TERM
SQL_SCROLL_CONCURRENCY
SQL_SCROLL_OPTIONS
SQL_STATIC_SENSITIVITY
SQL_TABLE_TERM
SQL_TXN_CAPABLE
SQL_TXN_ISOLATION_OPTION
SQL_USER_NAME

*/
