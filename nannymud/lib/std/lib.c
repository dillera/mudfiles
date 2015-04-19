/* Written by Profezzorn */
/* An easy and extendable way to make 'ordinary' items */

/*
 * Wanted list:
 * a guard captain
 * a barbarian
 * a barkeep
 * a drunkard
 * a druid
 * a footsoldier
 * a leprachaun
 * more weapons
 * bow and arrows
 * crossbow
 * an ogre
 * more armours
 * leather pants
 * more treasures
 * more tools
 * more food and drinks
 * and improvements to existing things
 */

void reset(int arg)
{
  object *o;
  int e;
  /* some cleaning */
  o=all_inventory(this_object());
  for(e=sizeof(o)-1;e>sizeof(o)/2;e--)
    destruct(o[e]);
}

/* Usage, Just call "/std/lib"->make("orc") or something, and an orc
 * will be returned.
 */

varargs object make(string what, mixed arg)
{
  object o;

  what=replace(what,".","");
  what=replace(what,"/","");
  what=replace(what," ","_");

  if(file_size("/std/constructors/"+what+".c")>0)
  {
    o=load_object("/std/constructors/"+what);
    move_object(o,this_object());
    return (object)call_other(o,"make",arg);
  }
      
  if(file_size("/std/items/"+what+".c")>0)
    return clone_object("/std/items/"+what);

  if(file_size("/std/saves/"+what+".o")>0)
  {
    arg=read_file("/std/saves/"+what+".o",1,1);
    if(sscanf(arg,"__object \"%s\"",arg))
    {
      o=clone_object(arg);
      o->do_restore("std/saves/"+what);
      return o;
    }
  }

  return 0;
}

string *query_makeable_items()
{
  string *a,*b;
  int e;
  a=get_dir("/std/constructors/")+get_dir("/std/items/");
  for(e=0;e<sizeof(a);e++)
    if(!sscanf(a[e],"%s.c",a[e]))
      a[e]=0;

  b=get_dir("/std/saves/");
  for(e=0;e<sizeof(b);e++)
    if(!sscanf(b[e],"%s.o",b[e]))
      b[e]=0;
  
  a+=b;
  a-=({0});
  for(e=0;e<sizeof(a);e++)
    a[e]=replace(a[e],"_"," ");

  return a;
}

