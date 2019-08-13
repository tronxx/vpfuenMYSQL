/* Function to detect validity of function key touched */
/* chefun3.c - for new way of accessing instructions Sep 10 88 (880930) */
/* fixed to be used with ncurses -- April 8 1997 */

chefun (int i)
{
  unsigned int c;
  extern int lok, surk[], fk[], dk[], newp;
  extern int kivo, inpu, krow[], kcol[], tkk;
  if (tkk == 0)
    c = i;
  else
    c = tkk;
  tkk = 0;
  /* first check in dk (direction keys) */
  if ((newp = dk[c]) != 0)
    return (1);
  /* now check in function keys */
  if ((newp = fk[c]) != 0)
    {
      surk[kivo++] = lok;
      return (1);
    }
  return (0);
}
