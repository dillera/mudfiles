int level;

void reset()
{
  string a,b;

  if (sscanf( file_name( this_object() ), "%s#%s", a, b) == 2)
  {
    destruct( this_object() );
    return;
  }
}

status wizardp( string str )
{
  return "/obj/daemon/fingerd"->wizardp(str);
}

status playerp( string str )
{
  return "/obj/daemon/fingerd"->playerp(str);
}
