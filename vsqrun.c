/*
  vsqrun0.c = the Linux VPG4 SQL management data base instructions section
  by Franz J Fortuny  -- April 28, 1997
  for when the SQL Driver does not have �SQLExtendedFetch....
  Modified: Oct 31 1998 -- Prepare the statements WHEN NEEDED
*/

case 'O':
case 'o': /* open a connection or a table */
 ar[aa].modo = *ptr;
 ++ptr;
 switch(*ptr) {
  case '@': /* connection being established for data base */

    if(!sqinit) {
      sqinit = 1;
      temphdbc = mysql_init(NULL);
      hdbc[dbh]  = mysql_init(NULL);
      //vChk(SQLAllocEnv(&hEnv));
      //for(i=0;i<10;i++) {
		  // hdbc[i] = mysql_init(NULL);
		  // if (hdbc[i]  == NULL) {
      //       fprintf(stderr, "%s\n", mysql_error(hdbc[i]));
      //       exit(1);
      //     }
	    //}
    }
    //vChk(SQLAllocConnect(hEnv,&hdbc[dbh]));

    /* O@,"DataBaseName","usr","pwd","HostName",0 o 1;*/
    MP; ptr1 = (char*)getad(ptr);
    MP; ptr2 = (char*)getad(ptr);
    MP; ptr3 = (char*)getad(ptr);
    MP; ptr4 = (char*)getad(ptr);
    MP; ptr5 = (char*)getad(ptr);
#ifdef DEBUG
                      fprintf(debug, "DbName: %s Usr:%s Pwd:%s, Hostname:%s\n", ptr1, ptr2, ptr3, ptr4);
                      fflush(debug);
#endif
    if(mysql_real_connect(hdbc[dbh],
             (UCHAR *) ptr4, /* Host name */

             (UCHAR *) ptr2, /* user's name */

             (UCHAR *) ptr3, /* authentif. word */
             (UCHAR *) ptr1, /* DatabaseName */
                0, NULL,
             0) == NULL) {
                MP; MP;
              ins = atoi(ptr); goto e1;
    }
#ifdef DEBUG
                      fprintf(debug, "Se Realizo la Conexion a BD: %s Usr:%s Pwd:%s\n", ptr1, ptr2, ptr3);
                      fflush(debug);
#endif

    MP;
    //if(*ptr == '0')
    //vChk(SQLSetConnectOption(hdbc[dbh],SQL_AUTOCOMMIT,
    //                   SQL_AUTOCOMMIT_OFF));
    break;
  case 't': /* table being connected to */
    MP; ptr1 = (char*)getad(ptr);
    memset((void *)&ar[aa],0,sizeof(struct archi));
#ifdef DEBUG
                      fprintf(debug, "Se va a abrir Tabla: %s\n", ptr1);
#endif
    ar[aa].tname = ptr1;
    ar[aa].hdbc = hdbc[dbh];
    ar[aa].cRow[ik] = 0;

    // --> Bloqueo que haga el Stmt_Prepare de ot por que puede cambiar las direcciones
    break;
    sprintf(sql1,"select * from %s",ptr1);
    //vChk(SQLAllocStmt(hdbc[dbh],&ar[aa].hstmt[ik]));
    if(ar[aa].modo != 'O')
    //vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CONCURRENCY,SQL_CONCUR_READ_ONLY));
#ifdef DEBUG
                      fprintf(debug, "Se va a prepaparar Stmt aa:%d ik:%d: sql: %s\n", aa, ik, sql1);
                      fflush(debug);
#endif
    ar[aa].hstmt[ik] = mysql_stmt_init(ar[aa].hdbc);
    mysql_stmt_prepare(ar[aa].hstmt[ik],sql1,strlen(sql1));
#ifdef DEBUG
                      fprintf(debug, "--Se prepaparo Stmt aa:%d ik:%d: sql: %s\n", aa, ik, sql1);
                      fflush(debug);
#endif
/*  vChk(SQLExecute(ar[aa].hstmt[ik]));*/
    ar[aa].cRow[ik] = 0;
  //mysql_stmt_bind_result (aa);

    ar[aa].pre[ik]=1;
/*  SQLGetCursorName(ar[aa].hstmt[ik],ar[aa].curname[ik],32,NULL);
    vChk(SQLFreeStmt(ar[aa].hstmt[ik],SQL_CLOSE));*/
    break;
   }
#ifdef DEBUG
                      fprintf(debug, "Sali del case\n");
                      fflush(debug);
#endif

break;

case 'I': /*Include the row */
 //vChk(SQLSetPos(ar[aa].hstmt[ik],0,SQL_ADD,SQL_LOCK_NO_CHANGE));
 // vbindpar(aa);
 if(ar[aa].modo != 'O')
    err("Table %s f%d open for read only.\n",ar[aa].tname,aa);
 if(ar[aa].prein[ik] == 0) { /* prepare statement if it is not */
    //vsqinsert(aa);
    ar[aa].prein[ik] = 1;
 }
 if(qptr != 0) { ins = qptr; goto e1; }
 //if((sqres = SQLExecute(ar[aa].stins)) != SQL_SUCCESS && sqres != SQL_SUCCESS_WITH_INFO) {
 //   if(qptr != 0) { ins = qptr; goto e1; }
 //  else err("Table %s INSERT is failing and Qgoto; was not programmed.\n",ar[aa].tname);
 //}
break;

case 'w': /* update the row */
 if(qptr != 0) { ins = qptr; goto e1; }
  // vChk(SQLSetPos(ar[aa].hstmt[ik],1,SQL_UPDATE,SQL_LOCK_UNLOCK));
  /* sqres = vChk2(SQLExecute(ar[aa].stupd[ik]),ar[aa].stupd[ik]);*/
 //if(ar[aa].premo[ik] == 0) { /* prepare statement if it is not */
 //   vsqupdate(aa);
 //   ar[aa].premo[ik] = 1;
 //}
 //sqres = SQLExecute(ar[aa].stupd[ik]);
 //if(sqres != SQL_SUCCESS && sqres != SQL_SUCCESS_WITH_INFO) {
  //if(qptr == 0) {
  //  err("Table %s f%d k%d UPDATE or INSERT has failed and Qgoto; is not programmed\n",ar[aa].tname,aa,ik);
  //}
 //ins = qptr; goto e1;
 //}
 //if(*(ptr+1) =='c') SQLFetch(ar[aa].hstmt[ik]);
break;

case 'D': /* row to be deleted */
   ins = qptr; goto e1;
// vChk(SQLSetPos(ar[aa].hstmt[ik],1,SQL_DELETE,SQL_LOCK_UNLOCK));
// SQLGetCursorName(ar[aa].hstmt[ik],ar[aa].curname[ik],32,NULL);
//if(ar[aa].modo == 'O') {
// sprintf(sql1,"delete from %s where current of %s",
//               ar[aa].tname,ar[aa].curname[ik]);
// if(ar[aa].prede[ik] == 0) { /* prepare statement if it is not */
//    vsqdelete(aa);
//    ar[aa].prede[ik] = 1;
// }
// vChk(SQLExecute(ar[aa].stdel[ik]));
// }
// else err("This table (%s) was opened in read only mode. Can't delete.\n",
//           ar[aa].tname);
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
   //sqres = SQL_SUCCESS;
   break;
  case from_no:
   sqres_fetch = MYSQL_NO_DATA;
   break;
  case from_a:
#ifdef DEBUG
   fprintf(debug,  "Ejecuto un Fetch Tabla:%d K:%d\n", aa, ik);
   fflush(debug);
#endif
          //sqres = SQLExtendedFetch(ar[aa].hstmt[ik], SQL_FETCH_NEXT, 0, &udw1, ar[aa].fRSt);
       sqres_fetch = mysql_stmt_fetch(ar[aa].hstmt[ik]);
#ifdef DEBUG
       if(sqres_fetch == MYSQL_NO_DATA ) {
         fprintf(debug,  "No tengo datos en Fetch Tabla:%d K:%d\n", aa, ik);
         fflush(debug);
	   } else {
         despliega_resultados(aa, ik);
       }
#endif
   //sqres = SQLFetch(ar[aa].hstmt[ik]);
  break;
  case from_b:
          //sqres = SQLExtendedFetch(ar[aa].hstmt[ik], SQL_FETCH_PRIOR, 0, &udw1, ar[aa].fRSt);
     //sqres = SQLFetchPrev(ar[aa].hstmt[ik]);
       sqres_fetch = mysql_stmt_fetch(ar[aa].hstmt[ik]);
#ifdef DEBUG
       despliega_resultados(aa, ik);
#endif
  break;
  case from_T:
          //sqres = SQLExtendedFetch(ar[aa].hstmt[ik], SQL_FETCH_FIRST, 0, &udw1, ar[aa].fRSt);
       sqres_fetch = mysql_stmt_fetch(ar[aa].hstmt[ik]);
      //sqres = SQLFetch(ar[aa].hstmt[ik]);
     //fFety = SQL_FETCH_FIRST;
   break;
  case from_l:
          //sqres = SQLExtendedFetch(ar[aa].hstmt[ik], SQL_FETCH_FIRST, 0, &udw1, ar[aa].fRSt);
     //sqres = SQLFetch(ar[aa].hstmt[ik]);
     sqres_fetch = mysql_stmt_fetch(ar[aa].hstmt[ik]);

   break;
 } /* end of switch from */
/*x   sqres = SQLExtendedFetch(ar[aa].hstmt[ik],
                            fFety,
                            0,
                            &udw1,
                            ar[aa].fRSt); */
chlock: from = 0;
if(sqres_fetch == MYSQL_NO_DATA ) {
	if(sqres_fetch == 1) {
       fprintf(stderr, " %s\n", mysql_stmt_error(ar[aa].hstmt[ik]));
	}
    ++ptr; ins = *(int*)ptr; from=from_no; goto e1;
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

case 'T': /* previous */
 from = from_T;
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
 //vChk(SQLFreeStmt(ar[aa].hstmt[ik],SQL_CLOSE));
 //vChk(SQLExecDirect(ar[aa].hstmt[ik],sql1,SQL_NTS));
 //if(!ar[aa].bound[ik]) { vSQLBind(aa); ar[aa].bound[ik]= 1; }
 //from = from_a;
 break;
case 'P': /* execute a prepared statement */
 ++ptr; from=0;
 ptr3 = ptr;
 if(ar[aa].pre[ik]==0) {
  ar[aa].pre[ik]=1;
  goto doprepare;
 }
 //vChk(SQLFreeStmt(ar[aa].hstmt[ik],SQL_CLOSE));
fromdoprepare:
#ifdef DEBUG
                      fprintf(debug, "estoy en fromdoprepare Case 'P' y Voy a Realizar stmt_execute Table:%d ik:%d\n", aa, ik);
                      fflush(debug);
#endif
 ptr = ptr3;
 if(*ptr != '$') {
  //vChk(SQLExecute(ar[aa].hstmt[ik]));
   //sqres_fetch = mysql_stmt_execute(ar[aa].hstmt[ik]);
   sqres_fetch = mysql_stmt_execute(ar[aa].hstmt[ik]);
   despliega_parametros(aa, ik);
   #ifdef DEBUG
                      fprintf(debug, "stmt_execute Table:%d ik:%d ya Esta Realizado\n", aa, ik);
                      fprintf(debug, "Parametros: %d\n", ar[aa].numpar_z[ik]);
                      fflush(debug);
   #endif
   vSQLBind(aa);
   if ( mysql_stmt_store_result(ar[aa].hstmt[ik])) {
      fprintf(stderr, " mysql_store_result_result() failed\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(ar[aa].hstmt[ik]));
      exit(0);
   }
   //debug_describe_resultados(aa, ik);
  //if(ar[aa].modo == 'O')
  //  SQLGetCursorName(ar[aa].hstmt[ik],ar[aa].curname[ik],32,NULL);
    //ar[aa].sqres[ik] = mysql_use_result(hdbc[aa]);
  }
 else { /* se trata de una cl�usula UPDATE O INSERT o DELETE */
   //sqres = SQLExecute(ar[aa].hstmt[ik]);
  sqres_fetch = mysql_stmt_execute(ar[aa].hstmt[ik]);
  if(!sqres_fetch) {
     if(qptr == 0) {
      err("P$ f%d k%d with UPDATE INSERT DELETE has failed and Qgoto; is not programmed\n",
        aa,ik,ar[aa].tname);
     }
     ins = qptr; goto e1;
  }
  break;
 }
 #ifdef DEBUG
                      fprintf(debug, "Ya se ejecuto aa:%d ik:%d \n", aa, ik);
                      fflush(debug);
 #endif

 from = from_a;

 break;

case 'k': /* cursor prepare instruction */
   ++ptr;
   if(isdigit(*ptr)) {
    ar[aa].ik = atoi(ptr);
    ik = ar[aa].ik;
    /*SQLGetCursorName(ar[aa].hstmt[ik],ar[aa].curname[ik],32,NULL);*/
    break;
   }
   if(*ptr == 'L') { /* kL; sets LOCK mode reads */
     //  vChk(SQLSetStmtOption(ar[aa].hstmt[ik],SQL_CONCURRENCY,SQL_CONCUR_LOCK));
     //  break;

   }
   ar[aa].pun[ik]=ptr;
   ar[aa].pre[ik]=0;
   goto endkq;

   doprepare:
   ptr = ar[aa].pun[ik];

   #ifdef DEBUG
                      fprintf(debug, "Estoy en doprepare Se va a Ejecutar aa:%d ik:%d %s\n", aa, ik, ptr);
                      fflush(debug);
   #endif

   // --> Aqui llevaba *ptr == 'q' pero para bloquearlo le puse 'U' DRBR 9-Mar-2019
   if(*ptr == 'p' || *ptr == 'q' ) {
     #ifdef DEBUG
                      fprintf(debug, "Estoy kp o kq aa:%d ik:%d \n", aa, ik);
                      fflush(debug);
     #endif
     vSQL_Prepare_STMT(aa);
     while(*ptr++ != ';');
   }

  goto fromdoprepare;
  endkq:
break;

case 'C': // end the connections hstmt[10],stins,stdel[10],stupd[10];
if(*(ptr+1) == 'D') // Disconnect from active database
{
 //vChk(SQLDisconnect(hdbc[dbh]));
 //vChk(SQLFreeConnect(hdbc[dbh]));
 hdbc[dbh]=0;
 break;
}
 for(i=0;i<MAXSTMT;i++) {
  ar[aa].prede[i]=0;
  ar[aa].premo[i]=0;
  ar[aa].prein[i]=0;
#define STXX ar[aa].hstmt[i]
  if(STXX != NULL) {
	  mysql_stmt_close(ar[aa].hstmt[i]);
   //SQLFreeStmt(STXX,SQL_UNBIND);
   //SQLFreeStmt(STXX,SQL_RESET_PARAMS);
   //SQLFreeStmt(STXX,SQL_DROP);
  }
#undef STXX
#define STXX ar[aa].stdel[i]
  if(ar[aa].stdel[i] != NULL) {
	  mysql_stmt_close(ar[aa].stdel[i]);
   //SQLFreeStmt(STXX,SQL_UNBIND);
   //SQLFreeStmt(STXX,SQL_RESET_PARAMS);
   //SQLFreeStmt(STXX,SQL_DROP);
  }
#undef STXX
#define STXX ar[aa].stupd[i]
  if(STXX != NULL) {
	  mysql_stmt_close(STXX);
   //SQLFreeStmt(STXX,SQL_UNBIND);
   //SQLFreeStmt(STXX,SQL_RESET_PARAMS);
   //SQLFreeStmt(STXX,SQL_DROP);
  }
#undef STXX
 }
#define STXX ar[aa].stins
 if(STXX != NULL) {
	  mysql_stmt_close(STXX);
   //SQLFreeStmt(STXX,SQL_UNBIND);
   //SQLFreeStmt(STXX,SQL_RESET_PARAMS);
   //SQLFreeStmt(STXX,SQL_DROP);
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
  case 'u':
      ++ptr; ++ptr; ptr1 = (char*)getad(ptr); MP;
      while (*ptr1) { *ptr1 = toupper(*ptr1); (char*)ptr1++; }
      break;
  case 'r':
      ++ptr; ++ptr;
      desti = (char*)getad(ptr); MP;
      ptr1 = (char*)getad(ptr); MP;
      ptr3 = (char*)getad(ptr); MP;
      while (*desti) {
          if(*desti == *ptr1) *desti = *ptr3; (char *) desti++;
      }
      break;
  case 'x':
   ++ptr; ++ptr;
   ptr1 = (char*)getad(ptr);
   if(!ar[aa].hstmt[ik]) {
     //vChk(SQLAllocStmt(hdbc[dbh],&ar[aa].hstmt[ik]));
   }
   //vChk(SQLFreeStmt(ar[aa].hstmt[ik],SQL_CLOSE));
   //vChk(SQLExecDirect(ar[aa].hstmt[ik],ptr1,SQL_NTS));
   // if(!ar[aa].bound[ik]) { vSQLBind(aa); ar[aa].bound[ik]= 1; }
   from = 0;
   break;
  case 'T': /* qT,SUBRUT,; */
T1:  ++ptr; ++ptr;
	  sur[nivo++] = ins + 1;
	  ins = atoi(ptr);
	  goto e1;
  case 'C': /* commit trans */
/*   vChk(SQLTransact(hEnv,SQL_NULL_HDBC,SQL_COMMIT));*/
   //sqres = SQLTransact(SQL_NULL_HENV,hdbc[dbh],SQL_COMMIT);
   //if(sqres == SQL_SUCCESS || sqres == SQL_SUCCESS_WITH_INFO)
   //  goto T1;
   if(!qptr)
     err("A commit operation has failed and Q..; is invalid\n");
   ins = qptr; goto e1;
  case 'R': /* rollback trans */
   //vChk(SQLTransact(SQL_NULL_HENV,hdbc[dbh],SQL_ROLLBACK));
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
