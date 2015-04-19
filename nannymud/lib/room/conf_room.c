inherit "/std/basic_room";

#define TP this_player()
#define TO this_object()

int in_conf=1;
mixed speakers=({});

reset(arg)
{
  set_short("The is the conference room for the Mortal Council");
  set_long(
    "The air is filled with dignity, around this chamber stands chairs.\n"+
    "On the walls hangs portraits of old famous players and Elders.\n"+
    "There is a small bronze plaque on the wall.\n");
  add_item(({"wall","walls"}),"The walls are covered with paintings.\n");
  add_item(({"paintings","painting","portraits","portrait"}),
    "The painting resembles old players,\nMats, Anvil, Angmar, Oros, Qqqq.\n");
  add_item("plaque",
	   "You can do these commands:\n"+
	   "word:     This means you ask for the word.\n"+
	   "has:      Shows who has the word right now.\n"+
	   "give:     'give word to <player>', give the word to player.\n"+ 
           "          You must have the word to be able to give away the word.\n"+
	   "agree:    You agree with <player>.\n"+
	   "disagree: You disagre with <player>.\n"+
	   "say:      Can be used if you have the word. (' doesn't work)\n");
  set_light(1);
  add_property("indoors");
  add_property("no_magic");
  add_property("no_fight");
  add_property("no_teleport");
  move_object(load_object("room/recorder"),TO);
  call_out("get_word",2);
}

get_word()
{
  object ob;
  ob=load_object("room/word");
  if(!ob)
    move_object(load_object("room/word"),TO);
  else
    move_object(ob,TO);
}

toggle_in_conf()
{
  if(in_conf)
    {
      add_exit("church","/room/church");
      in_conf=0;
    }
  else
    {
      remove_exit("church");
      in_conf=1;
    }
}


init()
{
  add_action("block","",1);
}

who_has()
{
  mixed all;
  object ob;
  int i;
  all=all_inventory(TO);
  for(i=0;i<sizeof(all);i++)
    {
      ob=present("has_the_word",all[i]);
      if(ob)
	return all[i]->query_name();
    }
  return "Noone";
}

block(arg)
{
  string s1;
  object ob,word, who;
  switch(query_verb())
    {
    case "tell":
    case "K":
    case "send":
    case "M":
    case "dr":
    case "i":
    case "look":
    case "exa":
    case "examine":
    case "who":
    case "help":
    case "earmuffs":
    case "away":
    case "toggle":
    case "l":
    case "quit":
    case "T":
      return 0;
    case "yes":
      return my_do("yes");
    case "no":
      return my_do("no");
    case "blank":
    case "abstain":
      return agree(arg,"abstain");
    case "agree":
      return agree(arg,"agree");
    case "disagree":
      return agree(arg,"disagree");
    case "has":
	  write(who_has()+" has the word now.\n");	  
	  return 1;
    case "topic":
      return topic(arg);
    case "church":
      return 0;
    case "word":
      ob=present("has_the_word",TP);
      if(!ob)
	{
	  if(member_array(TP,speakers)==-1)
	    speakers+=({TP});
	  else
	    {
	      write("You are already on the list.\n");
	      return 1;
	    }
	  tell_room(TO,TP->query_name()+" asks for the word.\nAnd is added to the speakerslist.\n",({TP}));

	  write("You ask for the word.\n"+
		who_has()+" has the word now.\n");
	  
	  return 1;
	}
      else
	{
	  write("You already have the word.\n");
	  return 1;
	}
    case "pass":
      ob=present("has_the_word",TP);
      if(!ob)
	{
	  write("You don't have the word.\n");
	  return 1;
	}
      if(sizeof(speakers)>0)
	{
	  tell_room(TO,TP->query_name()+" gives the word to "+speakers[0]->query_name()+".\n",({TP}));
	  write("You give the word to "+speakers[0]->query_name()+".\n");
	  transfer(ob,speakers[0]);
	  speakers-=({speakers[0]});
	  return 1;
	}
      else
	{	  
	  tell_room(TO,"There is noone more on the Speakers List.\n");
	  return 1;
	}

    case "end":
      if(arg=="conf"&&TP->query_level()>22)
	{
	  tell_room(TO,TP->query_name()+" annouces that the Council has ended.\n"+
		"An exit has been opend to the Church.\n");
	  toggle_in_conf();
	  return 1;
	}
    case "start":
      if(arg=="conf"&&TP->query_level()>22)
	{
	  tell_room(TO,TP->query_name()+" annouces that the Council has started.\n");
	  toggle_in_conf();
	  return 1;
	}
    case "say":
      if(TP->query_level()>22) return 0;
      if(present("has_the_word",TP))
	return 0;
      else
	return write("You can't speak unless you have the word.\n"),1;
    case "snatch":
      if(TP->query_level()>22) {
	ob=load_object("room/word");
	if(!ob)
	  move_object(load_object("room/word"),TO);
	else {
          who=environment(ob);
	  move_object(ob,TP);
          tell_object(who,TP->query_name()+" takes the word from you.\n");
          tell_room(TO,TP->query_name()+" takes the word from "+who->query_name()+".\n",({who, TP}));
	  write("You take the word from "+who->query_name()+".\n");
        }
        return 1;
      }
    default:
      if(TP->query_level()>22) return 0;
      write("You can't do that here.\n");
      return 1;
    }
}


topic(arg)
{
  if(arg)
    {
      tell_room(TO,TP->query_name()+" suggests '"+arg+"' as the new topic.\n",({TP}));
      write("You suggest '"+arg+"' as the new topic.\n");
      return 1;
    }
  write("Suggest what topic?\n");
  return 1;
}

agree(str,out)
{
  object ob;
  if(!str)
    {
      tell_room(TO,TP->query_name()+" "+out+"s.\n",({TP}));
      write("You "+out+".\n");
      return 1;
    }
  ob=present(str,TO);
  if(ob)
    {
      tell_room(TO,TP->query_name()+" "+out+"s with "+ob->query_name()+".\n",({TP}));
      write("You "+out+" with "+ob->query_name()+".\n");
      return 1;
    }
  else
    {
      write("There is no such player in this room.\n");
      return 1;
    }
}




my_do(str)
{
  object ob;
  if(str)
    {
      tell_room(TO,TP->query_name()+" says: "+str+".\n",({TP}));
      write("You say: "+str+".\n");
      return 1;
    }
  write("Do what?\n");
  return 1;  
}


