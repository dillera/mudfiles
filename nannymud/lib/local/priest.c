inherit "obj/monster";

int delay, flag;
object par1, par2, obj, attackedby;
int p1gender, p2gender, direct;
string initiator, match;

void tell_present(string str)
{
  flag = 0;
  if(direct == 0)
      tell_room(environment(this_object()),"\nThe priest says...\n" + str);
  else
    tell_room(environment(this_object()),"\nThe priest tells " + this_player()->query_name() + "...\n" + str);
}

/* Profezzorn */
void init2()
{
  string name;
  if(!this_player() || environment(this_player())!=environment())
    return;

  name=capitalize(this_player()->query_real_name());
  if("/obj/daemon/wedd"->query_spouse(name) &&
     !present("wedring",this_player()))
  {
    write("The priest whispers to you: I see you've lost your wedring, here's a new one.\n"+
	  "The priest gives you a wedring.\n");
    say("The priest gives something to "+name+".\n");
    move_object(clone_object("/obj/wedring"),this_player());
  }
}

void init()
{
  if(!this_player() || !interactive(this_player())) return;
  call_out("init2",1);
  add_action("wed", "wed");
  add_action("couples", "married");
  add_action("bless", "bless");
  add_action("divorce","divorce");
  ::init();
}

int divorce(string who)
{
  string spouse;
  mixed data;
  object o,ring;

  if(this_player()->query_level()<21)
  {
    write("You are not allowed to divorce people.\n");
    return 1;
  }
  data=(mixed)"/obj/daemon/wedd"->query_spouse(who);
  if(!data)
  {
    write(capitalize(who)+" is not married.\n");
    return 1;
  }
  spouse=data[0];
  "/obj/daemon/wedd"->divorce(who);
  if(o=find_player(lower_case(who)))
  {
    tell_object(o,"You are now divorced from "+capitalize(spouse)+".\n");
    if(ring=present("wedring",o)) destruct(ring);
  }
  if(o=find_player(lower_case(spouse)))
  {
    tell_object(o,"You are now divorced from "+capitalize(who)+".\n");
    if(ring=present("wedring",o)) destruct(ring);
  }

  write(capitalize(who)+" and "+capitalize(spouse)+" are now divorced.\n");
  return 1;
}

void reset(int arg)
{
  object inv1,inv2;
  int said;

  if (!arg)
  {
    set_name("whiterobed priest");
    set_alias("priest");
    set_level(15);
    set_al(500);
    set_race("human");
    set_short("A whiterobed priest");
    set_long("A whiterobed priest. He informs you that he can perform\n" +
	     "the following services.\n" +
	     "'I can wed a couple, but I need a Wizard as a witness,\n" +
	     " married will list those presently married and\n" +
	     " bless will bless the initiator, the cost is 2000 gold coins.'\n");
    set_wc(0);
    set_ac(0);
  }
  ::reset();
  set_heart_beat(0);
  said=0;
  inv1=first_inventory(this_object());
  /* Preventing the priest from holding unique items for very long times. 
     Banshee 951010 */
  while (inv1) {
    inv2=next_inventory(inv1);
    transfer(inv1,"room/void");
    if (!said && environment(this_object())) {
      tell_room(environment(this_object()),
		"The Priest sacrifices some things to God.\n");
      said=1; }
    destruct(inv1);
    inv1=inv2; }
}

int married(object obj)
{
  return !!"/obj/daemon/wedd"->query_spouse(obj->query_real_name());
}

string husorwife(int str)
{
  if(str == 1)
    return "husband";
  else
    return "wife";
}
    
int wed(string str)
{
  string part1, part2;
  if (this_player()->query_invis())
  {
    write ("You cannot be witness if we cannot see you.\n");
    return 1;
  }

  if (this_player()->query_frog())
  {
    write ("Do not tempt God! Frogs are not apt as witness.\n");
    return 1;
  }

  if (this_player()->query_ghost())
  {
    write ("That is impossible in your immaterial state.\n");
    return 1;
  }

  flag = delay = direct = 0;
  if (!str)  return 0;

  if (this_player()->query_level() < 20)
  {
    tell_present("I'll need a wizard as a witness and it must\n" +
		 "be he who gives the command.\n");
  } else {
    if (sscanf(str,"%s %s",part1,part2) != 2)
    {
      write("The args are as follows...'> wed <player1> <player2>'.\n");
    } else {
      par1 = present(part1);
      par2 = present(part2);
      if (!par1 || !par2)
      {
	write("The priest looks very confused, someone is missing.\n");
	return 1;
      }
      if (!living(par1) || !living(par2) || par1->query_npc() || par2->query_npc())
      {
        write ("Only real people may get married.\n");
      }
      else if (!interactive(par1))
      {
        write (capitalize(part1) + " is only bodily present, " +
        par1->query_possessive() + " spirit is not with us.\n");
      }
      else if (!interactive(par2))
      {
        write (capitalize(part2) + " is only bodily present, " +
        par2->query_possessive() + " spirit is not with us.\n");
      }
      else if(!par1 || !par2) 
      {
        write("The priest looks around searching and says: I cannot see " +
             (par1?capitalize(part2)
                  :capitalize(part1)+(par2?"":" or "+capitalize(part2))) +
	      " here.\n");
      }
      else if (par1 == par2)
      {
        write(capitalize(part1) + " can only marry a " +
        (par1->query_gender_string()=="female"?"man":"woman") + ", not " +
        par1->query_objective() + "self.\n");
      }
      else if (par1->query_frog() || par2->query_frog())
      {
        write ("Frogs may neither marry nor get married.\n");
      }
      else if (par1->query_ghost())
      {
        write ("That is impossible in " + capitalize(part1) + 
        "'s immaterial state.\n");
      }
      else if (par2->query_ghost())
      {
        write ("That is impossible in " + capitalize(part2) + 
        "'s immaterial state.\n");
      }
      else if(married(par1) || married(par2))
      {
	tell_present("What is this? You should be punished! The law forbids\n" +
		     "polygamy. " + this_player()->query_name() + " should punish you " +
		     "in a way he thinks suitable.\n");
      }
      else
      {
	p1gender = par1->query_gender();
   	p2gender = par2->query_gender();
 	if(p1gender == 0 || p2gender == 0)
	{
	  tell_present("The law of the One God does state that marriage\n" +
	 	       "between man/monster or monster/monster is a blasphemy.\n" +
		       "Therefore I have to refuse to wed this couple.\n");
	}
	else if(p1gender == p2gender)
	{
	  tell_present("The law of the One God states that marriage between\n" +
 		       "couples of the same sex is a cardinal sin and I, as the\n" +
		       "voice of the One God, cannot agree to marry this couple.\n");
	}
	else
	{
	  tell_present("As far as I can see there is no reason not to marry\n" +
		       "this couple. If there are anyone who believes that he\n" +
	 	       "or she has a reason to not joining this couple in holy\n" +
		       "matrimony should speak up now, before it's too late.\n");
	  initiator = this_player()->query_name();
	  flag = 1;
	  call_out("marriage",9);
	  return 1;
	}
      }
    }
  }
  flag = 0;
  return 1;
}


int couples()
{
  mixed *pairs;
  int e;
  pairs=(mixed *)"/obj/daemon/wedd"->query_pairs();
  for(e=0;e<sizeof(pairs);e++)
  {
    pairs[e]=capitalize(pairs[e][0])+" was married to "+
      capitalize(pairs[e][1])+" by "+
	capitalize(pairs[e][2])+" "+
	  pairs[e][3];
  }
      
  this_player()->simplemore("The priest starts reading from a scroll.\n" +
    "These are the couples presently married...\n"+
      implode(pairs,"\n")+"\n");
  this_player()->flush();
  return 1;
}

int bless(string arg)
{
  if (arg) return 0;
  direct = 1;
  if(this_player()->query_alignment() > 500) 
  {
    tell_present("You have to search for the One God's blessings in\n" +
		 "other ways, You have already treaded past me on the\n" +
		 "great ladder to saintlyhood,\n");
  }
  else if(this_player()->query_alignment() < -500) 
  {
    tell_present("You are a blasphemy! How dare you enter this church of\n" +
		 "the One God! The burden of your sins I will not remove until\n" +
		 "the day you have proven worthy! Out! Out! Out!!!!\n");
  }
  else if(this_player()->query_money() < 2000)
  {
    tell_present("I am disappointed in you my child! If you don't think\n" +
		 "that the Church is worthy your money then you aren't worthy\n" +
		 "of it's blessings.\n");
  }
  else
  {
    tell_present("Everyone is a son of the One God.\n" +
		 "Receive his blessing and go do his bidding.\n" +
		 "I have heard that the world of Mendor is in\n" +
		 "great need of heroic deeds.\n");
    this_player()->add_alignment(100+random(100));
    this_player()->add_money(-2000);
  }
  direct = 0;
  return 1;
}

void nomarriage()
{
  remove_call_out("marriage");
  tell_present("It seems that a reason has been found and I strongly suggest that\n" +
	       "these problems is sorted out before attempting a new wedding.\n");
}

string par_title(object ob)
{
  string newtitle;
  if(newtitle = ob->getenv("TITLE")) 
    return newtitle;
  return (string)ob->query_name()+" "+ob->query_title();
}

void marriage()
{
  tell_present("Since it seems that there is nothing which hinders this marriage\n" +
	       "I ask you " + par1->query_name() + ". Do you take " +
	       par_title(par2) + " to be your\nlawfully " +
	       "wedded " + husorwife(p2gender) + "?\n");
  tell_object(par1,"The priest whispers to you: A simple yes or no will do.\n");
  match = par1->query_name();
  flag = 2;
}

void marriage2(string par)
{
  tell_present("It seems that " + par->query_name() + " has changed his mind. " +
	       "A marriage is not a thing to be\ntaken lightly so I suggest that " +
	       "You think it over to the next time.\n");
}

void marriage3()
{
  tell_present("And do you " + par2->query_name() + " take " + 
	       par_title(par1) + "\n" +
	       "to be your lawfully wedded " + husorwife(p1gender) + "?\n");
  tell_object(par2,"The priest whispers to you: A simple yes or no will do.\n");
  match = par2->query_name();
  flag = 3;
}

void marriage4()
{
  tell_present("And now for the rings...\n" +
	       initiator + " gives two rings to the priest.\n" +
	       "The priest says: Here you have " + par1->query_name() + " and one for you too " + par2->query_name() + ".\n" +
	       "The priest gives the rings to the couple.\n");
  tell_present("These rings will follow you always as a sign of your bond to\n" +
	       "each other. Thereby I proclaim you husband and wife.\n" +
	       "You may kiss the bride.\n");
  obj = clone_object("obj/wedring");
  transfer(obj,par1);
  obj = clone_object("obj/wedring");
  transfer(obj,par2);
  "/obj/daemon/wedd"->marry(par1->query_real_name(),par2->query_real_name(),initiator);
  obj = clone_object("players/mortis/obj/wedgift");
  transfer(obj,par1);
  obj = clone_object("players/mortis/obj/wedgift");
  transfer(obj,par2);
  tell_object(par1,"Your bridal suite is ready, just type 'suite'.\n");
  tell_object(par2,"Your bridal suite is ready, just type 'suite'.\n");
  flag = 0;
  match = "";
}

void hit_player(int dam)
{
  write("You are disgusted of yourself.\n");
  this_object()->stop_fight();
  this_player()->stop_fight();
  return 0;
}

int id_no(string str) { return str == " no" || str == "no " || str == "no"; }

int id_yes(string str)
{
  return str == " yes" || str == "yes " || str == "yes"
    || str == "I do" || str == "i do";
}

void catch_tell(string str)
{
  string pars1,pars2,temp; 
  if(sscanf(str, "%s says: %s", pars1, pars2) == 2)
  {
    sscanf(pars2, "%s\n", pars2);
    temp = lower_case(pars2);
    if(flag == 1) call_out("nomarriage",1);
    if(flag == 2 && id_no(temp) && match == pars1) call_out("marriage2",1,par1);
    if(flag == 2 && id_yes(temp) && match == pars1) call_out("marriage3",1);
    if(flag == 3 && id_no(temp) && match == pars1) call_out("marriage2",1,par2);
    if(flag == 3 && id_yes(temp) && match == pars1) call_out("marriage4",1);
  }
}


int query_prevent_shadow(object ob)
{
  string tmp;

  if (sscanf(file_name(ob), "local/%s", tmp)==1)
    return 0;
  else
    return 1;
}

string query_spouse(string str)
{
  mixed *q;
  q=(mixed *)"/obj/daemon/wedd"->query_spouse(str);
  return q && q[0];
}
