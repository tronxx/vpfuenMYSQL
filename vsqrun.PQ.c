/*
  vsqrun.c = the Linux VPG4 SQL management data base instructions section
  by Franz J Fortuny  -- April 28, 1997
  for when the SQL Driver does not have ¿SQLExtendedFetch....
  P$...  Modified to generate errors on P$ with UPDATE statement
*/

case 'O':
case 'o': /* open a connection or a table */
 ar[aa].modo = *ptr;
 ++ptr;
 switch(*ptr) {
  case '@': /* connection being established for data base */

  if(!sqinit) {
    sqinit = 1;
    vChk(SQLAllocEnv(&hEnv));
    for(i=0;i<10;i++) hdbc[i]=0;
  }
  vChk(SQLAllocConnect(hEnv,&hdbc[dbh]));

  /* O@,"tcp/ip linux 1313","sa","lux",0 o 1,;*/
  MP; ptr1 = (char*)getad(ptr);
  MP; ptr2 = (char*)getad(ptr);
  MP; ptr3 = (char*)getad(ptr);
  if(SQLConnect(hdbc[dbh],
             (UCHAR *) ptr1, /* source name */
             SQL_NTS,
             (UCHAR *) ptr2, /* user's name */
             SQL_NTS,
             (UCHAR *) ptr3, /* authentif. word */
             SQL_NTS) != SQL_SUCCESS) {
              MP; MP;
              ins = atoi(ptr); goto e1;
             }
  MP;
  if(*ptr == '0')
   vChk(SQLSetConnectOption(hdbc[dbh],SQL_AUTOCOMMIT,
                      SQL_AUTOCOMMIT_OFF));
   break;
  case 't': /* table being connected to */
   MP; ptr1 = (char*)getad(ptr);
  ar[aa].tname = ptr1;
  ar[aa].hdbc = hdbc[dbh];
  sprintf(sql1,"select * from %s",ptr1);
  vChk(SQLAllocStmt(hdbc[dbh],&ar[aa].hstmt[ik]));
/*  vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CURSOR_TYPE,SQL_CURSOR_STATIC));
  vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_MAX_ROWS,1));*/
  if(ar[aa].modo != 'O')
/*   vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CONCURRENCY,SQL_CONCUR_VALUES));
  else*/
   vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CONCURRENCY,SQL_CONCUR_READ_ONLY));
  vChk(SQLPrepare(ar[aa].hstmt[ik],(UCHAR*)sql1,SQL_NTS));
 vChk(SQLExecute(ar[aa].hstmt[ik]));
  ar[aa].cRow[ik] = 0;
//  vChk(SQLRowCount(ar[aa].hstmt[ik], &ar[aa].cRow[ik]));
//  printf("cRow from RowCount = %ld\n",ar[aa].cRow[ik]);
  vSQLBind(aa);
  SQLGetCursorName(ar[aa].hstmt[ik],ar[aa].curname,32,NULL);
 if(ar[aa].modo == 'O'){ vsqinsert(aa);
                         vsqupdate(aa);
                         vsqdelete(aa); }
   break;
 }
break;

case 'I': /*Include the row */
 //vChk(SQLSetPos(ar[aa].hstmt[ik],0,SQL_ADD,SQL_LOCK_NO_CHANGE));
// vbindpar(aa);
 if((sqres = SQLExecute(ar[aa].stins)) != SQL_SUCCESS && 
     sqres != SQL_SUCCESS_WITH_INFO) {
  if(qptr != 0) {ins = qptr; goto e1;}
  else err("Table %s INSERT is failing and\n\
Qgoto; was not programmed.\n",ar[aa].tname);
}
break;

case 'w': /* update the row */
// vChk(SQLSetPos(ar[aa].hstmt[ik],1,SQL_UPDATE,SQL_LOCK_UNLOCK));
/* sqres = vChk2(SQLExecute(ar[aa].stupd[ik]),ar[aa].stupd[ik]);*/
 sqres = SQLExecute(ar[aa].stupd[ik]);
 if(sqres != SQL_SUCCESS && sqres != SQL_SUCCESS_WITH_INFO) {
  if(qptr == 0) {
   err("Table %s UPDATE has failed and\n\
Qgoto; is not programmed\n",ar[aa].tname);
}
  ins = qptr; goto e1;
 }
 // SQLFetch(ar[aa].hstmt[ik]);
break;

case 'D': /* row to be deleted */
// vChk(SQLSetPos(ar[aa].hstmt[ik],1,SQL_DELETE,SQL_LOCK_UNLOCK));
// SQLGetCursorName(ar[aa].hstmt[ik],ar[aa].curname,32,NULL);
if(ar[aa].modo == 'O') {
// sprintf(sql1,"delete from %s where current of %s",
//               ar[aa].tname,ar[aa].curname);
 vChk(SQLExecute(ar[aa].stdel[ik]));
 }
 else err("This table (%s) was opened in read only mode. Can't delete.\n",
           ar[aa].tname);
break;

case 'r': tolock = lock_r; goto don;
case 'N': tolock = lock_N; goto don;
case 'n': /* do the buffer filling */
 tolock = 0;
don:
 switch (from) {
  case 0:
/*   sqres = SQLSetPos(ar[aa].hstmt[ik],1,SQL_REFRESH,SQL_LOCK_NO_CHANGE);
   goto chlock;*/
   sqres = SQL_SUCCESS;
   break;
  case from_no:
   sqres = SQL_NO_DATA_FOUND;
   break;
  case from_a:
   sqres = SQLFetch(ar[aa].hstmt[ik]);
  break;
  case from_b:
   sqres = SQLFetchPrev(ar[aa].hstmt[ik]);
  break;
  case from_T:
   fFety = SQL_FETCH_FIRST;
   break;
  case from_l:
   fFety = SQL_FETCH_LAST;
   break;
 } /* end of switch from */
/*   sqres = SQLExtendedFetch(ar[aa].hstmt[ik],
                            fFety,
                            0,
                            &udw1,
                            ar[aa].fRSt);*/
chlock: from = 0;
 if(sqres != SQL_SUCCESS) {
  if(sqres == SQL_NO_DATA_FOUND) { ++ptr; ins = *(int*)ptr; from=from_no; goto e1; }
  else { vChk(sqres); }
 }
/* if(tolock) {
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
 } */
break;

case 'f': /* f1; fd0-9; */
 ++ptr;
 if(*ptr == 'd') dbh = atoi(ptr+1);
 else {aa = atoi(ptr);
       ik = ar[aa].ik; }
break;

case 'a': /* Advance */
 from = from_a;
break;

case 'b': /* previous */
 from = from_b;
break;

/*case 'T':
 vChk(SQLFreeStmt(ar[aa].hstmt[ik],SQL_CLOSE));
 vChk(SQLExecute(ar[aa].hstmt[ik]));
 from = from_a;
 break;*/

case 'S':
 ptr1 = getad(ptr+1);
 MP;
 ptr2 = getad(ptr);
 sprintf(sql1,"select * from %s where %s like '%s'",
          ar[aa].tname,ptr2,ptr1);
 vChk(SQLFreeStmt(ar[aa].hstmt[ik],SQL_CLOSE));
 vChk(SQLExecDirect(ar[aa].hstmt[ik],sql1,SQL_NTS));
 if(!ar[aa].bound[ik]) { vSQLBind(aa); ar[aa].bound[ik]= 1; }
 from = from_a;
 break;
case 'T':
case 'P': /* get record (s) with key */
 ++ptr;
 vChk(SQLFreeStmt(ar[aa].hstmt[ik],SQL_CLOSE));
 if(*ptr != '$') {
   vChk(SQLExecute(ar[aa].hstmt[ik]));
   if(!ar[aa].stupd[ik] && ar[aa].modo == 'O') {
     vChk(SQLExecute(ar[aa].hstmt[ik]));
     SQLGetCursorName(ar[aa].hstmt[ik],ar[aa].curname,32,NULL);
     vsqupdate(aa);
     vsqdelete(aa);
   }
  }
  else {
   sqres = SQLExecute(ar[aa].hstmt[ik]);
   if(sqres != SQL_SUCCESS && sqres != SQL_SUCCESS_WITH_INFO) {
   if(qptr == 0) {
     err("Table %s UPDATE has failed and\n Qgoto; is not programmed\n",ar[aa].tname);
   }
   ins = qptr; goto e1;
  }
  }
/* ptr1 = getad(ptr+1);
 sprintf(sql1,"select * from %s where nombre like '%s%%'",
          ar[aa].tname,ptr1);
 vChk(SQLExecDirect(ar[aa].hstmt[ik],sql1,SQL_NTS));
 if(!ar[aa].bound[ik]) { vSQLBind(aa); ar[aa].bound[ik]= 1; }*/
 from = from_a;
 break;

case 'k': /* cursor prepare instruction */
 ++ptr;
 if(isdigit(*ptr)) {
  ar[aa].ik = atoi(ptr);
  ik = ar[aa].ik;
  SQLGetCursorName(ar[aa].hstmt[ik],ar[aa].curname,32,NULL);
  break;
 }
 if(*ptr == 'q') {
  /* prepare the cursor action  kp,"     " or sVar,130,sVar1,5,iVar,2;*/
  MP;
  ptr1 = (char*)getad(ptr);
  vChk(SQLAllocStmt(ar[aa].hdbc,&ar[aa].hstmt[ik]));
  vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CURSOR_TYPE,SQL_CURSOR_STATIC));
  if(ar[aa].modo != 'O')
   /*vChk(
   SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CONCURRENCY,SQL_CONCUR_VALUES));
  else*/
   vChk(
   SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CONCURRENCY,SQL_CONCUR_READ_ONLY));
//  sprintf(sql1,"select * from %s where ",ar[aa].tname);
//  strcat(sql1,ptr1);
  vChk(SQLPrepare(ar[aa].hstmt[ik],(UCHAR*)ptr1,SQL_NTS));
  ar[aa].bound[ik] = 0;
  vSQLBind(aa);
  if(*ptr == '"') { while(*ptr++ != 0); ++ptr; }
  else MP;
  iPar = 1;
  do {
   ptr1 = (char*)getad(ptr);
   i = 8;
   switch(*ptr) {
    case 's':
     MP;
     cbSize = atoi(ptr);
     i = SQL_NTS;
     MP;
     switch(*ptr) {
      case '1': fSqtype = SQL_VARCHAR; fCtype = SQL_C_CHAR; break;
      case '2': fSqtype = SQL_TIMESTAMP; fCtype = SQL_C_TIMESTAMP; break;
      case '3': fSqtype = SQL_DATE; fCtype = SQL_C_DATE; break;
      case '4': fSqtype = SQL_TIME; fCtype = SQL_C_TIME; break;
      default: err("Bad data type - instr # %d\n",ins); break;
     }
     break;
    case 'd': fCtype = SQL_C_DOUBLE; fSqtype = SQL_FLOAT; break;
    case 'f': fCtype = SQL_C_FLOAT; fSqtype = SQL_REAL; break;
    case 'D':
    case 'l': fCtype = SQL_C_LONG; fSqtype = SQL_INTEGER; break;
    case 'i': fCtype = SQL_C_SSHORT; fSqtype = SQL_SMALLINT; break;
    case ';': goto yakqp;
    default: err("Bad data type %c",*ptr); break;
   }
/*   MP;
   ptr2 = (char*)getad(ptr);
   *(SDWORD*)ptr2 = i;*/
   vChk(SQLBindParameter(ar[aa].hstmt[ik],
      iPar,  /* NO. of parameter */
      SQL_PARAM_INPUT, /* flag to Parameter type */
      fCtype, /* C data type of parameter */
      fSqtype, /*  sql server data type of parameter */
      cbSize-1, /*size of column (precision) */
      0, /* scale of column */
      ptr1, /* where the data is located */
      cbSize, /* maxim. ptr1 buffer size */
      &bk )); /* param. length size */
   iPar++;
   MP;
   } while(*(ptr) != ';');
   break;
 }
 if(*ptr == 'p') {
  /* prepare the cursor action  kp,"     " or sVar,130,sVar1,5,iVar,2;*/
  MP;
  ptr1 = (char*)getad(ptr);
  vChk(SQLAllocStmt(ar[aa].hdbc,&ar[aa].hstmt[ik]));
  vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CURSOR_TYPE,SQL_CURSOR_STATIC));
  if(ar[aa].modo != 'O')
   /*vChk(
   SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CONCURRENCY,SQL_CONCUR_VALUES));
  else*/
   vChk(
   SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CONCURRENCY,SQL_CONCUR_READ_ONLY));
  sprintf(sql1,"select * from %s where ",ar[aa].tname);
  strcat(sql1,ptr1);
  vChk(SQLPrepare(ar[aa].hstmt[ik],(UCHAR*)sql1,SQL_NTS));
  ar[aa].bound[ik] = 0;
  vSQLBind(aa);
  if(*ptr == '"') { while(*ptr++ != 0); ++ptr; }
  else MP;
  iPar = 1;
  do {
   ptr1 = (char*)getad(ptr);
   i = 8;
   switch(*ptr) {
    case 's':
     MP;
     cbSize = atoi(ptr);
     i = SQL_NTS;
     MP;
     switch(*ptr) {
      case '1': fSqtype = SQL_VARCHAR; fCtype = SQL_C_CHAR; break;
      case '2': fSqtype = SQL_TIMESTAMP; fCtype = SQL_C_TIMESTAMP; break;
      case '3': fSqtype = SQL_DATE; fCtype = SQL_C_DATE; break;
      case '4': fSqtype = SQL_TIME; fCtype = SQL_C_TIME; break;
      default: err("Bad data type - instr # %d\n",ins); break;
     }
     break;
    case 'd': fCtype = SQL_C_DOUBLE; fSqtype = SQL_FLOAT; break;
    case 'f': fCtype = SQL_C_FLOAT; fSqtype = SQL_REAL; break;
    case 'D':
    case 'l': fCtype = SQL_C_LONG; fSqtype = SQL_INTEGER; break;
    case 'i': fCtype = SQL_C_SSHORT; fSqtype = SQL_SMALLINT; break;
    case ';': goto yakqp;
    default: err("Bad data type %c",*ptr); break;
   }
/*   MP;
   ptr2 = (char*)getad(ptr);
   *(SDWORD*)ptr2 = i;*/
   vChk(SQLBindParameter(ar[aa].hstmt[ik],
      iPar,  /* NO. of parameter */
      SQL_PARAM_INPUT, /* flag to Parameter type */
      fCtype, /* C data type of parameter */
      fSqtype, /*  sql server data type of parameter */
      cbSize-1, /*size of column (precision) */
      0, /* scale of column */
      ptr1, /* where the data is located */
      cbSize, /* maxim. ptr1 buffer size */
      &bk )); /* param. length size */
   iPar++;
   MP;
   } while(*(ptr) != ';');
 }
yakqp:
break;
case 'C': // end the connections hstmt[10],stins,stdel[10],stupd[10];
if(*(ptr+1) == 'D') // Disconnect from active database
{
 vChk(SQLDisconnect(hdbc[dbh]));
 vChk(SQLFreeConnect(hdbc[dbh]));
 hdbc[dbh]=0;
 break;
}
 for(i=0;i<MAXSTMT;i++) {
#define STXX ar[aa].hstmt[i]
  if(STXX != NULL) {
   SQLFreeStmt(STXX,SQL_UNBIND);
   SQLFreeStmt(STXX,SQL_RESET_PARAMS);
   SQLFreeStmt(STXX,SQL_DROP);
  }
#undef STXX
#define STXX ar[aa].stdel[i]
  if(STXX != NULL) {
   SQLFreeStmt(STXX,SQL_UNBIND);
   SQLFreeStmt(STXX,SQL_RESET_PARAMS);
   SQLFreeStmt(STXX,SQL_DROP);
  }
#undef STXX
#define STXX ar[aa].stupd[i]
  if(STXX != NULL) {
   SQLFreeStmt(STXX,SQL_UNBIND);
   SQLFreeStmt(STXX,SQL_RESET_PARAMS);
   SQLFreeStmt(STXX,SQL_DROP);
  }
#undef STXX
 }
#define STXX ar[aa].stins
 if(STXX != NULL) {
   SQLFreeStmt(STXX,SQL_UNBIND);
   SQLFreeStmt(STXX,SQL_RESET_PARAMS);
   SQLFreeStmt(STXX,SQL_DROP);
 }
#undef STXX
 if(ar[aa].a_buf != NULL) free(ar[aa].a_buf);
 temphdbc = ar[aa].hdbc;
 memset(&ar[aa],0,sizeof(ARCHI));
 ar[aa].hdbc = temphdbc;
break;

case 'q':
 ++ptr;
 switch(*ptr) {
  case 'f': --ptr;
   if( vform(ptr) == -1) if(inp != 0) {ins = inp; goto e1; }
   break;
  case '+':
   ++ptr; ++ptr; ptr1 = (char*)getad(ptr); MP;
   while(*ptr != ';') {
    strcat(ptr1,vRetString(ptr));
    if(*ptr == '"') { while(*ptr++ != 0); ++ptr; }
    else while(*ptr++ != ',');
   }
   break;
  case 'x':
   ++ptr; ++ptr;
   ptr1 = (char*)getad(ptr);
   if(!ar[aa].hstmt[ik])
     vChk(SQLAllocStmt(hdbc[dbh],&ar[aa].hstmt[ik]));
   vChk(SQLFreeStmt(ar[aa].hstmt[ik],SQL_CLOSE));
   vChk(SQLExecDirect(ar[aa].hstmt[ik],ptr1,SQL_NTS));
   if(!ar[aa].bound[ik]) { vSQLBind(aa); ar[aa].bound[ik]= 1; }
   from = from_a;
   break;
  case 'T': /* qT,SUBRUT,; */
T1:  ++ptr; ++ptr;
	  sur[nivo++] = ins + 1;
	  ins = atoi(ptr);
	  goto e1;
  case 'C': /* commit trans */
/*   vChk(SQLTransact(hEnv,SQL_NULL_HDBC,SQL_COMMIT));*/
   sqres = SQLTransact(SQL_NULL_HENV,hdbc[dbh],SQL_COMMIT);
   if(sqres == SQL_SUCCESS || sqres == SQL_SUCCESS_WITH_INFO)
     goto T1;
   if(!qptr)
     err("A commit operation has failed and Q..; is invalid\n");
   ins = qptr; goto e1;
  case 'R': /* rollback trans */
   vChk(SQLTransact(SQL_NULL_HENV,hdbc[dbh],SQL_ROLLBACK));
   goto T1;
  case 'D': /* Date related instructions */
   MP;
   i = atoi(ptr);
   MP; ptr1 = (char*)getad(ptr);
   MP; ptr2 = (char*)getad(ptr);
   switch(i) {
    case 1: /* stFecha,DFecha de sql a vpg*/
     memset(&tms,0,sizeof(struct tm));
     tms.tm_mday = *(short*)(ptr1+4);
     tms.tm_mon = *(short*)(ptr1+2) - 1;
     tms.tm_year = *(short*)ptr1 - 1900;
     *(long*) ptr2 = mktime(&tms);
    break;
    case 2: /* stFecha,DFecha de vpg a sql */
     memcpy(&tms,localtime((long*)ptr2),sizeof(struct tm));
     *(short*)(ptr1+4) = tms.tm_mday;
     *(short*)(ptr1+2) = tms.tm_mon + 1;
     *(short*)ptr1 = tms.tm_year + 1900;
    break;
    case 3: /* stTimeStamp,DFecha de sql a vpg */
     memset(&tms,0,sizeof(struct tm));
     tms.tm_sec = *(short*)(ptr1+10);
     tms.tm_min = *(short*)(ptr1+8);
     tms.tm_hour = *(short*)(ptr1+6);
     tms.tm_mday = *(short*)(ptr1+4);
     tms.tm_mon = *(short*)(ptr1+2) - 1;
     tms.tm_year = *(short*)ptr1 - 1900;
     *(long*) ptr2 = mktime(&tms);
    break;
    case 4: /* stTimeStamp,DFecha de vpg a sql */
     memcpy(&tms,localtime((long*)ptr2),sizeof(struct tm));
     *(short*)(ptr1+10) = tms.tm_sec;
     *(short*)(ptr1+8) = tms.tm_min;
     *(short*)(ptr1+6) = tms.tm_hour;
     *(short*)(ptr1+4) = tms.tm_mday;
     *(short*)(ptr1+2) = tms.tm_mon + 1;
     *(short*)ptr1 = tms.tm_year + 1900;
    break;
    case 5:
     sprintf(ptr2,"%04d-%02d-%02d",
     *(short*)ptr1,*(short*)(ptr1+2),*(short*)(ptr1+4));
    break;
    case 6:
     sprintf(ptr2,"%04d-%02d-%02d %02d:%02d:%02d",
     *(short*)ptr1,*(short*)(ptr1+2),*(short*)(ptr1+4),
     *(short*)(ptr1+6),*(short*)(ptr1+8),*(short*)(ptr1+10));
    break;
    case 7:
     sprintf(ptr2,"%04d%02d%02d",
     *(short*)ptr1,*(short*)(ptr1+2),*(short*)(ptr1+4));
    break;
    case 8:
     sprintf(ptr2,"%02d-%3.3s-%04d",
     *(short*)(ptr1+4),mt+((*(short*)(ptr1+2)-1)*3),*(short*)ptr1);
    break;
   }
  break;
 }
break;
