int fecha_valida(int anu, int mes, int dia);

/* Funcion que toma recibe una fecha en anu, mes, dia y devuelve -1 si no es
   una fecha valida y un 1 si es fecha valida
 */
int fecha_valida (int anu, int mes, int dia)
{
    int valido_z=1;
    if(dia < 1 || dia > 31) valido_z = -1;
    if(mes < 1 || mes > 12) valido_z = -1;
    if(anu < 1900 || anu > 2050) valido_z = -1;
    switch (mes) {
    case 4:
    case 6:
    case 9:
    case 11:
        if(dia > 30) valido_z = -1; break;
    case 2:
        if(dia > 29) {
            valido_z = -1;
        } else {
          if (anu % 4 != 0 && dia > 28) valido_z = -1;
        }
       break;
    }
    return (valido_z);
}
