/*
  vsqrun.c = the Linux VPG4 SQL management data base instructions section
  by Franz J Fortuny  -- April 28, 1997
*/

case 'O':
case 'o': /* open a connection or a table */
 ar[aa].modo = *ptr;
 ++ptr;
 if(*ptr == '@') { /* connection being established for data base */
  ++ptr;

  if(!sqinit) {
    sqinit = 1;
    vChk(SQLAllocEnv(&hEnv));
  }
  vChk(SQLAllocConnect(hEnv,&hdbc[dbh]));

  /* O@tcp/ip linux 1313~,sa~,lux~; */

  ptr1 = ptr; MP;
  ptr2 = ptr; MP;
  vChk(SQLConnect(hdbc[dbh],
             (UCHAR *) ptr1, /* source name */
             SQL_NTS,
             (UCHAR *) ptr2, /* user's name */
             SQL_NTS,
             (UCHAR *) ptr, /* authentif. word */
             SQL_NTS));

  break;
 } /* if data base connection */

 else {
  if(*ptr == '(') {
   ++ptr;
   ptr1 = (char*)getad(ptr);
  }
  else {
   ptr1 = ptr;
  }
  ar[aa].tname = ptr1;
  ar[aa].hdbc = hdbc[dbh];
  sprintf(sql1,"select * from %s",ptr1);
  vChk(SQLAllocStmt(hdbc[dbh],&ar[aa].hstmt[ik]));
  vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CURSOR_TYPE,SQL_CURSOR_STATIC));
  if(ar[aa].modo == 'O')
   vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CONCURRENCY,SQL_CONCUR_LOCK));
  else
   vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CONCURRENCY,SQL_CONCUR_READ_ONLY));
  vChk(SQLExecDirect(ar[aa].hstmt[ik],(UCHAR*)sql1,SQL_NTS));
  vChk(SQLRowCount(ar[aa].hstmt[ik], &ar[aa].cRow[ik]));
  vSQLBind(aa);
 } /* to actually open a table */
break;

case 'I': /*Include the row */
 vChk(SQLSetPos(ar[aa].hstmt[ik],0,SQL_ADD,SQL_LOCK_NO_CHANGE));
break;

case 'w': /* update the row */
 vChk(SQLSetPos(ar[aa].hstmt[ik],1,SQL_UPDATE,SQL_LOCK_UNLOCK));
break;

case 'D': /* row to be deleted */
 vChk(SQLSetPos(ar[aa].hstmt[ik],1,SQL_DELETE,SQL_LOCK_UNLOCK));
break;

case 'r': tolock = lock_r; goto don;
case 'N': tolock = lock_N; goto don;
case 'n': /* do the buffer filling */
 tolock = 0;
don:
 switch (from) {
  case 0:
   sqres = SQLSetPos(ar[aa].hstmt[ik],1,SQL_REFRESH,SQL_LOCK_NO_CHANGE);
   goto chlock;
  case from_a:
   fFety = SQL_FETCH_NEXT;
  break;
  case from_b:
   fFety = SQL_FETCH_PRIOR;
  break;
  case from_T:
   fFety = SQL_FETCH_FIRST;
   break;
  case from_l:
   fFety = SQL_FETCH_LAST;
   break;
 } /* end of switch from */
   sqres = SQLExtendedFetch(ar[aa].hstmt[ik],
                            fFety,
                            0,
                            &udw1,
                            ar[aa].fRSt);
chlock: from = 0;
 if(sqres != SQL_SUCCESS) {
  if(sqres == SQL_NO_DATA_FOUND) { ++ptr; ins = *(int*)ptr; goto e1; }
  else vChk(sqres);
 }
 if(tolock) {
  switch(tolock) {
   case lock_r:
    sqres = SQLSetPos(ar[aa].hstmt[ik],1,SQL_REFRESH,SQL_LOCK_EXCLUSIVE);
    if(sqres == SQL_ERROR) { ins = *(int*)qptr; goto e1; }
    break;
   case lock_N:
    Iagain:
    sqres = SQLSetPos(ar[aa].hstmt[ik],1,SQL_REFRESH,SQL_LOCK_EXCLUSIVE);
    if(sqres == SQL_ERROR) { sleep(1); goto Iagain; }
    break;
  }
 }
break;

case 'f': /* f1; fd0-9; */
 ++ptr;
 if(*ptr == 'd') dbh = atoi(ptr+1);
 else {aa = atoi(ptr+1);
       ik = ar[aa].ik; }
break;

case 'a': /* Advance */
 ++ptr;
 if(*ptr == 'l') from = from_l;
 else from = from_a;
break;

case 'b': /* previous */
 from = from_b;
break;

case 'T': /* first one */
 from = from_T;
 break;

case 'S':
case 'P': /* get record (s) with key */
 vChk(SQLFreeStmt(ar[aa].hstmt[ik],SQL_CLOSE));
 vChk(SQLExecute(ar[aa].hstmt[ik]));
 if(!ar[aa].bound[ik]) { vSQLBind(aa); ar[aa].bound[ik]= 1; }
 from = from_a;
 break;

case 'k': /* cursor prepare instruction */
 ++ptr;
 if(*ptr != 'p') {
  ar[aa].ik = atoi(ptr);
  ik = ar[aa].ik;
  break;
 }
 else {
  /* prepare the cursor action  kp,"     " or sVar,130,sVar1,5,iVar,2;*/
  MP;
  ptr1 = (char*)getad(ptr);
  vChk(SQLAllocStmt(ar[aa].hdbc,&ar[aa].hstmt[ik]));
  vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CURSOR_TYPE,SQL_CURSOR_STATIC));
  if(ar[aa].modo == 'O')
   vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CONCURRENCY,SQL_CONCUR_LOCK));
  else
   vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CONCURRENCY,SQL_CONCUR_READ_ONLY));
  vChk(SQLPrepare(ar[aa].hstmt[ik],(UCHAR*)ptr1,(SDWORD)strlen(ptr1)));
  if(*ptr == '"') { while(*ptr++ != 0); ++ptr; }
  else MP;
  iPar = 0;
  while(*ptr != ';') {
   cbSize = 0;
   switch(*ptr) {
    case '1': fCtype = SQL_C_CHAR; fSqtype = SQL_VARCHAR; break;
    case '2': fCtype = SQL_C_DOUBLE; fSqtype = SQL_FLOAT; break;
    case '3': fCtype = SQL_C_FLOAT; fSqtype = SQL_REAL; break;
    case '4': fCtype = SQL_C_LONG; fSqtype = SQL_INTEGER; break;
    case '5': fCtype = SQL_C_SSHORT; fSqtype = SQL_SMALLINT; break;
    default: err("Bad data type %c",*ptr); break;
   }
   if(*ptr == '1') cbSize = atoi(ptr+1);
   MP;
   ptr1 = (char*)getad(ptr);
   vChk(SQLBindParameter(ar[aa].hstmt[ik],
      iPar,  /* NO. of parameter */
      SQL_PARAM_INPUT, /* flag to Parameter type */
      fCtype, /* C data type of parameter */
      fSqtype, /*  sql server data type of parameter */
      cbSize, /*size of column (precision) */
      0, /* scale of column */
      ptr1, /* where the data is located */
      0, /* maxim. ptr1 buffer size */
      &sdw1)); /* param. length size */
   iPar++;
   MP;
  }
  ar[aa].bound[ik] = 0;
 }
 break;
