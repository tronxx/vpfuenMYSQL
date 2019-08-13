case 'q': ++ptr;
 from=0;
 switch(*ptr) {
  case 'N': // new SQL statement
   ++ptr; ++ptr; ae = atoi(ptr);
//   if(ar[ae].fldesc != NULL) free(ar[ae].fldesc);
   if(ar[ae].hstmt[ar[ae].ik] != NULL)
    vChk(SQLFreeStmt(ar[ae].hstmt[ar[ae].ik],SQL_CLOSE));
   else
    vChk(SQLAllocStmt(ar[ae].hdbc,&ar[ae].hstmt[ar[ae].ik]));
/*   memcpy(&ar[ae],&ar[aa],sizeof(ARCHI));
   ar[ae].fldesc = NULL;
   ar[ae].a_fd = NULL;*/
   memset(sql,0,sizeof(sql));
  break;
  case 'S': // add to sql string
   ++ptr; ++ptr;
   while(*ptr != ';') {
    strcat(sql,vRetString(ptr));
    if(*ptr == '"') { while(*ptr++ != 0); ++ptr; }
    else while(*ptr++ != ',');
   }
  break;
  case 'X': // execute the statment
   vChk(SQLExecDirect(ar[aa].hstmt[ik],sql,SQL_NTS));
   vSQLBind(aa); ar[aa].bound[ik]= 0;
/*   if(Chk(DbiQExecDirect(ar[aa].hDb, qrylangSQL, sql, &ar[aa].a_fd)) != DBIERR_NONE) {
      Chk(DbiExit());
   	err("Sorry: you have errors in your SQL statement. Program must end.");
      }
X1:   if(Chk(DbiGetCursorProps(ar[aa].a_fd,&ar[aa].CuP)) != DBIERR_NONE) break;
   if(ar[aa].fldesc != NULL) free(ar[aa].fldesc);
    ar[aa].fldesc = (FLDDesc *)calloc(ar[aa].CuP.iFields,sizeof(FLDDesc));
   Chk(DbiGetFieldDescs(ar[aa].a_fd,ar[aa].fldesc));*/
  break;

  case 'T': // get the total and store in variable
   DbiGetRecordCount(ar[aa].a_fd,(UINT32*)getad(ptr+2));
  break;
{ case 'k': // qk,ff,eval,;
  int i; long max;
   ++ptr; ++ptr;
   ae = atoi(ptr); MP;
   max = *(long*)getad(ptr);
   DbiSetToBegin(ar[ae].a_fd);
   for(i=0;i<=max;i++) {
     DbiGetNextRecord(ar[ae].a_fd,dbiNOLOCK,NULL,NULL);
   }
   DbiGetRecord(ar[ae].a_fd,dbiNOLOCK,ar[ae].a_buf,NULL);
   resf = DbiSetToKey(ar[aa].a_fd,keySEARCHEQ,FALSE,0,0,ar[ae].a_buf);
   from= from_P;
  break; }
  case 'i': // get initialize the data in buffer
   //DbiInitRecord(ar[aa].a_fd,ar[aa].a_buf);
  break;
  case '+': // concatenate strings
   ++ptr; ++ptr; ptr1 = (char*)getad(ptr); MP;
   while(*ptr != ';') {
    strcat(ptr1,vRetString(ptr));
    if(*ptr == '"') { while(*ptr++ != 0); ++ptr; }
    else while(*ptr++ != ',');
   }
  break;

  case 'p': // prepare a statment to execute qp,0-20,"select xxxxx " or sBuf,;
   ++ptr; ++ptr; i = atoi(ptr); MP;
   ptr1 = (char*)getad(ptr);
   if(hSt[i] != NULL) Chk(DbiQFree(&hSt[i]));
   if(ar[aa].a_fd != NULL) Chk(DbiCloseCursor(&ar[aa].a_fd));
   if(Chk(DbiQAlloc(ar[aa].hDb,qrylangSQL,&hSt[i])) != DBIERR_NONE) err("");
   if(Chk(DbiQPrepare(hSt[i],ptr1)) != DBIERR_NONE) err("");
  break;

  case 'x': // execute a prepared statement qx,0-20,#p,[123nn],buf,;
   ++ptr; ++ptr; i=atoi(ptr); MP;
 { int j,np;
   np = atoi(ptr); MP;
   if(np == 0) break;
   memset(pfd,0,sizeof(FLDDesc)*8);
   for(j=0; j < np; j++) {
    pfd[j].iFldNum = j + 1;
    switch(*ptr) {
     case '1': pfd[j].iFldType = fldFLOAT; pfd[j].iLen = sizeof(double);
       pfd[j].iUnits1 = 1; break;
     case '2':
       pfd[j].iFldType = fldINT32; pfd[j].iLen = sizeof(fldINT32);
       pfd[j].iUnits1 = 1; break;
     case '3':
       pfd[j].iFldType = fldZSTRING; pfd[j].iLen = atoi(ptr+1);
       pfd[j].iUnits1 = atoi(ptr+1);
       break;
     default: err("Only d 1, l 2, and s 3 are acceptable in qp...; instr."); break;
    }
    MP;
   }
   ptr1 = (char*)getad(ptr);
   if(Chk(DbiQSetParams(hSt[i], np, pfd , (pBYTE) ptr1)) != DBIERR_NONE) err("");
  }
   Chk(DbiQExec(hSt[i],&ar[aa].a_fd));
   goto X1;

  case 'C': // Close the active cursor
   Chk(DbiCloseCursor(&ar[aa].a_fd));
  break;

  case 'R': // REfresh data in cursor
   Chk(DbiForceReread(ar[aa].a_fd));
  break;

 }
break; // endf case 'q'

