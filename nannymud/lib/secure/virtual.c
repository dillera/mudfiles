/* The master will ask this object for any virtual
 * objects matching a path
 */

#pragma strict_types

status query_prevent_shadow() { return 666; }

private static nomask string dirname(string s)
{
   int i;
   if(strlen(s) < 1 || s[0] != '/' || s == "/")
      return 0;
   for(i = strlen(s) - 1; s[i] != '/'; i--)
      ;
   return i == 0 ? "" : extract(s, 0, i - 1);
}

nomask object get_vobject(string fname)
{
   string str;
   for(str = fname[0] == '/' ? fname : "/" + fname; str; str = dirname(str))
   {
      if(file_size(str + "/vcompile.c") < 0)
	 continue;
      return (object) call_other(str + "/vcompile", "compile_virtual", fname);
   }
   return 0;
}
