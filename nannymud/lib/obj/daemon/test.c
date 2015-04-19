inherit "obj/thing";
object a,b,w;

reset(arg)
{
  ::reset();
  if(arg) return;
  if(!_isclone(this_object())) return;
  set_short("En festerist-frack (worn)");
  set_long("Den e kleggig. Den e bra. Den matchar folk! Match <x> <y>\n");
  set_value(0);
  w=this_player(1);
  if((w->query_level())<22) destruct(this_object());
}

id(str) { return str=="frack";}
drop()  { destruct(this_object()); }

mej(str)
{
  tell_object(w,":: "+str+" ::\n");
}

init()
{
  if(this_player()->query_level()<22) destruct(this_object());
  add_action("patch","match");
  add_action("_status","statu");
  add_action("stop","stop");
}

patch(string x, string y)
{
  if(a||b) 
   {mej("Already active. Use 'statu' to see yourself. Use stop to deactive");
    return 1;}
  if(!x) { notify_fail("Usage: match XXX YYY\n"); return; }
  if(!(a=find_player(x))) { mej(x+" not found!"); return 1; }
  make_snooper(a); mej("Adding to "+x);
  if(!y) { mej("No second arg supplied."); return 1; } 
  if(!(b=find_player(y))) { mej(y+" not found...!"); }
  make_snooper(b); mej("Adding to "+y);
  return 1;
}

_status()
{
  mej("checking");
  if(a) mej(a->query_name());
  if(b) mej(b->query_name());
  return 1;
}

stop()
{ stopaa(a); a=0; stopaa(b); b=0; return 1;
}

stopaa(aa)
{
   object bl;
  if(!aa) {mej("(Not snooping this person."); return; }
  bl=present("item",aa);
  if(bl) 
   { 
     mej("Destructing 'item' in player "+aa->query_name());
     destruct(bl);
   } else 
   {
     mej(aa->query_name()+" had no 'item'...");
     if(aa->query_level()<21) {
       mej("That was odd, "+
           aa->query_name()+" is a mortal and should have one.");
        } else
      { 
       mej("Ok, this is wizard... ");
       bl=present("wiztool",aa);
       if(bl) mej("Had a wiztool, shutting report off in it!");
              else
              mej("Odd, a wiz without wiztool? This shouldnt happen, help!");
      }
    }
}


report(object vem, string str) { mej(vem->query_real_name()+" == "+str); }

make_snooper(object whoto)
{
  object a;  
  if(whoto->query_level()<21) 
  {
    mej("Adding snoopobj to mortal, "+
        whoto->query_name()+", level "+whoto->query_level());
    a=clone_object("/obj/daemon/item");
    a->set_w(this_object());
    move_object(a,whoto);
  } else
  {
   mej("Ok, "+whoto->query_name()+" is a wizard even...");
   mej("Patching the wiztool.");
  }
  return 1;
}


