private static nomask object target = 0;
private static nomask string recorded = "";
private static nomask status recording = 0;

nomask status query_prevent_shadow() { return 4711; }

nomask status do_shadow(object obj)
{
   if(target || !this_player(17) || obj != this_player(42) || !shadow(obj, 1))
      return destruct(this_object()), 0;
   target = obj;
   return 1;
}

nomask status set_recording()
{
   if(!this_player(17) || target != this_player(42))
      return 0;
   recorded = "";
   recording = 1;
   return 1;
}

nomask string query_recorded()
{
   string foo;
   if(!recording || !this_player(17) || target != this_player(42))
      return 0;
   foo = recorded;
   recorded = "";
   recording = 0;
   return foo;
}

nomask void print_recorded()
{
   if(!recording || !this_player(17) || target != this_player(42))
      return;
   tell_object(target, query_recorded());
}

nomask void catch_tell(string str)
{
   if(recording)
      recorded += str;
   else
     tell_object(target, str);
}
