#define FILE "/obj/party"

void party_help()
{
  this_player()->simplemore(read_file("/obj/party_help.txt"));
  this_player()->flush();
}

void long()
{
  write("This is a party object.\n");
  write("It supports these commands:\n");
  write("----------------------------------------------------------------\n");
  cat("/obj/party_brief.txt");
  write("----------------------------------------------------------------\n"); 
}

object master=this_object(),*user=({}),expected_environment;
string short="a party object (%s)",party_name="Unnamed";
int total_exp;
mapping player=([]);


mixed get_player_index(object o,string what)
{
  return mappingp(player[o]) && player[o][what];
}

varargs object *members(int i)
{
  string what;
  if(i)
  {
    what="w";
  } else {
    what="o";
  }
  return map_array(m_indices(player),"get_player_index",master,what)-({0});
}

status id(string s)
{
  return s=="party_ob"||s=="object"||s=="party"||s=="party object";
}

void send(object ob,string s)
{
  if(!ob||!player[ob]||!player[ob]["j"]) 
    return;
  tell_object(player[ob]["o"],s);
}

status party_write(string s)
{
  if(!master)
    return 1;
  if(this_object()!=master)
    return (status)master->party_write(s);
  map_array(user,"send",this_object(),sprintf("Party: %-=*s",70,s));
  return 1;
}

void init()
{
  if(!master)
    return;
  if(master==this_object() && environment(this_object())==this_player())
    add_action("do_command","",1);
  if(environment(this_object())==this_player())
  {
    add_action("do_party_command","party");
    master->assert_player();
  }
}

void share_exp_2(int xp, object room)
{
/* Somewhat recoded by Celeborn to fix
   so that players not present shouldnt 
   get any shares of the xp. */

  int share,i,tot_share;
  int no_present_players;

  total_exp+=xp;
  user-=({ 0 });
  no_present_players = 0;

  for(i=0;i<sizeof(user);i++)
   if(mappingp(player[user[i]]) && 
      player[user[i]]["s"] && player[user[i]]["j"])
    {
      if (present(player[user[i]]["o"],room) 
	  /* Added by Banshee to prevent ghost from gaining xp */
	  && !(player[user[i]]["o"]->query_ghost()))
      {
	tot_share+=player[user[i]]["s"];
	no_present_players++;
      }
    }
  
  
  for(i=0;i<sizeof(user);i++)
  {
    if(mappingp(player[user[i]]))
    {
      if(!player[user[i]]["j"]) continue;
      
      if(present(player[user[i]]["o"],room) 
	 /* Added by Banshee to prevent ghost from gaining xp */
	 && !(player[user[i]]["o"]->query_ghost())) {
	if(!catch(player[user[i]]["o"]->
		  add_exp((xp*player[user[i]]["s"])/tot_share)))
	{
	  player[user[i]]["exp"]+=(xp*player[user[i]]["s"])/tot_share;
	  /* Taren, 23 Apr 1995, to lessen the confusion amongst the players*/
	  party_write("Experience points divided.\n");
#if 0
	  party_write( player[user[i]]["n"]+" got "+
		      (player[user[i]]["s"] * 100 ) / tot_share+
		      "% of the total ( " +(xp*player[user[i]]["s"])/tot_share
		      +" xp )\n");
#endif
	}
      } else {
	/* Added by Banshee to let the party know that ghosts don't get xp */
	if (player[user[i]]["o"]->query_ghost())
	  party_write( player[user[i]]["n"]+" got no xp. (Ghost)\n");
	else
	  party_write( player[user[i]]["n"]+" got no xp. (Not present)\n"); 
      }
    }
  }
}


void share_exp(int xp) 
{
  if(!master)
  {
    destruct(this_object()); /* Party disbanded */
    return;
  }
  
  if(master!=this_object()) 
    master->share_exp_2(xp, environment(environment(this_object())));  
  else
    share_exp_2(xp, environment(this_player()));
}


int total_money;
void share_money(int money) 
{
  int share,i,tot_share;
  
  total_money+=money;
  
  user-=({ 0 });
  
  for(i=0;i<sizeof(user);i++)
    if(player[user[i]] 
       /* Added by Banshee to prevent ghost from gaining money */
       && !(player[user[i]]["o"]->query_ghost())
       && (present(player[user[i]]["o"], environment(this_player()))))
      tot_share++;
  
  for(i=0;i<sizeof(user);i++)
  {
    if(player[user[i]] && player[user[i]]["j"] 
       /* Added by Banshee to prevent ghosts from gaining money */
       && !(player[user[i]]["o"]->query_ghost())
       && (present(player[user[i]]["o"], environment(this_player()))))

    {
      player[user[i]]["o"]->add_money(money/tot_share);
      player[user[i]]["gld"]+=money/tot_share;
      party_write(player[user[i]]["n"]+" got "+money/tot_share+" coins.\n");
    }
  }
}

void set_master()
{
  master=previous_object();
  call_out("selfdest",120);
}

void selfdest()
{
  party_write(environment()->query_name()+" didn't join the party.\n");
  destruct(this_object());
}

string short()
{
  if(!master)
  {
    destruct(this_object());
    return 0;
  }
  return short&&sprintf(short,master->master_name());
}


status stat;

void foo(object who) /* Cleaned up a bit. Banshee 950815 */
{
  string s;
  if(!objectp(who) || !player[who]["j"])
    return;
  write(sprintf("%1s %-11s%2d   %3d   %3d/%3d  %3d/%3d    %2s\n",
		(who==this_object()?"L":" "),
		player[who]["n"],player[who]["o"]->query_level(),
		player[who]["s"],player[who]["o"]->query_hp(),
		player[who]["o"]->query_max_hp(),
		player[who]["o"]->query_spell_points(),
		player[who]["o"]->query_max_sp(),
		(player[who]["f"]?"F":" ")+
		(environment(player[who]["o"])==environment(this_player())
		 ?"P":" ")));
		/* capitalize((((string)(environment(player[who]["o"])
				       || this_object())->short())|| 
			    "unknown")[0..40]));  Banshee 950814 */ 
}

status party_status()
{
  write(party_name+" ("+sizeof(user)+" member"+(sizeof(user)>1?"s":"")+".)\n");
/*  write("Total experience given: "+total_exp+"\n");
  write("Total gold split      : "+total_money+"\n");  Banshee 950814 */
  write("--Name------Level--Shr---Hp/MHp---Sp/MSp----FP------------------------------\n");
  map_array(user - ({0}),"foo",this_object());
  write("--Name------Level--Shr---Hp/MHp---Sp/MSp----FP------------------------------\n");
  return 1;
}

void party_set(string what,mixed to_what)
{
  int a;
  for(a=0;a<sizeof(user);a++) 
    if(objectp(user[a])&&player[user[a]]&&player[user[a]]["j"])
      player[user[a]][what]=to_what;
}

status party_follow()
{
  if(previous_object()==this_object())
  {
    if(player[master]["f"])
    {
      player[master]["f"]=0;
      party_write("All unfollowing.\n");
      party_set("f",0);
    } else {
      player[master]["f"]=0;
      party_write("All following.\n");
      party_set("f",1);
    }
    move_object(this_object(),environment());
    return 1;
  }
  if(!present(this_player(),environment(environment(this_object()))))
  {
    write("But the leader is not present!\n");
  } else {
    player[previous_object()]["f"]=!player[previous_object()]["f"];
    party_write(player[previous_object()]["n"]+
		(player[previous_object()]["f"]?" started ":" stopped ")+
		"following the leader.\n");
  }
  return 1;
}

status party_join()
{
  if(player[previous_object()]["j"])
  {
    write("You are already a member.\n");
  }else{
    player[previous_object()]["j"]=1;
      party_write(player[previous_object()]["n"]+" is now a member of the party and\n");
    party_write("will receive "+player[previous_object()]["s"]+" shares of the experience.\n");
  }
  return 1;
}

status party_say(object o,string s)
{  
  if(!player[o])
    return 1;
  if(s[0]=='#')
    party_write(player[o]["n"]+" "+s[1..100000]+"\n");
  else
    party_write("["+player[o]["n"]+"] "+s+"\n");
  return 1;
}

status party_name(object o,string s)
{
  if(strlen(s)>78)
  {
    write("To many characters in name.\n");
    return 1;
  }
  party_name=s;
  party_write(player[o]["n"]+" changed the partyname to '"+s+"'\n");
  return 1;
  
}

void assert_player()
{
  if(!mappingp(player[previous_object()]))
    return;
  player[previous_object()]["o"]=this_player();
}

void query_me_joined()
{
  return mappingp(player[previous_object()]) && player[previous_object()]["j"];
}

status do_party_command(string s)
{
  mixed *c;

  if(!master)
  {
    write("Error: No party-leader.\n");
    destruct(this_object());
    return 1;
  }
  master->assert_player();
    if(!s) s="status";
  c=explode(s," ");
  if(!c) c=({ s });
  c-=({""});
  user-=({ 0 });

  if(c[0]!="join" && !master->query_me_joined())
  {
    write("You must join the party first.\n");
    return 1;
  }
  switch(c[0])
  {
   case "status":
    if(!master) return 0;
    return (status)master->party_status();
    
   case "help":
    party_help();
    return 1;
    
   case "follow":
    if(!master) return 0;
    return (status)master->party_follow();
    
   case "say":
    if(!master) return 0;
    return (status)master->party_say(this_object(),implode(c[1..10000]," "));
    
   case "name":
    if(!master) return 0;
    return (status)master->party_name(this_object(),implode(c[1..10000]," "));

   case "split":
    if(sizeof(c) != 2 ||
       !sscanf(c[1],"%d",c[1]))
    {
      write("Syntax: party split <amount>\n");
      return 1;
    }
    if(!master) return 0;

    if((int)this_player()->query_money()<c[1])
    {
      write("But you do not have more than "+this_player()->query_money()+
	    " gold coins!\n");
      return 1;
    }
    if(c[1]<1)   /* Celeborn 93-08-28 */
    {
      write("You can't share a negative amount of money!\n");
      return 1;
    }

    this_player()->add_money(-c[1]);
    master->share_money(c[1]);
    return 1;

   case "hide":
    if(!short)
    {
      short="a party object (%s)";
      write("Ok, you unhide your partyobject\n");
    } else {
      short=0;
      write("Ok, you hide your partyobject\n");
    }
    return 1;

   case "leave":
    destruct(this_object());
    return 1;
    
   case "reference":
    cat("/obj/party_brief.txt");
    return 1;

   case "list":
    FILE->list();
    return 1;

  case "join":
    remove_call_out("selfdest");
    return (status)master->party_join();

    /* Master functions only */
   case "add":
   {
     object who,ob,mark;
     if(master!=this_object()) return 0;
     if(sizeof(c)<2)
     {
       write("Syntax: party add <who>\n");
       return 1;
     }
     if(!(who=find_player(lower_case(c[1])))) 
     {
       write("Add what player?\n");
       return 1;
     }
     c[1]=capitalize(lower_case(c[1]));
     if(player[c[1]])		/* Already a member... */
     {
       write("But "+c[1]+" is already a member!\n");
       return 1;
     } else if (present("party object",who)) {
       write("But "+c[1]+" is already a member of an other party!\n");
       return 1;
     } else if(!present(who,environment(this_player()))) {
       write("But "+c[1]+" is not present!\n");
       return 1;
     }
     if(who->query_level()>19 && this_player()->query_level()<20)
     {
       write("Mortals cannot party with wizards.\n");
       return 1;
     }

     /*Patch by Taren to please Avis and the Leper guild, 95 Aug 29*/
     mark=present("guild_mark",this_player());
     if(mark&&mark->not_allow_partying(c,who))
       return 1;
     mark=present("guild_mark",who);
     if(mark&&mark->not_allow_partying(c,who))
       return 1;

     ob=clone_object(FILE);
     player[ob]=
       ([
	 "f":0,			/* no automatic follow /Profezzorn */
	 "n":capitalize((string)who->query_real_name()),
	 "o":who,
	 "s":who->query_level(), /* changed to lev. default shares by Slater */
	 "j":0,			/* not joined in yet. */
	 ]);
     player[player[ob]["n"]]=ob;
    
     user+=({ ob });
     user-=({ 0 });	/* Add the new player, and remove all dead ones */
     ob->set_master();
    
     move_object(ob,who);
     tell_object(who,(string)this_player()->query_name()+" invites you to join the party "+party_name+"\n");
     tell_object(who,"Write 'party join' within 2 minutes to accept.\n");
     return 1;
   }

   case "share":
    if(master!=this_object()) return 0;
    if(sizeof(c)<3)
    {
      write("Syntax: party share <who> <how much>\n");
      return 1;
    }
    sscanf(c[2],"%d",c[2]);
    c[1]=capitalize(lower_case(c[1]));
    
    if(!player[c[1]] || !player[player[c[1]]]["o"] ||
       !player[player[c[1]]]["j"])
    {
      write("But there is no such player in the party!\n");
      return 1;
    }
    if((int)player[player[c[1]]]["o"]->query_level()-4>c[2] ||
       c[2]<1||
       (int)player[player[c[1]]]["o"]->query_level()<c[2])
    {
      int foo;
      write("Illegal share. Must be in the range "+
	    (((foo=(int)player[player[c[1]]]["o"]->query_level()-4)<1)?
	     (1):(foo))+
	    " to "+
	    (int)player[player[c[1]]]["o"]->query_level()+".\n");
      return 1; 
      /* New valid share range level -> max((level-4),1) */
      /* Change by Slater, Mon Jan 17 04:49:58 1994      */
    }
    player[player[c[1]]]["s"]=c[2];
    party_write(player[player[c[1]]]["n"]+" will receive "
		+player[player[c[1]]]["s"]+" shares of the experience.\n");
    return 1;
    
   case "remove":
    if(master!=this_object()) return 0;
    if(sizeof(c)<2)
    {
      write("Syntax: party remove <who>\n");
      return 1;
    }

    c[1]=capitalize(lower_case(c[1]));
    if(player[c[1]] && player[player[c[1]]]["j"])
    {
      party_write(c[1]+" was removed from the party.\n");
      destruct(player[c[1]]);
      player[player[c[1]]]=0;
      player[c[1]]=0;
      return 1;
    } else {
      write("Whom?\n");
      return 1;
    }

   case "leader":
    if(master!=this_object()) return 0;
    if(sizeof(c)<2)
    {
      write("Syntax: party leader <who>\n");
      return 1;
    }
    c[1]=capitalize(lower_case(c[1]));
    if(player[c[1]] && player[player[c[1]]]["j"])
    {
      object aa,ab,ba,bb;
      mapping tmp;
      aa=this_object();     /* The master object */
      ab=player[aa]["o"];   /* The former master */
      ba=player[c[1]];      /* The other object. */
      bb=player[ba]["o"];   /* The other player. */

      /* Watch closely now... */
      tmp=player[aa];
      player[aa]=player[ba];
      player[ba]=tmp;
      player[player[aa]["n"]]=aa;
      player[player[ba]["n"]]=ba;
      move_object(aa,bb);            
      move_object(ba,ab);            

      party_write(player[this_object()]["n"]+
		  " is now the leader of the party.\n");      
      return 1;
    } else {
      write("Whom?\n");
      return 1;
    }
  }
}

status do_command(string s)
{
  int a;
  if(stat) return 0;
  if(environment()!=this_player()) return 0;  
  if(expected_environment!=environment(this_player())&&
     (expected_environment=environment(this_player())))
    move_object(this_object(),this_player());
  
  switch(query_verb())
  {
    /* Some wiz commands, for convenience :) */
   case "goto":
   case "goin":

    /* Broms ear-ring, and some guilds */
   case "haven":

    /* a lot of boats */
   case "embarge":
   case "disembarge":
   case "leave":
   case "board":
    

    /* Some trees, and some mountains */
   case "climb":

    /* Valhalla, evil caves... */
   case "mount":

    /* A slightly odd boat, from the Earth Sea trilogy, call lookfar */
   case "call":

    /* Some boring castles */
   case "enter":
   case "exit":

    /* A few boring places as well... */
   case "out":

    /* Standard mud-lib code */
   case "north":
   case "south":
   case "west":
   case "east":
   case "northwest":
   case "southwest":
   case "northeast":
   case "southeast":
   case "up":
   case "down":

    /* Profezzorns cube */
   case "meta-north":
   case "meta-south":
   case "meta-west":
   case "meta-east":
   case "meta-up":
   case "meta-down":
   case "mn":
   case "ms":
   case "mw":
   case "me":
   case "mu":
   case "md":

    stat=1;
    if(this_object()!=master) return 0; /* How should this happend? */
    if(!s) s=query_verb(); else s=query_verb()+" "+s;
    if(!command(s,this_player())) /* There was no such command.
				   * there is no need the check
				   * the environment, to see if
				   * it has changed, really...
				   */
      return notify_fail(""), stat=0;
    call_out("restore",1,({ expected_environment , s }));
    stat=0;
    if(expected_environment!=environment()&&pointerp(user)&&sizeof(user)) 
      /* We moved, and we have followers, may they follow us to their end */

      for(a=0;a<sizeof(user);a++) 
	if(objectp(user[a])&&player[user[a]]["f"] && user[a]!=master)
	  if(environment(player[user[a]]["o"])== expected_environment
	     && !player[user[a]]["o"]->query_ld())
	    command(s,player[user[a]]["o"]);
    expected_environment=environment();
    move_object(this_object(),this_player());          
    remove_call_out("restore");
    return 1;    /* We have done the command here... */
    
   default: /* Not our business */
    return 0;
  }
}

void restore(mixed *c)
{
  stat=0;
  if(pointerp(c) && sizeof(c)<2)
    if(objectp(c[0]))
      command("bug While doing "+c[1]+" in "+file_name(c[0])+
	      " I encountered a bug.",this_player());
    else     
      command("bug While doing "+c[1]+" I encountered a bug.",this_player());
}

string master_name()
{
  if(previous_object()==master)
    return "leader";
  
  return (mappingp(player)&&
	  mappingp(player[master])&&
	  player[master]["n"]) || "leader";
}

void place_new(object where)
{
  move_object(clone_object(FILE),where);
}

int get()
{
  object who,ob;
  if(!interactive(this_player()) || player[master] || 
     present("party_ob",this_player()))
    return 0;
  
  who=this_player();
  FILE->register();
  player[ob=master=this_object()]=
    ([
      "f":1,
      "n":capitalize((string)who->query_real_name()),
      "o":who,
      "s":who->query_level(), /* Changed to lev. default shares by Slater */
      "j":1, /* sure */
      ]);
  player[player[ob]["n"]]=ob;
  
  user=({ master, });
  
  party_write("You now have a master party object.\n");
  party_write("You will receive "+player[ob]["s"]+" shares.\n");
  call_out("place_new", 1, environment(this_player()));
  return 1;
}

object *parties=({});

void register()
{
  parties+=({ previous_object() });
}

void list()
{
  int a;
  parties-=({ 0 });
  for(a=0;a<sizeof(parties);a++)
    parties[a]->short_info();
}

void short_info()
{
  int a;
  user-=({ 0 });
  write(party_name+" ("+sizeof(user)+" member"+(sizeof(user)>1?"s":"")+")\n< ");
  for(a=0;a<sizeof(user);a++)
    write(player[user[a]]["n"]+" ");
  write(">\n");
}

void party_leave(object ob)
{
  int a;
  if(ob==this_object())
  {
    party_write("The party is disbanded.\n");
    user-=({ 0, master });
    for(a=0;a<sizeof(user);a++) 
      if(objectp(user[a]))
	destruct(user[a]);
  } else {
    if(player[ob])
    {
      party_write(player[ob]["n"]+" left the party.\n");
      player[player[ob]["n"]]=0;
      player[ob]["n"]=0;
      return;
    }
  }
}

void _exit()
{
  catch(master->party_leave(this_object()));
}

int drop(int silent)
{
  if(this_player() != environment())
    return 1;
  if(query_verb()!="drop" && (silent || query_verb()=="quit"))
  {
    destruct(this_object());
    return 0;
  }
  write("Type 'party leave' to leave the party.\n");
  if(this_object()==master)
    write("Note: this would disband the party.\n");
  return 1;
}

prevent_insert() { return 1; } /* Brom 950819 */
