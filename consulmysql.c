#define STRING_SIZE 50

#include <my_global.h>
#include <mysql.h>
#include <string.h>
#include <stdio.h>
#include "archisq.h"
#include "stdlib.h"
#include "string.h"
#include "cli0defs.h"

MYSQL hdbc[10];
  //extern HENV hEnv;
ARCHI *ar;
int ik;
SDWORD actsize;


void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);        
}

int main(int argc, char **argv)
{
  MYSQL *con = mysql_init(NULL);
  MYSQL_STMT    *stmt;
  MYSQL_BIND    bind[30];
  my_ulonglong  affected_rows;
  int           param_count;
  int           size_z;
  short         small_data;
  int           int_data;
  char          str_data[STRING_SIZE];
  unsigned long str_length;
  unsigned long str_length_nombre;
  my_bool       is_null;
  char			misql_z[250];
  char			clave_z[5];
  char			nombre_z[30];
  int			cia_z;
  int ii_z;
  
  hdbc[0] = mysql_init(NULL);
  
  printf("MySQL client version: %s\n", mysql_get_client_info());

  if (hdbc[0] == NULL) {
      fprintf(stderr, "%s\n", mysql_error(hdbc[0]));
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "root", "", 
          "inven", 0, NULL, 0) == NULL)   {
      fprintf(stderr, "%s\n", mysql_error(hdbc[0]));
      mysql_close(hdbc[0]);
      exit(1);
  }  
  
  // Voy a Crear un QRY Prepare para ver que funcione como yo quiero.
  sprintf(misql_z, "insert into almacen (clave, nombre, cia) values (?,?,?)");
  stmt = mysql_stmt_init(hdbc[0]);
  if (!stmt) {
     fprintf(stderr, " mysql_stmt_init(), out of memory\n");
     exit(0);
  }
  
  if (mysql_stmt_prepare(stmt, misql_z, strlen(misql_z)))
  {
    fprintf(stderr, " mysql_stmt_prepare(), INSERT failed\n");
    fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
    exit(0);
  }
  fprintf(stdout, " prepare, INSERT successful\n");
  
  param_count= mysql_stmt_param_count(stmt);
  fprintf(stdout, " total parameters in INSERT: %d\n", param_count);
  
  if (param_count != 3) /* validate parameter count */  {
	    fprintf(stderr, " invalid parameter count returned by MySQL\n");
	    exit(0);
  }
  
  //size_z = sizeof(MYSQL_BIND)*3;
  size_z = sizeof(bind);
  //bind = (MYSQL_BIND*) malloc(size_z);
  memset(bind, 0, size_z);
  /* STRING PARAM */
  bind[0].buffer_type= MYSQL_TYPE_STRING;
  bind[0].buffer= (char *)clave_z;
  bind[0].buffer_length= 5;
  bind[0].is_null= 0;
  bind[0].length= &str_length;

  bind[1].buffer_type= MYSQL_TYPE_STRING;
  bind[1].buffer= (char *)nombre_z;
  bind[1].buffer_length= 30;
  bind[1].is_null= 0;
  bind[1].length= &str_length_nombre;

  bind[2].buffer_type= MYSQL_TYPE_LONG;
  bind[2].buffer= (char *)&cia_z;
  bind[2].length= 0;

  if (mysql_stmt_bind_param(stmt, bind)) {
     fprintf(stderr, " mysql_stmt_bind_param() failed\n");
     fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
     exit(0);
  }

  // Ahora voy a Insertar dos almacenes
  cia_z = 1;             /* integer */
  memset(clave_z, 0, 4);
  sprintf (clave_z, "TI"); /* string  */
  str_length= strlen(clave_z);

  memset(nombre_z, 30, 4);
  sprintf (nombre_z, "TICUL"); /* string  */
  str_length_nombre= strlen(nombre_z);
  is_null= 1;

  /* Execute the INSERT statement - 1*/
  if (mysql_stmt_execute(stmt)) {
     fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
     fprintf(stderr, " %s\n", mysql_stmt_error(stmt)); 
     exit(0);
  }

  memset(clave_z, 0, 4);
  sprintf (clave_z, "PR"); /* string  */
  str_length= strlen(clave_z);

  memset(nombre_z, 30, 4);
  sprintf (nombre_z, "PROGRESO"); /* string  */
  str_length_nombre= strlen(nombre_z);
  is_null= 1;

  /* Execute the INSERT statement - 1*/
  if (mysql_stmt_execute(stmt)) {
     fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
     fprintf(stderr, " %s\n", mysql_stmt_error(stmt)); 
     exit(0);
  }

  memset(clave_z, 0, 4);
  sprintf (clave_z, "AL"); /* string  */
  str_length= strlen(clave_z);

  memset(nombre_z, 30, 4);
  sprintf (nombre_z, "ALMACEN GENERAL"); /* string  */
  str_length_nombre= strlen(nombre_z);
  is_null= 1;

  /* Execute the INSERT statement - 1*/
  if (mysql_stmt_execute(stmt)) {
     fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
     fprintf(stderr, " %s\n", mysql_stmt_error(stmt)); 
     exit(0);
  }


  if (mysql_query(con, "SELECT * FROM almacen")) {
      finish_with_error(con);
  }
  
  MYSQL_RES *result = mysql_store_result(con);
  
  if (result == NULL) {
      finish_with_error(con);
  }

  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;
  
  while ((row = mysql_fetch_row(result))) 
  { 
      for( ii_z = 0; ii_z < num_fields; ii_z++) 
      { 
          printf("%s ", row[ii_z] ? row[ii_z] : "NULL"); 
      } 
          printf("\n"); 
  }
  
  mysql_free_result(result);
  mysql_close(con);

  exit(0);
}
