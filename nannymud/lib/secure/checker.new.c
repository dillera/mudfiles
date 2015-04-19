/**************************************
 *  A small checker...    /Gwendolyn  *
 **************************************/

#define NUMSAVED        20

private static nomask mixed *report, *spydata;
private static nomask object old;

nomask status query_prevent_shadow() { return 42; }

private nomask status query_sec()
{
   return (this_player(1) && (int) this_player(1)->query_level() > 22);
}

nomask status do_shadow(object obj)
{
   if(!query_sec() || !obj || !shadow(obj, 1))
      return destruct(this_object()), 0;
   old = this_player(1);
   spydata = ({});
   report = ({ obj, ({ obj }) ({ obj }) });
   return 1;
}

nomask mixed *query_listeners(object obj)
{
   if(!query_sec())
      return ({});
   else if(!obj)
      return report + ({});
   else
      return member_array(obj, report[1]);
}

nomask varargs mixed *query_spy(int no)
{
   if(!query_sec())
      return ({});
   else if(no < 1)
      return spydata + ({});
   else
      return spydata[sizeof(spydata)-no..sizeof(spydata)];
}

nomask status add_listener(object obj)
{
   if(!query_sec() || !obj || !report)
      return 0;
   if(member_array(obj, report[1]) == -1)
      report[1] += ({ obj });
   return 1;
}

nomask status sub_listener(object obj)
{
   if(!query_sec() || !report)
      return 0;
   report[1] -= ({ obj });
   return 1;
}

nomask void catch_tell(string str)
{
   string *tmp;
   int i;
   if(!report)
      return;
   if(old != this_player(1))
   {
      old = this_player(1);
      tmp =
      ({
	 (old ? capitalize((string) old->query_real_name()) : "<None>"),
	 (previous_object() ? file_name(previous_object()) : "<NoPrev>"),
	 (query_verb() ? query_verb() : "<NoVerb>"),
	 str + ""
       });
      if(previous_object() != this_object())
	 spydata += ({ tmp });
      report[1] -= ({ 0 });
      for(i = 0; i < sizeof(report[1]); i++)
      {
	 if(old != report[1][i])   /* || old == report[0])  */
	 {
	    tell_object(report[1][i], "Tp: " + tmp[0] + "   From: " + tmp[1] +
			"   Verb: " + tmp[2] + "\n");
	 }
      }
   }
   else if(sizeof(spydata) && previous_object() != this_object())
      spydata[sizeof(spydata) - 1][3] += str;
   for(i = 0; i < sizeof(report[1]); i++)
   {
      if(old != report[1][i] || old == report[0])
	 tell_object(report[1][i], str);
   }
}
