#include "new_wiz.h"

object *inv;
string *current_channel;
mapping secure=mkmapping(REST_CHAN+SUPER_REST_CHAN+POWER_REST_CHAN,
			 REST_CHAN+SUPER_REST_CHAN+POWER_REST_CHAN);
mapping all_secure=mkmapping(SUPER_REST_CHAN+POWER_REST_CHAN,
			     SUPER_REST_CHAN+POWER_REST_CHAN);

void dclean(object o) 
{
  if(interactive(o))
  {
    this_player()->move_player("X",CHURCH);
  }else{
    while(first_inventory(o)) destruct(first_inventory(o));
    destruct(o);
  }
}

int wiz_player_p(object o)
{
  return file_name(o)[0..strlen(WIZ_PLAYER)-1]==WIZ_PLAYER;
}

int do_security(string channel)
{
  object *obs;
  int e,nr;
  if(!interactive(previous_object()))
    return 0;

  if(secure[channel])
  {
    nr=sizeof(obs=all_inventory()-inv);
  
    for(e=0;e<nr;e++)
      if(!wiz_player_p(obs[e]))
	dclean(obs[e]);

    inv=all_inventory(this_object());

    if(all_secure[channel])  current_channel[1]="";
  }

  current_channel[0]=channel;
  return 1;
}

void reset_security()
{
  if(!interactive(previous_object())) return 0;
  current_channel[0]="";
  current_channel[1]="all";
}

string *get_current_channel_array()
{
  if(wiz_player_p(previous_object())) return current_channel;
}

void reset()
{
  if(current_channel) return;
  set_light(1);
  inv=({});
  current_channel=({BASE_CHANNEL,"all"});
}

void long()
{
  write("You are teleported to the church.\n");
  this_player()->move_player("X",CHURCH);
}

void init()
{
  if(interactive(this_player())) return;
  if(!wiz_player_p(this_player()))
    dclean(this_player());
}

mixed query_prevent_shadow() { return "Japp"; }
