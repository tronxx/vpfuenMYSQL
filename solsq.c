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

#include "archisq.h"
#include "stdlib.h"
#include "string.h"

extern FILE *debug;

extern MYSQL hdbc[10];
  //extern HENV hEnv;
extern ARCHI *ar;
extern int aa;
extern int ik;
extern int ins;
extern int actsize;
extern char *ptr1;
extern char *ptr;


void vSQLBind(int miaa) /* get desc. and bind columns */
{
  char cname[60];
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

      case MYSQL_TYPE_STRING: thisz = field_z->length + 1; break;
      default: thisz = field_z->length; break;
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
  prepare_meta_result = mysql_stmt_result_metadata(ar[aa].hstmt[ik]);  
  numcols_z = mysql_num_fields(prepare_meta_result);  
  memset(ar[aa].bind_col, 0, 60 * sizeof(MYSQL_BIND));  
  bptr = (char *) calloc(1, bufsz);
  if( bptr == NULL) {
  		    fprintf(stderr, "Can't get memory for table # %d, key %d, (%s) Column: ",aa,ik,ar[aa].tname, field_z->name);
  }
  ar[aa].a_buf = bptr;
  ar[aa].a_dat = bptr;
  //-.- Debug
  memset(msg_z, 0, sizeof(msg_z));
  sprintf(msg_z,  "Voy a Preparar las columnas para el bind");
  message_debug(msg_z);
  icol_z = 0;
  while(field_z = mysql_fetch_field(prepare_meta_result)) {
	 ar[aa].bind_col[icol_z].buffer_type = field_z->type;
	 ar[aa].bind_col[icol_z].buffer_length = &ar[aa].col_tam[icol_z];
 	 switch(field_z->type) {
			case MYSQL_TYPE_VAR_STRING: 
			  thisz = field_z->length + 1; 
			break;

			case MYSQL_TYPE_DOUBLE:
			  thisz = sizeof(double); 
			  break;
			case MYSQL_TYPE_FLOAT:
			  thisz = sizeof(float); 
	          break;
			case MYSQL_TYPE_DECIMAL:
			case MYSQL_TYPE_NEWDECIMAL: 
			  thisz = sizeof(double); 
	          break;
			case MYSQL_TYPE_LONG: 
			  thisz = sizeof(int); 
	          break;
			case MYSQL_TYPE_SHORT: 
			  thisz = sizeof(short int); 
			break;

			case MYSQL_TYPE_DATE: thisz = sizeof(MYSQL_TIME); 
			case MYSQL_TYPE_TIME: thisz = sizeof(MYSQL_TIME); 
			case MYSQL_TYPE_DATETIME: thisz = sizeof(MYSQL_TIME);
			break;

			case MYSQL_TYPE_STRING: 
			   //ar[aa].bind[icol_z].buffer= bptr;
			  thisz = field_z->length + 1;
			break;
			default: thisz = cprecis + 1; 
            break;
     }
	 ar[aa].bind_col[icol_z].is_null= &ar[aa].col_is_null[icol_z];
	 ar[aa].bind_col[icol_z].length= &ar[aa].col_tam[icol_z];
	 ar[aa].bind_col[icol_z].error = &ar[aa].col_error[icol_z];
	 ar[aa].bind_col[icol_z].buffer = bptr;
	 memset(msg_z, 0, sizeof(msg_z));
	 sprintf(msg_z,   "Field %d %s Pos: %d Size: %d ", icol_z, field_z->name, bptr, thisz);
	 message_debug(msg_z);
	 icol_z++;
	 bptr = bptr + thisz;
	  
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
  int icol;
  char cname[60];
  int cnamersz,dType,ctype,cscale,ifnul;
  int cprecis,bufsz=0,thisz;
  char *bptr;
  int bind;
  /* Free the prepared result metadata */
  mysql_free_result(ar[aa].sqres[ik]);
  mysql_stmt_close( ar[aa].hstmt[ik] );
}

void message_debug(char *msg_z) {
#ifdef DEBUG
	  fprintf(debug, "%s\n", msg_z);
      fflush(debug);
#endif

}	

void debug_describe_resultados(int miaa, int ik)
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
			   sprintf(valpar_z, "%d Tipo: DOUBLE:  %f", iPar, (double *)ar[aa].bind_param[ik][iPar].buffer);
			
			break;
			case MYSQL_TYPE_LONG: 
			   sprintf(valpar_z, "%d Tipo: LONG:  %d", iPar, (int *)ar[aa].bind_param[ik][iPar].buffer);
			break;

			case MYSQL_TYPE_SHORT: 
			   sprintf(valpar_z, "%d Tipo: SHORT: %d", iPar, (short int *)ar[aa].bind_param[ik][iPar].buffer);
			break;

			case MYSQL_TYPE_DATE: 
			case MYSQL_TYPE_TIME: 
			case MYSQL_TYPE_DATETIME: 
			  sprintf(valpar_z, "%d Tipo: DATE: %s", iPar, (double *)ar[aa].bind_param[ik][iPar].buffer);
			break;
		}
		strcat(msg1_z, valpar_z);
	}
	//-.- Debug
	message_debug(msg1_z);
}

void despliega_resultados(int miaa, int ik)
{
	char msg1_z[1024];
	char valpar_z[1024];
	int iPar=0;
    char *bptr;
    //-.- Debug
	memset(msg1_z, 0, sizeof(msg1_z));
	sprintf(msg1_z, "Estoy en despliega resultados. Se realizo fetch, n.Cols:%d", ar[aa].numcols_z);
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
			  sprintf(valpar_z, "%d Pos: %d Tipo: DATE: %s", iPar, bptr,  (double *)bptr);
			break;
		}
		message_debug(valpar_z);
	}
    //-.- Debug
}

void despliega_parametros(int miaa, int ik)
{
	char msg1_z[1024];
	char valpar_z[1024];
	int iPar=0;
    char *bptr;
    //-.- Debug
	memset(msg1_z, 0, sizeof(msg1_z));
	sprintf(msg1_z, "Estoy en despliega Parametros. Se realizo un Execute n.Params:%d", ar[aa].numpar_z[ik]);
	message_debug(msg1_z);

 	memset(msg1_z, 0, sizeof(msg1_z));
	memset(valpar_z, 0, sizeof(valpar_z));
	for(iPar = 0; iPar < ar[aa].numpar_z[ik]; iPar++) {
		bptr = ar[aa].bind_param[ik][iPar].buffer;
		switch ( ar[aa].bind_param[ik][iPar].buffer_type  ) {
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
			  sprintf(valpar_z, "%d Pos: %d Tipo: DATE: %s", iPar, bptr,  (double *)bptr);
			break;
		}
		message_debug(valpar_z);
	}
    //-.- Debug
}

/*
  DRBR 2 de Julio de 2019
  Se modifica para que el prepare sea var= :NOMBRE_PARAMETRO
  y según el tipo de parámetro será si se agrega o no un apóstrofe 
*/
void vSQL_Prepare_STMT(int miaa) /* get desc. and bind columns */
{
  char cname[60];
  int param_count, cnamersz,dType,ctype,cscale,ifnul;
  int cprecis=0,bufsz=0,thisz=0, fSqtype, paramsize_z;
  MYSQL_RES     *prepare_meta_result;
  MYSQL_FIELD   *field_z;
  char *bptr;
  char *ptr, *ptr1, *ptr2, *ptr3;
  char msg_z[255];
  char msg1_z[255];
  char valpar_z[1024];
  char sql1[2048];
  int sqres_fetch;
  my_bool is_null;
  int bind, numcols_z, icol_z, ii_z=0;
  ptr =   ar[aa].pun[ik];

    //-.- Debug
	memset(msg1_z, 0, sizeof(msg1_z));
	sprintf(msg1_z, "-01- Estoy en Prepare Statment :%d  ptr:%s", aa, ptr);
	message_debug(msg1_z);

  memset(sql1, 0, sizeof(sql1));
  ptr2 = ptr;
  if(*ptr2 == 'p' || *ptr2 == 'q' ) {
	  if(*ptr2 == 'p' ) {
        sprintf(sql1,"select * from %s where ", ar[aa].tname);
      }
      while(*ptr2 != ',') ptr2++;
      ptr2++; // Avanzo el Apuntador
	  ptr1 = (char*)getad(ptr2);
      strcat(sql1,ptr1);
  }
  if(*ptr2 == 'x' ) {
	  // Acanzo el punero hasta la coma 
	  // y tomo el contenido de la variable String
	  // La Funcion es qx,varsql_z,var1_z,...,varn_z,;
      while(*ptr2 != ',') ptr2++;
      ptr2++;
	  ptr1 = (char*)getad(ptr2);
      strcat(sql1,ptr1);
  }
  // Avanzo hasta encontrar las comillas, o la coma, k1; kp,"cia=:CIA",cia_z,;
  while(*ptr2 != ',') ++ptr2;

#ifdef DEBUG
                      fprintf(debug, "Preparando para stmt_prepare: %s\n", sql1);
                      fflush(debug);
#endif
  ar[aa].hstmt[ik] = mysql_stmt_init(ar[aa].hdbc);  
  if (mysql_stmt_prepare(ar[aa].hstmt[ik],sql1,strlen(sql1))) {
     fprintf(stderr, " mysql_stmt_prepare(), SELECT failed\n");
     fprintf(stderr, " %s\n", mysql_stmt_error(ar[aa].hstmt[ik]));
    exit(0);
  }
  
#ifdef DEBUG
                      fprintf(debug, "Ejecutado stmt_prepare: %s\n", sql1);
                      fflush(debug);
#endif
   //memset(ar[aa].bind_param[ik], 0, sizeof(MYSQL_BIND)*60);
   param_count= mysql_stmt_param_count(ar[aa].hstmt[ik]);
   ar[aa].numpar_z[ik] = param_count;
   
#ifdef DEBUG
                      fprintf(debug, "Num de Parametros: %d\n", param_count);
                      fflush(debug);
#endif
   ii_z = 0;
   // --> Aqui estoy en una coma, avanzo un lugar para leer mi variable
   ptr2++;
   memset(valpar_z, 0, sizeof(valpar_z));
   do {
	     paramsize_z = 0;
       ptr1 = (char*)getad(ptr2);
       switch(*ptr2) {
         case 's':
		   sprintf(valpar_z, "N.Param: %d Pos: %d Tipo: String: %s", ii_z, ptr2, ptr2);
      	   while(*ptr2 != ',') ptr2++;
           ptr2++;
           paramsize_z = atoi(ptr);
           //i = SQL_NTS;
           //  --> Avanzo hasta la siguiente coma y un lugar más
      	   while(*ptr2 != ',') ptr2++;
           ptr2++;

           switch(*ptr2) {
              case '1': fSqtype = MYSQL_TYPE_STRING; break;
              case '2': fSqtype = MYSQL_TYPE_TIMESTAMP; break;
              case '3': fSqtype = MYSQL_TYPE_DATE;  break;
              case '4': fSqtype = MYSQL_TYPE_TIME;  break;
              default: err("Bad data type - instr # %d %s\n",ins, ptr2); break;
           }
           // ar[aa].bind_param[ik][iPar].buffer         = (char *) ptr1; --> Aqui tengo la direccion
           break;
        case 'd': 
   	      sprintf(valpar_z, "%d Pos: %d Tipo: Double: %f", ii_z, ptr2, *(double *)ptr1);
          fSqtype = MYSQL_TYPE_DOUBLE; 
        break;
        case 'f': 
   	      sprintf(valpar_z, "%d Pos: %d Tipo: Float: %f", ii_z, ptr2, *(float *)ptr1);
          fSqtype  = MYSQL_TYPE_FLOAT; 
        break;
        case 'D':
        case 'l': 
   	      sprintf(valpar_z, "%d Pos: %d Tipo: Long: %d", ii_z, ptr2, *(int *)ptr1);
          fSqtype  = MYSQL_TYPE_LONG;  
        break;
        case 'i': 
   	      sprintf(valpar_z, "%d Pos: %d Tipo: Int: %d", ii_z, ptr2, *(int *)ptr1);
          fSqtype  = MYSQL_TYPE_SHORT; 
        break;
        case ';': break;
    
        default: err("Bad data type %c",*ptr2); break;
     }
	 message_debug(valpar_z);
     ar[aa].bind_param[ik][ii_z].buffer_type= fSqtype;
	 ar[aa].bind_param[ik][ii_z].buffer         = (char *)ptr1;
	 ar[aa].bind_param[ik][ii_z].is_null= &is_null;
	 ar[aa].bind_param[ik][ii_z].length = paramsize_z;
#ifdef DEBUG
                      fprintf(debug, "--Parametro: %d Direccion:%d\n", ii_z, ptr1);
                      fflush(debug);
#endif
     while(*ptr2++ != ',');
     ptr3 = ptr2 + 1; // Avanzo el apuntador y checo si es un punto y coma 
     if(*ptr3 == ';') break;
     ii_z++;
   } while(*ptr2++ != ';');
   if (mysql_stmt_bind_param(ar[aa].hstmt[ik], ar[aa].bind_param[ik])) {
		fprintf(stderr, " mysql_stmt_bind_param() failed\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(ar[aa].hstmt[ik]));
		exit(0);
   }
   //vSQLBind(aa);
   ar[aa].pre[ik]=1;
   // --> Voy a correr el apuntador
}
