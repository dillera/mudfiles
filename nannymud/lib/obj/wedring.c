/* Conformed to 'the one indention style' (tm) 
 * and fixed so movement inside inventory doesn't cause messages
 * /Profezzorn
 *
 * 940108 Changed it so that it does not check for presence
 * and ev. linkdeath of partner every damn heartbeat.
 * /Cohen
 *
 * 940325 Fixed so that it doesn't start to glow, if your partner is
 * linkdead when you log on.
 * /Slater
 *
 * 940330 fixed some typos - Moonchild
 *
 * 940622 Fixed so that the ring only starts/stops to glow once.
 * /Slater
 *
 * 950124 Added some new feelings to the ring: showring, fiddle (soul-
 * compatible). Changed short and long descs. Added wedding band and band
 * to id str. Allowed beeping in 'beep partner'and also when glowing, 
 * but only if beepon is on.  /Akasha
 * 
 * 950209 Fixed a typo / Taren
 *
 * 960227 Fixed the 'show ring to <person>' bug and some glaringly wrong
 * grammer. /Akasha (& Profezzorn)
 */

#define SIMP 0
object soul;
static string name;
int inited, glowing;

int beep=1; /* 1 means it will beep */
int query_beep() { return beep; }

string poss(string who)
{
  if(who[-1]=='s') return who+"'";
  return who+"'s";
}

string short()                
{
  if(!environment() || !environment()->query_living())
    return "a wedding ring";

  switch((int)environment()->query_gender())
  {
  case 0: return "a steel wedding ring";
  case 1: return "a gold wedding ring";
  case 2: return "a diamond wedding ring";
  default: return "a strange wedding ring";
  }
}

string extra_look()
{
  return capitalize( (string) environment()->query_pronoun() ) +
    " is wearing a"+(glowing?" glowing":"")+" wedding ring on one of " +
      (string)environment()->query_possessive() +
	" fingers";
} 

string query_long()                
{
  if(!environment() || !environment()->query_living())
    return "It's a lonely wedring.\n";

  switch((int)environment()->query_gender())
  {
  case 0: 
    return ("A strong and sturdy wedding ring made of steel.\n"+
	    "It carries the inscription... \n"+
	    "'To "+capitalize(environment(this_object())->query_real_name())+
	    " with eternal love from "+name+"'.\n");
    break;
  case 1:
    return ("A handsome, highly polished solid 18K gold wedding band.\n"+
	    "It carries the inscription... \n"+
	    "'To "+capitalize(environment(this_object())->query_real_name())+
	    " with eternal love from "+name+"'.\n");
    break;
  case 2:
    return ("A beautiful one carat, crystal clear, diamond wedding ring\n"+
	    "with an 18K gold band. It carries the inscription... \n"+
	    "'To "+capitalize(environment(this_object())->query_real_name())+
	    " with eternal love from "+name+"'.\n");
    break;
  }
}

void long()
{
  write(query_long());
  write("Type 'help ring' for a list of commands availible.\n");
}

string query_auto_load() { return "/obj/wedring:"+beep; }

int get() { return 1; }

int id(string str)
{
  return (str=="wedring" || str=="ring" || str=="wedding ring" ||
	  str=="wedding band" || str=="band");
}

void im_here()
{
  object ob,*pl;
  pl = ({ ob = environment(this_object()) });
  if(!glowing)
  {
    tell_object(ob, "Your wedding ring starts to glow softly.\n");
    if(beep)
      tell_object(ob, ""); /* add control-g in ed here */
    glowing = 1;
    if(environment(ob))
    {
      tell_room(environment(ob), poss(ob -> query_name())+
		" wedding ring starts to glow softly.\n",pl);
    }
  }
}

void check_partner()
{
  object o,ring;
  if(stringp(name) && (o=find_player(lower_case(name))) && !o->query_ld())
  {
    im_here();
    if(ring=present("wedring", o))
      ring->im_here();
  }
}

void init()
{
  if(environment(this_object()) != this_player()) return;

  if(!inited) 
    call_out("init_zzz",0);
  else
    check_partner();

  if(!this_object()) return;

  if(soul=present("soul",this_player()))
    soul->add_verb((["fiddle": ({SIMP,({"thoughtfully"})," start$ fiddling with \nYOUR wedding ring \nHOW","",""}), ]));
    
  add_action("beep","beep");
  add_action("show","show");
  add_action("help","help");
  add_action("admire","admire");
  add_action("ring","ring");
}

void init_arg(string str)
{
  sscanf(str,"%d",beep);
}

int help(string str) 
{
  if(id(str))
  {
    write("You can 'show ring to <name>' to show it off to your friends.\n"+
	  "You can 'fiddle' with your ring thoughtfully. (soul-compatible)\n"+
	  "You can 'admire ring' to show everyone how much you like it.\n"+
	  "You can 'beep partner' to try to get some attention.\n"+
	  "You can 'ring beep on/off' to hear the ring beeping at you.\n"
	  );
    return 1;
  }
}

int drop(int silently)
{
  object o,ring;
  if(query_verb()=="quit" &&
     (o=find_player(lower_case(name))) &&
     (ring=present("wedring", o)))
    ring->bye();
  return 1;
}

void init_zzz()
{
  mixed data;
  if(data = (mixed)"/obj/daemon/wedd"->query_spouse(environment()->query_real_name()))
  {
    if(!"/obj/daemon/fingerd"->playerp(lower_case(data[0])))
      {
	write("Your spouse no longer exists.\n");
	"/obj/daemon/wedd"->divorce(environment()->query_real_name());
	return;
      }
    name=capitalize(data[0]);
    write("Your wedding ring slips onto your finger.\n");
  } else {
    name=0;
    if (environment(this_object())->query_level()>21)
    {
      switch(environment()->query_gender())
      {
       case 0: name="the Dump Monster"; break;
       case 1: name="Jennifer"; break;
       case 2: name="Harry"; break;
      }
      return;
    }
    write("You don't seem to be married.\n"+
	  "You have no reason to possess this ring.\n");
    write("The ring disappears in a puff of smoke.\n");
    destruct(this_object());
    return;
  }
  inited=1;
  check_partner();
}

string query_spouse()  { return name; }

void bye()
{
  object ob, *pl;
  pl=({ ob=environment(this_object()) });
  if(glowing)
  {
    glowing = 0;
    tell_object(ob,"Suddenly your wedding ring goes dark!\n");
   if(environment(ob))
    {
      tell_room(environment(ob),"Suddenly "+poss(ob->query_name())+
	        " wedding ring goes dark.\n",pl);
    }
  }
}

/* Added by Chrystoval since I like the feeling beep as well... ;) */
int beep(string arg)
{
  object ob,ring;
  if(arg!="partner") return 0; 
  ob=find_player(lower_case(name));
  if(!ob)
    return write("Sorry but "+capitalize(name)+
		 " doesn't seem to be on right now.\n"),1;
  if(ob->query_ld())
    return write("Sorry but "+capitalize(name)+
		 " seems to be linkdead right now.\n"),1;  
  write("You beep "+capitalize(name)+" to get some attention.\n");
  tell_object(ob,capitalize(this_player()->query_real_name())
	      + " beeps you to get some attention.\n");
  if((ring=present("wedring",ob)) && ring->query_beep())
    tell_object(ob,""); /* add control-g in ed here */
  return 1;
}

int ring(string onoff)
{
  notify_fail("Use 'ring beep on' or 'ring beep off'\n");
  if(!onoff) return 0;
  onoff=lower_case(onoff);
  if(!sscanf(onoff,"beep %s",onoff)) return 0;
  if(onoff != "on" && onoff != "off") return 0;
  beep = onoff=="on";
  if(beep)
    write("Ok, beeping is turned on.\n");
  else
    write("Ok, beeping is turned off.\n");
  return 1;
}

int show(string str)
{
  object ob;
#if 0 /* Not working - Prof 960227 */
  if(!str || !id(str)) { /* Banshee 960114 */
      notify_fail("Use show ring to <person>\n");
      return 0; }
  str=lower_case(str);
#else
  notify_fail("Use show ring to <person>\n");
  str=lower_case(str||"");
#endif
  if(!sscanf(str,"ring to %s",str)) return 0;

  if(ob=present(str,environment(this_player())))
  {
    write("You proudly show your ring to "+capitalize(str)+".\n");
    tell_object(ob,capitalize(this_player()->query_name())+
		" proudly shows you "+this_player()->query_possessive()+
		" wedding ring.\n");
    tell_object(ob,query_long());
    tell_room(environment(ob),capitalize(this_player()->query_name())+
	      " proudly shows "+this_player()->query_possessive()+
	      " ring to "+capitalize(str)+".\n",({this_player(),ob}));

  }else{
    write("You cannot see "+capitalize(str)+" here!\n");
  }
  return 1;
}

int admire(string str)
{
  object ob; 
  ob = environment(this_player());
  if(!str) return 0;
  if(str == "ring")
    {
      write("You hold out your hand and admire your wedding ring happily.\n");
      tell_room(ob,capitalize(this_player()->query_name())+" admires "+
		this_player()->query_possessive()+" wedding ring happily.\n",
		({this_player()}));
      if(this_player()->query_gender() == 2)
	write("Your diamond ring catches the light and sparkles delightfully.\n");
      if(this_player()->query_gender() == 1)
	write("Your wedding ring catches the light and emits a golden hue.\n");
      if(this_player()->query_gender() == 0)
	write("Your wedding ring reflects the light with a silvery sheen.\n");
      return 1;
    }
}

void _signal_lost_connection()
{
 object o,ring; 
  if(!name && this_object())
    {
    destruct(this_object());
    return;
  }
  if((o=find_player(lower_case(name))) && (ring=present("wedring", o)))
    ring->bye();
}


