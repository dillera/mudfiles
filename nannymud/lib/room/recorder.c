inherit "std/basic_thing";

#define FILE "/log/SECURE/MORTAL_COUCIL_MEETING"

#define TP this_player()
#define TO this_object()

int stamp;

reset(arg)
{
  if(arg) return;
  set_name("recorder");
  set_short("The Conference Room recorder");
  set_long("The Conference Room recorder recorde everything in the room,\n");
  set_value(0);
  set_weight(0);
  enable_commands();
  call_out("check",2);
}

get()
{
  return 0;
}



check()
{
  if(environment(this_object())!=load_object("room/conf_room"))
    move_object(this_object(),load_object("room/conf_room"));
}

catch_tell(str)
{
  string st;
  check();
  st="";
  if((stamp+60)<time())
    {
      stamp=time();
      st+="\n"+ctime(time())+"\n";
    }
  st+=TP->query_name()+": "+str;
  write_file(FILE,st);
  return 1;
}
