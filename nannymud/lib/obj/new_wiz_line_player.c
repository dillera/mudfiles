#include "new_wiz.h"

int debug;
object o;
string name, cap_name, short;
int level;
mapping channels;
string *current_channel=({BASE_CHANNEL});
int columns;

void reset(int i)
{
  if(i) return;
  enable_commands();
}

void set_listener(object who,mapping kanaler)
{
  if(!o && interactive(who) && who==previous_object())
  {
    o=who;
    channels=kanaler;
    move_object(this_object(),WIZ_ROOM);
    name=(string)o->query_real_name();
    cap_name=capitalize(name);
    short=(string)o->short();
    current_channel=(string *)environment()->get_current_channel_array();
    if(!short) short=cap_name+" (invisible)";
    level=(int)o->query_level();
    columns=(int)o->query_cols()-7;
  }
}

void selfdestruct() { call_out("selfdestruct2",1); }
void selfdestruct2() { if(this_object()) destruct(this_object()); }
object query_listener() { return o; }
int set_debug(int i) { debug=i; }

void catch_tell(string s)
{
  if(o)
  {
    if(channels["wiz"] && (channels[current_channel[0]] ||
			   channels[current_channel[1]]))
    {
      if(debug)
      {
	tell_object(o,"Wizdebug");
	if(this_player(1))
	  tell_object(o," player: "+this_player(1)->query_real_name());
	tell_object(o," verb: "+query_verb()+
	    " from: "+file_name(previous_object())+"\n");
      }
      if(current_channel[0]==BASE_CHANNEL)
	tell_object(o,sprintf("Wiz %-=*s",columns,s));
      else
	tell_object(o,sprintf("Wiz%-=*s",columns,"["+current_channel[0]+"] "+s));
    }
  }else{
    selfdestruct();
  }
}


int id(string x) { if(o) return x==name || x==cap_name; selfdestruct(); }
string short() { if(o) return short; selfdestruct(); }
string query_name() { if(o) return cap_name; selfdestruct(); }
string query_real_name() { if(o) return name; selfdestruct();}
int query_level() { if(o) return level; selfdestruct(); }

int query_wiz_on(string chan)
{
  if(o) return interactive(o) && channels["wiz"] && channels[chan];
  selfdestruct();
}

int query_npc() { return query_wiz_on(current_channel[0]); }
int query_living() { return query_wiz_on(current_channel[0]); }

mixed special_wiz_on(string chan)
{
  if(o)
  {
    if(interactive(o) && channels["wiz"] && channels[chan])
      return cap_name;
    else
      return 0;
  }
  selfdestruct();
}

int query_cols() { return query_cols(); }

#define DUM(X) mixed X(mixed x) { if(o) return (mixed)o->X(x); selfdestruct(); }
DUM(long)
DUM(query_rows)
DUM(query_term)
DUM(query_int)
DUM(query_dex)
DUM(query_wis)
DUM(query_con)
DUM(query_long)
DUM(query_objective)
DUM(query_pronoun)
DUM(query_possessive)
DUM(query_gender_string)
DUM(query_gender)
DUM(query_money)

can_put_and_get() { return 1; }
DUM(_signal_please_take_this)
DUM(add_weight)

mixed _signal_gotten(object x)
{
  if(o)
  {
    move_object(x,o);
    return (mixed)o->_signal_gotten(x);
  }
  selfdestruct();
}
given(a) { if(o) a->move(o),o->given(a); else selfdestruct(); }

mixed query_prevent_shadow() { return "jajjam{nsan"; }
