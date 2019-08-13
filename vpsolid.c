/* vpsolid.c -- file to be included when using connections to the
   linux / solid SQL Server
   by Franz J Fortuny -- April 1997
*/

case 'O':
case 'o': /* open a connection or a table */
 ++ptr; 

 if(*ptr == '@') { /* connection being established for data base */
  ++ptr;

  if(!sqinit) {
    sqinit = 1;
    vChk(SQLAllocEnv(hEnv));
  }
  vChk(SQLAllocConnect(hEnv,hdbc[dbh]));

  /* O@tcp/ip linux 1313~,sa~,lux~; */

  ptr1 = ptr; MP;
  ptr2 = ptr; MP;
  vChk(SQLConnect(hdbc[dbh],
             ptr1, /* source name */
             strlen(ptr1),
             ptr2, /* user's name */
             strlen(ptr2),
             ptr, /* authentif. word */
             strlen(ptr)));

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
  sprintf(sql1,"select * from %s",ptr);
  vChk(SQLAllocStmt(hdbc[dbh],&ar[aa].hstmt));
  vChk(SQLExecDirect(ar[aa].hstmt,sql1,strlen(sql1)));
 } /* to actually open a table */

break;

case 'f': /* f1; fd0-9; */
 ++ptr;
 if(*ptr == 'd') dbh = atoi(ptr+1);
 else aa = atoi(ptr+1);
break;

