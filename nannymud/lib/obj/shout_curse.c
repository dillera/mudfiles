/*
 * This is a curse that the player can't get rid of.
 * It prevents you from shouting.
 */

int end_time;

query_auto_load() {
  return "obj/shout_curse:" + (end_time-time());
}

start(ob)
{
  move_object(this_object(), ob);
  end_time = time()+3600;
  tell_object(ob, "You get a sore throat suddenly, without any reason.\n");
}

id(str) { return str == "shout_curse"; }
long() { write("How can you look at a curse ?\n"); }
drop() { return 1; }
init() { add_action("do_shout", "shout"); }

do_shout()
{
  if (time() < end_time)
  {
    write("You can't shout with a sore throat !\n");
    say(this_player()->query_name()+" makes croaking sounds.\n");
    return 1;
  } else {
    destruct(this_object());
    return 0;
  }
}

init_arg(str)
{
  sscanf(str, "%d", end_time);
  end_time+=time();
}

extra_look()
{
  if (time() < end_time)
  {
    return "the throat seems to be sore";
  } else {
    destruct(this_object());
  }
}

_exit()
{
  if(time()<end_time && query_verb()!="quit")
  {
    write_file("log/SECURE/shoutcurse",
	       "Shoutcurse destructed at "+ctime(time())+" "+
	       (this_player() && "TP: "+this_player()->query_real_name()+"("+this_player()->query_level()+") ")+
	       (this_player(1) && "TI: "+this_player(1)->query_real_name()+"("+this_player(1)->query_level()+") ")+
	       (environment() && "ENV: "+environment()->query_real_name()+"("+environment()->query_level()+") ")+
	       (previous_object() && "PO: "+file_name(previous_object())+" ")+
	       (query_verb() && "verb: "+query_verb())+
	       "\n"
	       );

	       
  }
}
