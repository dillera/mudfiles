/*  This is the room where the quest objects are stored.
 *  -- Modified for quest points by Padrone, Jan 5, 1992.
 *  Second latest change of this file: Padrone, Oct 7, 1992
 *  Latest change of this file: Gwendolyn, Feb 10, 1994
 *
 *  NOT! Who is the son of a silly person who edited this
 *  file without changing the "latest change" comment above?
 *  Hrmpf! I don't want to answer questions about YOUR bugs,
 *  I have more than enough trouble with my own, thank you very much!
 *		/Padrone, April 15, 1993
 */

#pragma strict_types
#pragma save_types

inherit "/room/room";

#include "tune.h"

#define POINTS_FOR_LOST_QUEST   20
#define MORE(X) this_player()->simplemore(X)
#define WRITE(X) do_write(X,use_more)

void do_write(string s,int use_more)
{
  if(use_more)
    MORE(s);
  else
    write(s);
}

/*  To sort the quests according to quest points, and also for efficicency,
 *  we read the data from the quest objects and store them in variables in
 *  this object.
 *  The only problem is to check that these variables are always updated,
 *  so for this we put an object, a "floater", "on top" of the others in the
 *  room. If someone moves an object into the room, the floater will not be
 *  first in the inventory any more.
 */

object *quest_objects, floating_object;
string *quest_names, *quest_shorts;
int *quest_values, nr_quests, all_points;

int inorout=1;

void reset(status flag)
{
   if(flag)
      return;
   set_light(1);
   short_desc = "Room of quests";
   long_desc =
     "DO NOT UPDATE OR DESTRUCT THIS ROOM.\n"+
      "This is the room of quests. Every wizard can build one or more " +
      "quests,\nand for each quest he (or she or it) has built, he " +
      "should put a quest\nobject in this room, when the quest has " +
      "been approved.\nThe quest object must be cloned from " +
      "\"/obj/quest_obj\".\nPlease read the file \"/doc/build/QUESTS\" " +
      "for more information.\nMortal players are not allowed in this room.\n";

   dest_dir =
   ({
      "/room/wiz_hall", "south"
    });
   items =
   ({
        "quest", "Hey, which one?",
	"quests", "On shelf after shelf are quest after quest"
    });
}

/* Maybe we should put this function in 'obj/simul_efun.c' or somewhere? */
/* Hmm, smartwrite... Reminds me of smartmonster...  /Gwendolyn */
void smartwrite(mixed something)
{
   int i, n;
   mixed this_element;
   string temp, temp2;
   if(stringp(something))
   {
      if(sscanf(something, "%s\n%s", temp, temp2) == 2)
      {
	 if(temp2 != "")
	    write("'" + temp + "\\n'...");
	 else
	    write("'" + temp + "\\n'");
      }
      else
	 write("'" + something + "'");
   }
   else if(objectp(something))
   {
      if(temp = (string) something->query_name())
	 write(temp);
      else
	 write(something);	/* write(file_name(something)); */
   }
   else if(pointerp(something))
   {
      n = sizeof(something);
      write("[");
      for(i = 0; i < n; ++i)
      {
	 if(i != 0)
	    write(", ");
	 this_element = something[i];
	 smartwrite(this_element);
      }
      write("]");
   }
   else
      write(something);
} /* smartwrite */

void dump_data()
{
   write("quest_objects = "); smartwrite(quest_objects); write("\n");
   write("quest_names = "); smartwrite(quest_names); write("\n");
   write("quest_shorts = "); smartwrite(quest_shorts); write("\n");
   write("quest_values = "); smartwrite(quest_values); write("\n");
   write("nr_quests = "); smartwrite(nr_quests); write("\n");
   write("all_points = "); smartwrite(all_points); write("\n");
   write("floating_object = "); smartwrite(floating_object); write("\n");
} /* dump_data */

status compare_points(object ob1, object ob2)
{
   return ob1->query_points() > ob2->query_points();
}

int get_points(object o)
{
  return (int)o->query_points();
}

void setup_data()
{
   object *all_the_objects;
   object this_quest;
   string quest_name, wizard_name;
   int this_points;
   int i, n;
   if(this_player())
   {
      write("/room/quest_room: Recalculating quest data...\n");
      log_file("QUESTS", ctime(time()) +
	       ": Re-calculating quest data (for " +
	       (string) this_player()->query_name() + ")\n");
   }
#if 0
   all_the_objects = sort_array(all_inventory(this_object()),
				"compare_points", this_object());
#else
   /* Mappingsort is faster /Profezzorn */
   all_the_objects=all_inventory(this_object());
   all_the_objects=m_values(mkmapping(map_array(all_the_objects,"get_points",this_object()),all_the_objects));
#endif
   quest_objects = ({});
   quest_names = ({});
   quest_shorts = ({});
   quest_values = ({});
   nr_quests = 0;
   all_points = 0;
   n = sizeof(all_the_objects);
   for(i = 0; i < n; ++i)
   {
      this_quest = all_the_objects[i];
      if(this_quest->id("quest"))
      {
	 quest_name = (string) this_quest->query_name();
	 if(member_array(quest_name, quest_names) != -1)
	 {
	    /* A duplicate. This one is later in the inventory, so it's older. Zap! */
	    wizard_name = creator(this_quest);
	    if(wizard_name != 0)
	    {
	       log_file(wizard_name, ctime(time()) +
			": Duplicate quest object removed from /room/quest_room: '" +
			quest_name + "'\n");
	    }
	    log_file("QUESTS", ctime(time()) +
		     ": Duplicate quest object removed from /room/quest_room: '" +
		     quest_name + "'\n");
	    destruct(this_quest);
	 }
	 else
	 {
	    quest_objects += ({ this_quest });
	    quest_names += ({ quest_name });
	    quest_shorts += ({ (string) this_quest->short() });
	    this_points = (int) this_quest->query_points();
	    quest_values += ({ this_points });
	    all_points += this_points;
	    ++nr_quests;
	 }
      } /* if it's a quest object */
   } /* for all the objects in the inventory */
} /* setup_data */

/* This function updates the variables if necessary. */
void check_data()
{
   if(!floating_object)
   {
      floating_object = clone_object("/obj/quest_floater");
      move_object(floating_object, this_object());
      setup_data();
   }
   else if(first_inventory(this_object()) != floating_object)
   {
      /* Someone has moved an object (probably a quest) into the room */
      move_object(floating_object, this_object());
      setup_data();
   }
   else if(member_array(0, quest_objects) != -1)
   {
      /* One of the quest objects has been destructed */
      setup_data();
   }
} /* check_data */

/*---------------------------------------------------------------------------*/

/* This function returns the number of solved but missing quests. */
int check_lost_quests(status silently,int use_more)
{
   string all_quests, this_quest, *lost_quests;
   int i, nr_lost;
   all_quests = (string) this_player()->query_quests(0);
   lost_quests = ({});
/*   write("You are "+this_player()->query_name()+" i hope...\n");*/
   while(all_quests)
   {
      if(sscanf(all_quests, "%s#%s", this_quest, all_quests) != 2)
      {
	 this_quest = all_quests;
	 all_quests = 0;
      }
/*      write("You have solved "+this_quest+".\n");*/
      if(member_array(this_quest, quest_names) == -1)
	 lost_quests += ({ this_quest });
   }
   nr_lost = sizeof(lost_quests);
   if(nr_lost > 0 && !silently)
   {
      WRITE("You have also solved " +
	    (nr_lost == 1 ? "a quest" : nr_lost + " quests") + " called ");
      for(i = 0; i < nr_lost; ++i)
      {
	 WRITE("'" + lost_quests[i] + "'");
	 if(i < nr_lost - 2)
	    WRITE(", ");
	 else if(i == nr_lost - 2)
	    WRITE(" and ");
      }
      WRITE(".\n");
   }
   return nr_lost;
} /* check_lost_quests */

/* This function returns the number of solved but missing quests. */
int more_check_lost_quests(status silently)
{
  return check_lost_quests(silently,1);
} /* more_check_lost_quests */

/*---------------------------------------------------------------------------*/

/* This function returns the number of quest points the player still
 * needs to be allowed to advance to level 20.
 */
int count(status silently,int use_more)
{
  int level;
   object ob;
   int solved_quests, lost_quests;
   int solved_points, my_quest_max, my_needed, more_points;
   string quest_name;
   int i, j;

   check_data();
   solved_quests = solved_points =0;
  /* First, count quests and points, and (if not silent) print unsolved quests */
   for(j = i = 0; i < nr_quests; ++i)
   {
     if(this_player()->query_quests(quest_names[i]))
      {
	/* A solved quest */
	 solved_quests += 1;
	 solved_points += quest_values[i];
      }else if(!silently)
      {
	 /* An unsolved quest */
	 if(!j) WRITE("Unsolved quests:\n");
	 j ++;
	 WRITE("  " + j + ". " + quest_shorts[i] + "\n");
      }
   } /* for all quests */

   /* Now, if not silent, print some info */
   if(!silently)
   {
      WRITE("You have solved " + solved_quests + " out of "
	    + nr_quests + " quests,\n");
      WRITE("and you have collected " + solved_points + " out of "
	    + all_points + " quests points.\n");
   }
   my_quest_max = (int) this_player()->query_my_quest_max();
   lost_quests = check_lost_quests(silently,use_more);

   if(lost_quests > 0)
   {
      solved_points += lost_quests * POINTS_FOR_LOST_QUEST;
      if(!silently)
      {
	 WRITE((lost_quests == 1 ? "This quest is" : "These quests are") +
	       " no longer available, but you will get\n");
	 WRITE(POINTS_FOR_LOST_QUEST + " points " +
	       (lost_quests == 1 ? "for it" : "each for them") +
	       " anyway, giving you a total of " + solved_points + ".\n");
      }
   }
   /* If a player has solved ALL the quests, he (she, it) can become a */
   /* wizard! This works only if the quest point max is at least 1000. */
   if(solved_quests == nr_quests && all_points >= 1000)
   {
     if(!silently)
       WRITE("Since you have solved all the quests, you can advance to wizard now.\n");
     return 0;
   }

  /* The number of quest points could decrease: */
   if(all_points < my_quest_max)
     my_quest_max = all_points;
   my_needed = QUEST_POINTS_PERCENT_TO_WIZ * my_quest_max / 100;

   /* The rest of the report */
   if(!silently)
   {

     WRITE("The requirement for you to become wizard is " + my_needed + " points.\n");
     WRITE("If you solve ALL quests, you can advance to wizard even\nwithout the usually required experience.\n");
   }

  level=(int)this_player()->query_level();
  if(level<4)
  {
    my_needed=0;
  }else{
    for(;level<19;level++) my_needed=my_needed*2/3;
  }
  more_points = my_needed - solved_points;
  if(more_points < 0) more_points = 0;
  if(all_points >= 1000)
  {
    if(!silently)
    {
      if(more_points == 0)
	WRITE("You don't need to solve any more quests to advance to next level.\n");
       else
	 WRITE("You need " + more_points + " more quests point"+
	       ( (more_points>1) ? "s":"")+" to advance to next level.\n");
    }
  }else{
    WRITE("The questsystem seem to be out of order, you cannot advance.\n");
    more_points=123456789;
  }
  if(!silently)
    WRITE("For more information, go to the tourist information office.\n");
   return more_points;
} /* count */

/* This function returns the number of quest points the player still
 * needs to be allowed to advance to level 20.
 */
int more_count(status silently)
{
  return count(silently,1);
} /* more_count */

/*---------------------------------------------------------------------------*/

/* This function lists one or, if qnumber is 0, all of the quests that
 * the player already hasn't solved.
 */
void list(int qnumber)
{
   int i, show_nr;
   check_data();
   if(qnumber < 0)
   {
      write("Quest number " + qnumber + "? But really!\n");
      return;
   }
   if(qnumber != 0)
   {
      show_nr = qnumber;
      for(i = 0; i < nr_quests; ++i)
      {
	 if(!this_player()->query_quests(quest_names[i]))
	    show_nr -= 1;
	 if(show_nr == 0)
	 {
	    write(quest_objects[i]->hint() + "\n");
	    write("(This quest gives " + quest_values[i] + " points.)\n");
	    return;
	 }
      }
      write("You don't have that many unsolved quests.\n");
      return;
   }
   show_nr = 0;
   for(i = 0; i < nr_quests; ++i)
   {
      if(!this_player()->query_quests(quest_names[i]))
      {
	 if(show_nr == 0)
	    write("Unsolved quests:\n");
	 show_nr++;
	 write("  " + show_nr + ". " + quest_shorts[i] + "\n");
      }
   } /* for all quest objects */
} /* list */

/* This function lists one or, if qnumber is 0, all of the quests that
 * the player already hasn't solved.
 */
void more_list(int qnumber)
{
   int i, show_nr;
   check_data();
   if(qnumber < 0)
   {
      write("Quest number " + qnumber + "? But really!\n");
      return;
   }
   if(qnumber != 0)
   {
      show_nr = qnumber;
      for(i = 0; i < nr_quests; ++i)
      {
	 if(!this_player()->query_quests(quest_names[i]))
	    show_nr -= 1;
	 if(show_nr == 0)
	 {
	    MORE(quest_objects[i]->hint() + "\n");
	    MORE("(This quest gives " + quest_values[i] + " points.)\n");
	    return;
	 }
      }
      MORE("You don't have that many unsolved quests.\n");
      return;
   }
   show_nr = 0;
   for(i = 0; i < nr_quests; ++i)
   {
      if(!this_player()->query_quests(quest_names[i]))
      {
	 if(show_nr == 0)
	    write("Unsolved quests:\n");
	 show_nr++;
	 write("  " + show_nr + ". " + quest_shorts[i] + "\n");
      }
   } /* for all quest objects */
} /* more_list */

/*---------------------------------------------------------------------------*/

/* This function lists one or, if qnumber is 0, all of the quests that
 * the player already has solved.
 */
void list_solved(int qnumber)
{
   int i, show_nr;
   check_data();
   if(qnumber < 0)
   {
      write("Quest number " + qnumber + "? But really!\n");
      return;
   }
   if(qnumber != 0)
   {
      show_nr = qnumber;
      for(i = 0; i < nr_quests; ++i)
      {
	 if(this_player()->query_quests(quest_names[i]))
	    show_nr -= 1;
	 if(show_nr == 0)
	 {
	    write(quest_objects[i]->hint() + "\n");
	    write("(This quest gives " + quest_values[i] + " points.)\n");
	    return;
	 }
      }
      write("You have not solved that many quests.\n");
      return;
   }
   /* Print solved quests */
   show_nr = 0;
   for(i = 0; i < nr_quests; ++i)
   {
      if(this_player()->query_quests(quest_names[i]))
      {
	 /* A solved quest */
	 if(show_nr == 0)
	    write("Solved quests:\n");
	 show_nr ++;
	 write("  " + show_nr + ". " + quest_shorts[i] + "\n");
      }
   } /* for all quest objects */
} /* list_solved */

/* This function lists one or, if qnumber is 0, all of the quests that
 * the player already has solved.
 */
void more_list_solved(int qnumber)
{
   int i, show_nr;
   check_data();
   if(qnumber < 0)
   {
      MORE("Quest number " + qnumber + "? But really!\n");
      return;
   }
   if(qnumber != 0)
   {
      show_nr = qnumber;
      for(i = 0; i < nr_quests; ++i)
      {
	 if(this_player()->query_quests(quest_names[i]))
	    show_nr -= 1;
	 if(show_nr == 0)
	 {
	    MORE(quest_objects[i]->hint() + "\n");
	    MORE("(This quest gives " + quest_values[i] + " points.)\n");
	    return;
	 }
      }
      MORE("You have not solved that many quests.\n");
      return;
   }
   /* Print solved quests */
   show_nr = 0;
   for(i = 0; i < nr_quests; ++i)
   {
      if(this_player()->query_quests(quest_names[i]))
      {
	 /* A solved quest */
	 if(show_nr == 0)
	    MORE("Solved quests:\n");
	 show_nr += 1;
	 MORE("  " + show_nr + ". " + quest_shorts[i] + "\n");
      }
   } /* for all quest objects */
} /* more_list_solved */

/*---------------------------------------------------------------------------*/

/* This function returns the sum of quest points for all the quests,
 * no matter if the player has solved them or not.
 */
int count_all()
{
   check_data();
   return all_points;
} /* count_all */

/* This function returns the sum of quest points
 * for the quests a player has solved.
 * The player object (not the name) of that player
 * must be given as argument to this function.
 */
int count_solved(object player_ob)
{
   string queststr, *all;
   int solved_points, i, j;
   check_data();
   solved_points = 0;
   if((queststr = (string) player_ob->query_quests(0)) &&
      (all = explode(queststr, "#")))
   {
      for(i = 0; i < sizeof(all); i++)
      {
	 if((j = member_array(all[i], quest_names)) != -1)
	    solved_points += quest_values[j];
	 else
	    solved_points += POINTS_FOR_LOST_QUEST;
      }
   }
   return solved_points;
} /* count_solved */

/*---------------------------------------------------------------------------*/

void _exit()
{
   if((int) this_player(1)->query_level_sec() < 22)
      throw("Ne ne.\n");
}

mixed *query_quest_data()
{
   check_data();
   return ({ quest_names + ({}), quest_values + ({}), quest_shorts + ({}) });
}

