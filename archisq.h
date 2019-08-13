/*
   archisq.h ------ header for the
   structure that VPG programs use for accessing
   multiple tables and cursors from one single program
   By Franz J Fortuny ----  April 28 1997
*/
#include <my_global.h>
#include <mysql.h>

#define MAXSTMT 20

typedef struct archi {
  MYSQL *hdbc;
  MYSQL_STMT *hstmt[20], *stins, *stdel[20], *stupd[20];
  MYSQL_BIND bind_col[60];
  MYSQL_RES *sqres[20];
  MYSQL_BIND bind_param[20][60];
  unsigned long col_tam[60];
  my_bool       col_is_null[60];
  my_bool       col_error[60];
  int iRow[20];
  int ik;
  int numcols_z;
  int numpar_z[20];
  int fRSt[2];
  char *a_buf;
  char *a_dat;
  char *tname;
  int a_sz;
  int cRow[20];
  char modo;
  int bound[20];
  int *szs;
  char curname[32][20];
  char prede[20];
  char premo[20];
  char prein[20];
  char pre[20];
  char *pun[20];
  } ARCHI;
