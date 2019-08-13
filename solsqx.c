/*
   This is part of the VPG4S version for LINUX.
   This section takes care of the binding of fields and parameters
  by Franz J Fortuny April 29 199
  Modificado 28-Feb-2019 Daniel R Basto Rivero
  Se deja en blanco la funcion VCK 
*/
#include <stdio.h>
#include <my_global.h>
#include <mysql.h>

//#include "cli0core.h"
#include "cli0defs.h"
//#include "cli0env.h"
//#include "cli0ext1.h"
#include "archisq.h"
#include "stdlib.h"
#include "string.h"

extern FILE *debug;

extern MYSQL hdbc[10];
  //extern HENV hEnv;
extern ARCHI *ar;
extern int ik;
extern SDWORD actsize;
void err(char *, ...);
void vbindpar(int aa,HSTMT thest);

RETCODE vChk(RETCODE sqres)

{
  char buf1[256],buf2[256],buf3[256];
  SDWORD sdw1;
  SWORD sw1;
  extern int aa;
  extern int ik;

  return sqres;

}


void vsqdelete(int aa)
{
  char *sql;
  UWORD icol;
  UCHAR cname[60];
  SWORD cnamersz,dType,ctype,cscale,ifnul;
  UDWORD cprecis,bufsz=0,thisz;
  char *bptr;
  int bind,i,longvar=0,rcol=0;

 sql = (char*) calloc(200,sizeof(char));

 sprintf(sql,"delete from %s where current of %s",ar[aa].tname,
             ar[aa].curname[ik]);
   free(sql);
}

void vsqupdate(int aa)
{
 /* this function prepares de insert statement */
  char *sql;
  UWORD icol;
  UCHAR cname[60];
  SWORD cnamersz,dType,ctype,cscale,ifnul;
  UDWORD cprecis,bufsz=0,thisz;
  char *bptr;
  int bind,i,longvar=0,rcol=0;

  sql = (char*) calloc(2000,sizeof(char));

  sprintf(sql,"update %s set ",ar[aa].tname);
  icol = 1;
  free(sql);
}

void vsqinsert(int aa)
{
 /* this function prepares de insert statement */
  char *sql;
  UWORD icol;
  UCHAR cname[60];
  SWORD cnamersz,dType,ctype,cscale,ifnul;
  UDWORD cprecis,bufsz=0,thisz;
  char *bptr;
  int bind,i,longvar=0,rcol=0;

 sql = (char*) calloc(2000,sizeof(char));

  sprintf(sql,"insert into %s (",ar[aa].tname);
  icol = 1;
  strcat(sql,") values ("); /* insert statement ready */
   for(i=0;i<rcol;i++) {
    if(i>0) strcat(sql,",");
    strcat(sql," ?");
   }
   strcat(sql,")");
   free(sql);
}

void vSQLBind(int aa) /* get desc. and bind columns */
{
  UCHAR cname[60];
  int cnamersz,dType,ctype,cscale,ifnul;
  int cprecis=0,bufsz=0,thisz=0;
  MYSQL_RES     *prepare_meta_result;
  MYSQL_FIELD   *field_z;
  char *bptr;
  char msg_z[255];
  int bind, numcols_z, icol_z, ii_z=0;

  icol_z = 0;
  prepare_meta_result = mysql_stmt_result_metadata(ar[aa].hstmt[ik]);
  numcols_z = mysql_num_fields(prepare_meta_result);
  memset(ar[aa].bind_col, 0, sizeof(ar[aa].bind_col));
  while(field_z = mysql_fetch_field(prepare_meta_result)) {
	 switch(field_z->type) {
		 
       case SQL_BIT: thisz = 1; break;
       case MYSQL_TYPE_VAR_STRING: thisz = field_z->length + 1; break;

       case MYSQL_TYPE_DOUBLE:
       case MYSQL_TYPE_FLOAT:
       case MYSQL_TYPE_DECIMAL:
       case MYSQL_TYPE_NEWDECIMAL: thisz = sizeof(double); break;
       case MYSQL_TYPE_LONG: thisz = sizeof(int); break;

       case MYSQL_TYPE_SHORT: thisz = sizeof(short int); break;

       case MYSQL_TYPE_DATE: thisz = sizeof(MYSQL_TIME); break;
       case MYSQL_TYPE_TIME: thisz = sizeof(MYSQL_TIME); break;
       case MYSQL_TYPE_DATETIME: thisz = sizeof(MYSQL_TIME); break;

	  case SQL_BIGINT:
	  case SQL_BINARY:
      case SQL_VARBINARY:
      case SQL_CHAR:
      case MYSQL_TYPE_STRING: thisz = field_z->length + 1; break;
      default: thisz = field_z->length + 1; break;
    }
	//-.- Debug
  	 memset(msg_z, 0, sizeof(msg_z));
	 sprintf(msg_z, "Field %d %s", icol_z, field_z->name);
	 message_debug(msg_z);
     bufsz = bufsz + thisz;
     icol_z++;
   }
   //-.- Debug
   memset(msg_z, 0, sizeof(msg_z));
   sprintf(msg_z,  "Tam.Buffer: %d\n", bufsz);
   message_debug(msg_z);

   bptr = (char *) calloc(1, bufsz);
   if( bptr == NULL) {
        err("Can't get memory for table # %d, key %d, (%s) ", aa, ik, ar[aa].tname);
   }
   ar[aa].a_sz  = bufsz;
   ar[aa].a_buf = bptr;

   //-.- Debug
   sprintf(msg_z, "Posicion de Buffer: %d Tam: %d ", bptr, bufsz);
   message_debug(msg_z);

   prepare_meta_result = mysql_stmt_result_metadata(ar[aa].hstmt[ik]);  
   numcols_z = mysql_num_fields(prepare_meta_result);  
	//-.- Debug
	memset(msg_z, 0, sizeof(msg_z));
	sprintf(msg_z,  "Voy a Preparar las columnas para el bind");
	message_debug(msg_z);

   icol_z = 0;
   while(field_z = mysql_fetch_field(prepare_meta_result)) {
		ar[aa].bind_col[icol_z].buffer_type = field_z->type;
		thisz = field_z->length;
		//-.- Debug

		switch(field_z->type) {
			case SQL_BIT: thisz = 1; break;
			case MYSQL_TYPE_VAR_STRING: 
			  thisz = field_z->length + 1;
			break;

			case MYSQL_TYPE_DOUBLE:
			  thisz = sizeof(double); break;
			case MYSQL_TYPE_FLOAT:
			  thisz = sizeof(float); break;
			case MYSQL_TYPE_DECIMAL:
			case MYSQL_TYPE_NEWDECIMAL: 
			  thisz = sizeof(double); break;
			case MYSQL_TYPE_LONG: 
			  thisz = sizeof(int); break;

			case MYSQL_TYPE_SHORT: 
			  thisz = sizeof(short int); break;
			break;

			case MYSQL_TYPE_DATE: thisz = sizeof(MYSQL_TIME); break;
			case MYSQL_TYPE_TIME: thisz = sizeof(MYSQL_TIME); break;
			case MYSQL_TYPE_DATETIME: thisz = sizeof(MYSQL_TIME); break;

			case SQL_BIGINT:
			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_CHAR:
			case MYSQL_TYPE_STRING: 
			   //ar[aa].bind[icol_z].buffer= bptr;
			  thisz = field_z->length + 1;
			break;
			default: thisz = cprecis + 1; break;
		}
		ar[aa].bind_col[icol_z].is_null= &ar[aa].col_is_null[icol_z];
		ar[aa].bind_col[icol_z].length= &ar[aa].col_tam[icol_z];
		ar[aa].bind_col[icol_z].error = &ar[aa].col_error[icol_z];
	    ar[aa].bind_col[icol_z].buffer = bptr;
	    memset(msg_z, 0, sizeof(msg_z));
	    sprintf(msg_z,   "Field %d %s Pos: %d Size: %d ", icol_z, field_z->name, bptr, thisz);
	    message_debug(msg_z);

		bptr = bptr + thisz;
		icol_z++;
   }
   ar[aa].numcols_z = numcols_z;

  
   if (mysql_stmt_bind_result(ar[aa].hstmt[ik], ar[aa].bind_col)) {
      fprintf(stderr, " mysql_stmt_bind_result() failed\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(ar[aa].hstmt[ik]));
      exit(0);
  }
  memset(msg_z, 0, sizeof(msg_z));
  sprintf(msg_z, "Se realizó el Bind");
  message_debug(msg_z);

}

void vendsql() /* get desc. and bind columns */
{
  UWORD icol;
  UCHAR cname[60];
  SWORD cnamersz,dType,ctype,cscale,ifnul;
  UDWORD cprecis,bufsz=0,thisz;
  char *bptr;
  int bind;

  icol = 1;
}

void message_debug(char *msg_z) {
#ifdef DEBUG
	  fprintf(debug, "%s\n", msg_z);
      fflush(debug);
#endif

}	

void debug_describe_resultados(int aa, int ik)
{
	char msg1_z[256];
	char valpar_z[256];
	int iPar=0;

		//-.- Debug
	memset(msg1_z, 0, sizeof(msg1_z));
	sprintf(msg1_z,  "Se realizo stmt_execute, n.Cols:%d Parametros:%d\n", ar[aa].numcols_z, ar[aa].numpar_z[ik]);
	message_debug(msg1_z);

	memset(msg1_z, 0, sizeof(msg1_z));
	memset(valpar_z, 0, sizeof(valpar_z));
	sprintf(msg1_z, "Parametros :\n");
	for(iPar = 0; iPar < ar[aa].numpar_z[ik]; iPar++) {
		sprintf(valpar_z, " %d = ", iPar);
		switch ( ar[aa].bind_param[ik][iPar].buffer_type  ) {
			case MYSQL_TYPE_VAR_STRING: 
			   sprintf(valpar_z, "%d Tipo: VAR_STRING: %s", iPar, (char *)ar[aa].bind_param[ik][iPar].buffer);
			break;

			case MYSQL_TYPE_DOUBLE:
			case MYSQL_TYPE_FLOAT:
			case MYSQL_TYPE_DECIMAL:
			case MYSQL_TYPE_NEWDECIMAL: 
			   sprintf(valpar_z, "%d Tipo: DOUBLE:  %f", iPar, *(double *)ar[aa].bind_param[ik][iPar].buffer);
			
			break;
			case MYSQL_TYPE_LONG: 
			   sprintf(valpar_z, "%d Tipo: LONG:  %d", iPar, *(int *)ar[aa].bind_param[ik][iPar].buffer);
			break;

			case MYSQL_TYPE_SHORT: 
			   sprintf(valpar_z, "%d Tipo: SHORT: %d", iPar, *(short int *)ar[aa].bind_param[ik][iPar].buffer);
			break;

			case MYSQL_TYPE_DATE: 
			case MYSQL_TYPE_TIME: 
			case MYSQL_TYPE_DATETIME: 
			  sprintf(valpar_z, "%d Tipo: DATE: %s", iPar, *(double *)ar[aa].bind_param[ik][iPar].buffer);
			break;
		}
		strcat(msg1_z, valpar_z);
	}
	//-.- Debug
	message_debug(msg1_z);
}

void despliega_resultados(int aa, int ik)
{
	char msg1_z[1024];
	char valpar_z[1024];
	int iPar=0;
    char *bptr;
    //-.- Debug
	memset(msg1_z, 0, sizeof(msg1_z));
	sprintf(msg1_z, "Estoy en despliega resultados. Se realizo fetch %d, n.Cols:%d", ik, ar[aa].numcols_z);
	message_debug(msg1_z);

 	memset(msg1_z, 0, sizeof(msg1_z));
	memset(valpar_z, 0, sizeof(valpar_z));
	for(iPar = 0; iPar < ar[aa].numcols_z; iPar++) {
		bptr = ar[aa].bind_col[iPar].buffer;
		switch ( ar[aa].bind_col[iPar].buffer_type  ) {
			case MYSQL_TYPE_VAR_STRING: 
			   sprintf(valpar_z, "%d Pos: %d Tipo: VAR_STRING: %s", iPar, bptr, (char *) bptr);
			break;

			case MYSQL_TYPE_DOUBLE:
			case MYSQL_TYPE_FLOAT:
			case MYSQL_TYPE_DECIMAL:
			case MYSQL_TYPE_NEWDECIMAL: 
			   sprintf(valpar_z, "%d Pos: %d Tipo: DOUBLE: %f", iPar, bptr,  *(double* )bptr);
			
			break;
			case MYSQL_TYPE_LONG: 
			   sprintf(valpar_z, "%d Pos: %d Tipo: LONG: %d", iPar, bptr, *(int *)bptr);
			break;

			case MYSQL_TYPE_SHORT: 
			   sprintf(valpar_z, "%d Pos: %d Tipo: SHORT: %d", iPar, bptr, *(short int *)bptr);
			break;

			case MYSQL_TYPE_DATE: 
			case MYSQL_TYPE_TIME: 
			case MYSQL_TYPE_DATETIME: 
			  sprintf(valpar_z, "%d Pos: %d Tipo: DATE: %s", iPar, bptr,  *(double *)bptr);
			break;
		}
		message_debug(valpar_z);
	}
    //-.- Debug
}
