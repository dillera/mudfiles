#include "new_wiz.h"
private mapping channels;
private int all;
static object wiz_player;

#define PROTECT nomask static protected private

PROTECT int join_wiz_channel(string channel)
{
   if(member_array(channel, POWER_REST_CHAN) != -1)
      if((int) this_player()->query_level() < 30)
	 return 0;
  if(-1!=member_array(channel,SUPER_REST_CHAN))
    if(this_player()->query_level()<25)
      return 0;

  if(-1!=member_array(channel,REST_CHAN))
    if(this_player()->query_level()<23)
      return 0;

  if(-1!=member_array(channel,SKILLED_CHAN))
    if(this_player()->query_level()<22)
      return 0;


  return channels[channel]=1;
}

PROTECT int leave_wiz_channel(string channel)
{
  int tmp;
  tmp=channels[channel];
  channels[channel]=0;
  return tmp;
}

PROTECT mapping query_channels()
{
  int e;
  mixed tmp;

  if(mappingp(channels)) return channels;
  tmp=channels;
  channels=([]);
  if(pointerp(tmp))
  {
    for(e=0;e<sizeof(tmp);e++)
      join_wiz_channel(tmp[e]);
  }else{
    join_wiz_channel(BASE_CHANNEL);
    join_wiz_channel(TOGGLE_CHANNEL);
  }
  if(all) join_wiz_channel("all");
  all=0;
  return channels;
}

nomask static object get_wiz_player()
{
  mixed tmp,tmp2;
  if(!wiz_player)
  {
    tmp=query_channels();
    tmp=mkmapping(m_values(tmp),m_indices(tmp));
    while(m_sizeof(tmp2=m_delete(tmp,0))<m_sizeof(tmp)) tmp=tmp2;
    if(this_player()->query_level()<23)
    {
      tmp=(m_values(tmp)-SUPER_REST_CHAN)-REST_CHAN - POWER_REST_CHAN;
    }else if(this_player()->query_level()<25){
      tmp=m_values(tmp)-SUPER_REST_CHAN - POWER_REST_CHAN;
    }
    else if((int) this_player()->query_level() < 30)
      tmp = m_values(tmp) - POWER_REST_CHAN;
    else{
      tmp=m_values(tmp);
    }
    channels=mkmapping(tmp,tmp);
    wiz_player=clone_object(WIZ_PLAYER);
    wiz_player->set_listener(this_player(),query_channels());
  }
  return wiz_player;
}

PROTECT void wiz_line_list(string channel)
{
  mapping channels,tmp,tmp2;
  string *who;

  channels=query_channels();

  write("You are listening on these channels:\n");

  tmp=mkmapping(m_values(channels),m_indices(channels));
  while(m_sizeof(tmp2=m_delete(tmp,0))<m_sizeof(tmp)) tmp=tmp2;
  write(sprintf("%#-*s\n",
		(int)this_player()->query_cols()-2,
		implode(m_values(tmp), "\n"))); 

  who=(string *)all_inventory(load_object(WIZ_ROOM))->
    special_wiz_on(channel)-({0});

  write("\nActive wizards on channel: " + channel + "\n");

  write(sprintf("%#-*s\n",
		(int)this_player()->query_cols()-2,
		implode(who , "\n"))); 
    
}

nomask static void init_wiz_line() 
{
  get_wiz_player();
  join_wiz_channel(BASE_CHANNEL);
  join_wiz_channel(TOGGLE_CHANNEL);
}

nomask static void update_wiz_line()
{
  if(wiz_player)
  {
    destruct(wiz_player);
    get_wiz_player();
  }
}

PROTECT void wiz_help()
{
  write("------- Wizline Help -------\n\n");
  write(WIZ_COMMAND + "[channel] <str>\tSend <str> over wizline.\n");
  write(WIZ_COMMAND + " /on   [channel]\tTurn on the wizline.\n");
  write(WIZ_COMMAND + " /off  [channel]\tTurn off the wizline.\n");
  write(WIZ_COMMAND + " /list [channel]\tShow who is listening.\n");
  write("The following two options has been added to the basic channel.\n");
  write(WIZ_COMMAND + " !<command>\tDoes a command over wizline.\n");
  write(WIZ_COMMAND + " #<emote>\tDoes an emote over wizline.\n");
}

PROTECT int check_channel(string channel)
{
  if(!channels["wiz"])
  {
    write("Your wizline is off, ('"+WIZ_COMMAND+" /on "+TOGGLE_CHANNEL+
	  "' turns it on.)\n");
    return 1;
  }

  if(!channels[channel] && !channels["all"])
  {
    write("You are not listening to that channel.\n");
    return 1;
  }
  return 0;
}

PROTECT tell_wiz_room(string mess)
{
  if(strlen(mess)<200)
    tell_room(WIZ_ROOM,mess);
  else
    all_inventory(load_object(WIZ_ROOM))->catch_tell(mess);
}

status wiz(string mess) 
{
  string channel;
  object e;
  
  if(this_player(1) != this_object() ||
     previous_object() != this_object())
  {
    write("Ajabaja!!\n");
    return 0;
  }

  if(query_verb()=="wizlist") return 0;

  if(!mess) 
  {
    wiz_help();
    return 1;
  }

  sscanf(query_verb(),WIZ_COMMAND+"%s",channel);
  if(channel=="") channel=BASE_CHANNEL;
    
  get_wiz_player();

  switch(mess[0])
  {
      
  case '!':
    if(check_channel(channel)) return 1;
    WIZ_ROOM->do_security(channel);
    if(channel==BASE_CHANNEL)
      write("Wiz ");
    else
      write("Wiz["+channel+"] ");
    e=environment(this_player());
    move_object(this_player(),WIZ_ROOM);
    move_object(get_wiz_player(),VOID);
    catch(command(extract(mess,1)));
    if(e)
    {
      if (this_player()) /* Bugged frequently on 'quit' over wizline. */
	move_object(this_player(),e);
      move_object(get_wiz_player(),WIZ_ROOM);
    }
    WIZ_ROOM->reset_security();
    return 1;

  case '#':
    if(check_channel(channel)) return 1;
    WIZ_ROOM->do_security(channel);
    tell_wiz_room(capitalize((string)this_player()->query_real_name())+" "+extract(mess,1)+"\n");
    WIZ_ROOM->reset_security();
    return 1;

  case '/':
    sscanf(mess,"%s %s",mess,channel);
    if(channel=="") channel=BASE_CHANNEL;
    switch(mess)
    {
    case "/help":
      wiz_help();
      return 1;

    case "/all":
      if(!SEC_CHECK) return 0;

      if(channels["all"])
	leave_wiz_channel("all");
      else
	join_wiz_channel("all");

      write("All is now "+(channels["all"]?"on":"off")+".\n");
      return 1;

    case "/list":
      if(check_channel(channel)) channel=BASE_CHANNEL;
      wiz_line_list(channel);
      return 1;

    case "/on":
      if(!join_wiz_channel(channel))
	 write("You failed to join channel " + channel + ".\n");
      else
	 write("Channel " + channel + " is now on.\n");
      return 1;

    case "/off":
      leave_wiz_channel(channel);
      write("Channel "+channel+" is now off.\n");
      return 1;

    }
    
    if(channel != BASE_CHANNEL) mess+=" "+channel;

    sscanf(query_verb(),WIZ_COMMAND+"%s",channel);
    if(channel=="") channel=BASE_CHANNEL;

  default:
    if(check_channel(channel)) return 1;
    WIZ_ROOM->do_security(channel);
    tell_wiz_room(capitalize((string)this_player()->query_real_name())+": "+mess+"\n");
    WIZ_ROOM->reset_security();
    return 1;
      
  }
}
