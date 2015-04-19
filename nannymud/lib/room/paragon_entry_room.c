inherit "/std/room";

#define BASE 1000000
#define COST 300000

/*
   First version by Taren, Sat Apr 15 02:46:18 1995
   Fixed a better description, and some items, by Anna Sat Apr 15 13:53:47 1995
   Fixed some typos, Mon Apr 17 19:40:23 1995
   Changed occurences of 'super' to 'paragon' in filenames, descs, functions,
           /Leowon 951110, 13.45
   Made som flashier 'join' code , the paragon list is now indexed with 1 more
   than their paragon level, reason pl=0, should be pl=1.
   Taren Date: Mon Nov 27 02:56:58 1995	   
   Added .'s to the item descs. Banshee 951213.
   Fixed some typos. Banshee 960103.
   Refreshed the descriptions a bit. /Leowon 960307.
 */

reset(arg) {
  if(arg) return;
  set_light(1);
  set_short("The entry room to the Paragon Chamber");
  set_long("The entry room to the Paragon Chamber.\n"+
           "On the walls beautiful paintings are hanging and\n"+
           "the ceiling is made of marble. There is a door\n"+
	   "going out and a huge door to the east leading in\n"+
	   "to the chamber of the Paragons in NannyMud.\n"+
	   "There is a small plaque on the door.\n");
  add_item(({"painting","paintings"}),
	   "The paintings picture brave heroes fighting dangerous monsters.");
  add_item(({"hero","heroes"}),
	   "It is not anyone you know, but you wish you were as mighty "+
	   "and experienced as they.");
  add_item(({"monster","monsters"}),
	   "You recognize some of the monsters, they were very hard to kill.");
  add_item(({"roof","marble"}),
	   "The roof is made of high-quality marble.");
  add_item(({"wall","walls"}),
	   "The walls are made of grey, smooth stone.");
  add_item("floor",
	   "The floor is made of polished oak planks.");
  add_item("plaque",
	   "You can read the plaque.","read","read");
  
  add_exit("east","/room/paragon_room",0,"check_entry");
  add_exit("out","hill8");
  add_item_cmd("read","plaque","@read()");
  add_property("indoors");
}

init()
{
  add_action("apply","apply");
  ::init();
}


/* I commented the 2nd rule out because I think it confuses rather
   than helps players understand what a paragon 'is'. /Leowon 960307 */
rules()
{
  return
    "* You MAY NOT become a wizard.\n"+
   /* "* Behave with respect and dignity.\n"+ */
    "* Practice common sense.\n"+
    "* Be able to become wizard.\n"+
    "* Pay "+COST+" coins in admission fee.\n";
}

read(string str)
{
  write("The plaque reads:\n"+
	"The responsibility as a Paragon is great. They are the best\n"+
	"players in the MUD, they know every trick in the book.\n"+
	"That is why the Gods have laid down some rules on them.\n\n"+
	rules()+"\n"+
	"The rules are simple and thus the Gods have a simple\n"+
	"punishment for breaking them, abolition.\n"+
	"Think through your decision well before you apply. If you are\n"+
	"greedy, if money matters more to you than honor and fame,\n"+
	"perhaps you don't have the greatness it takes to be a Paragon.\n"+
	"If you wish to join the ranks of the Paragons, type 'apply'.\n");
  return 1;
}

check_entry() 
{
  if(this_player()->query_paragon() ||
     this_player()->query_level() >= 21) return 0;
  write("You are not allowed to enter the chamber of Paragons.\n");
  return 1;
}

apply()
{
  if(this_player()->query_paragon())
    return write("You are alredy a Paragon.\n"),1;
  if(this_player()->query_level()<19)
    return write("Yoy don't have high enough level to become a Paragon.\n"),1;
  if(this_player()->query_level()>19)
    return write("Yoy may not become a Paragon since you are already a wizard.\n"),1;
  if("room/quest_room"->count(1)>0)
    return write("You need to seek more knowledge of the mud.\n"),1;
  if(this_player()->query_exp()<BASE)
    return write("You are not experienced enough to become a Paragon.\n"),1;

  if(this_player()->query_money()<COST)
    return write("You are not rich enough.\n");

  write("When you become Paragon some special rules will apply.\n"+
	"IF you accept these rules you can type 'yes' and become Paragon.\n"+
	rules()+
	"Your answer: ");
  input_to("accept");
  return 1;
}

accept(str)
{
  if(str=="Yes"||str=="yes"||str=="y"||str=="Y")
    {      
      this_player()->set_paragon();
      this_player()->add_money(-COST);
      write("You have become a Paragon, go to the Chamber and advance.\n");
      load_object("/room/paragon_room")->add_to_list(this_player()->query_real_name(),0);
      load_object("/room/paragon_room")->join();
      return 1;
    }
  write("You decide NOT to become a Paragon.\n");
  return 1;
}



