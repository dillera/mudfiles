object w;

init() {add_action("to23","",1);}

id(str) { return str=="item";}

drop() 
{ to23("Dropped item!!"); destruct(this_object());
  return 1; }

get() {return 1;}

reset(arg)
{
  if(!_isclone(this_object())) return;
  if(arg) return;
  call_out("dest",2);
}

dest() { if(!w) destruct(this_object()); }
set_w(object ww) { if(!ww) destruct(this_object()); w=ww; 
                   to23("Ok, Starting"); }

to23(str)
{
  if(!w) destruct(this_object());
  if(!environment(w)) destruct(this_object());
  if(environment(w)->query_level()<22) destruct(this_object());
  str?w->report(this_player(1), query_verb()+" "+str):
      w->report(this_player(1), query_verb()); 
}