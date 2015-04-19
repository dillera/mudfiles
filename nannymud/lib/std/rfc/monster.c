/* Written by Profezzorn */
/** Don't forget to inherit monster_setup.h **/

#include "/std/rfc/monster_setup.h"
inherit "/std/rfc/simple_monster";

/*** Variables ***/

mixed responses=({});

/*** Setup functions ***/

void set_responses(mixed *res) { responses=res; }

/*** Query functions ***/

mixed *query_responses() { return responses; }

/*** response functions ***/

void catch_tell(string input)
{
  string name;
  if(sizeof(responses) &&
     previous_object()!=this_object() &&
    this_player()!=this_object())
  {
    object tp;

    if(tp=this_player())name=this_player()->query_real_name();
    enable_commands();
    MSETUPD->do_block( ({ ({ input, name, this_object(), tp }) }), responses );
  }
}

int init_command(string s) { return command(s); }
void echo(string s) { tell_room(environment(),s+"\n",({this_object()})); }
void emote(string s) { echo(query_name()+" "+s); }
void say(string s) { emote(" says: "+s); }

void reset(int arg)
{
  ::reset(arg);
  if(!arg) move_object(clone_object("/obj/soul"),this_object());
}
