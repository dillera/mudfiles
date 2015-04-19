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
 *
 *  Many parts of this room was recoded by Profezzorn in January '95.
 */

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

object *quest_objects;
object floating_object;
string *quest_names;
string *quest_shorts;
string *quest_longs;
int *old_quest_values;
int *quest_values;
int nr_quests;
int all_points;
mapping quest_to_qp;

int inorout=1;

void reset(status flag)
{
  if(flag) return;
  set_light(1);
  set_short("Room of quests");
  set_long(
     "DO NOT UPDATE OR DESTRUCT THIS ROOM.\n"+
     "This is the room of quests. Every wizard can build one or more " +
     "quests,\nand for each quest he (or she or it) has built, he " +
     "should put a quest\nobject in this room, when the quest has " +
     "been approved.\nThe quest object must be cloned from " +
     "\"/obj/quest_obj\".\nPlease read the file \"/doc/build/QUESTS\" " +
     "for more information.\nMortal players are not allowed in this room.\n");


  add_exit("south","/room/wiz_hall");

  items =
  ({
     "quest", "Hey, which one?",
     "quests", "On shelf after shelf are quest after quest"
  });
}

void dump_data()
{
  write(sprintf("quest_objects = %O\n",quest_objects));
  write(sprintf("quest_names = %O\n",quest_names));
  write(sprintf("quest_shorts = %O\n",quest_shorts));
  write(sprintf("quest_long = %O\n",quest_longs));
  write(sprintf("quest_values = %O\n",quest_values));
  write(sprintf("old_quest_values = %O\n",old_quest_values));
  write(sprintf("nr_quests = %O\n",nr_quests));
  write(sprintf("quest_to_qp = %O\n",quest_to_qp));
  write(sprintf("all_points = %O\n",all_points));
  write(sprintf("floating_object = %O\n",floating_object));

} /* dump_data */


static void recalc_qp()
{
  int e;
  quest_values=(int *)"/obj/daemon/questd"->get_quest_values(quest_names, old_quest_values);

  /* Beleive me or not, but this sorts the array /Profezzorn */
#define SORT(X) X=m_values(mkmapping(quest_values,X))
  SORT(quest_names);
  SORT(quest_objects);
  SORT(quest_shorts);
  SORT(quest_longs);
  SORT(old_quest_values);
  SORT(quest_values); /* This one MUST be last */

  quest_to_qp=mkmapping(quest_names,quest_values);

  for(all_points=e=0;e<nr_quests;e++)
  {
    string a,b,c,d;

    if(sscanf(quest_shorts[e],"%s (%s points%s",a,c,b)==3)
    {
      while(sscanf(c,"%s (%s",d,c)) a+=" ("+d;
      quest_shorts[e]=a+" ("+quest_values[e]+" points"+b;
    }

    all_points+=quest_values[e];
  }
}

/*
 * This function causes us to recalculate all qp values next time
 * someone wants to know what is going on
 */
void recalc_qp_later()
{
  quest_values=0;
  quest_to_qp=0;
}

static void setup_data()
{
  object *all_the_objects;
  object this_quest;
  string quest_name, wizard_name;
  int this_points;
  int i, n;

  if(this_player())
  {
    string tmp;
    if(this_player()->query_real_name()=="profezzorn")
      write("/room/quest_room: Recalculating quest data...\n");

    if(tmp = catch(log_file("QUESTS", ctime(time()) +
		     ": Re-calculating quest data (for " +
		     (string) this_player()->query_name() + ")\n")))
      write(tmp+"\n");
  }
  all_the_objects=all_inventory(this_object());

  quest_objects = ({});
  quest_names = ({});
  quest_shorts = ({});
  quest_longs = ({});
  old_quest_values = ({});

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
	 if(wizard_name)
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
	quest_longs += ({ (string) this_quest->hint() });
	old_quest_values += ({ (int) this_quest->query_points() });

	++nr_quests;
      }
    } /* if it's a quest object */
  } /* for all the objects in the inventory */

  recalc_qp();
} /* setup_data */

/* This function updates the variables if necessary. */
void check_data()
{
  if(!floating_object)
    floating_object = clone_object("/obj/quest_floater");

  if(first_inventory(this_object()) != floating_object)
  {
    /* Someone has moved an object (probably a quest) into the room */
    setup_data();
    move_object(floating_object, this_object());
    return;
  }

  if(member_array(0, quest_objects) != -1)
  {
    /* One of the quest objects has been destructed */
    setup_data();
    return;
  }

  if(!quest_to_qp)
  {
    recalc_qp();
  }

} /* check_data */

/*---------------------------------------------------------------------------*/

/* This function returns the number of solved but missing quests. */
static int low_check_lost_quests(string *quests, status silently,int use_more)
{
   string *lost_quests;
   int nr_lost;

   lost_quests = quests - quest_names;

   nr_lost = sizeof(lost_quests);
   if(nr_lost > 0 && !silently)
   {
      WRITE("You have also solved " +
	    (nr_lost == 1 ? "a quest" : nr_lost + " quests") + " called "+
	    implode_nicely(lost_quests)+".\n");
   }
   return nr_lost;
}

/* This function returns the number of solved but missing quests. */
int check_lost_quests(status silently,int use_more)
{
   string *quests;

   check_data();
   quests=explode((string) this_player()->query_quests() || "", "#") || ({});

   return low_check_lost_quests(quests, silently, use_more);
}

/* This function returns the number of solved but missing quests. */
int more_check_lost_quests(status silently)
{
  return check_lost_quests(silently,1);
}

/*---------------------------------------------------------------------------*/

/* This function returns the number of quest points the player still
 * needs to be allowed to advance to level 20.
 */
count(status silently,int use_more)
{
  int level;
  object ob;
  int lost_quests;
  int solved_points, my_quest_max, my_needed, more_points;
  int i, j;
  string *solved_quests;
  string *solved_present_quests;

  check_data();
  solved_points = 0;

  solved_quests=explode((string)this_player()->query_quests() || "","#") || ({});

  solved_present_quests=solved_quests & quest_names;
  my_quest_max = (int) this_player()->query_my_quest_max();

  for(i=0;i<sizeof(solved_quests);i++)
    solved_points+=quest_to_qp[solved_quests[i]];

  if(!silently)
  {
    if(sizeof(solved_present_quests) != nr_quests)
    {
      mapping solved_map;
      solved_map=mkmapping(solved_present_quests,solved_present_quests);

      WRITE("Unsolved quests:\n");
      for(j = i = 0; i < nr_quests; ++i)
      {
        if(!solved_map[quest_names[i]])
        {
          /* An unsolved quest */
          j ++;
          WRITE("  " + j + ". " + quest_shorts[i] + "\n");
        }
      }
    }

    WRITE("You have solved " + sizeof(solved_present_quests) + " out of "
	   + nr_quests + " quests,\n");
    WRITE("and you have collected " + solved_points + " out of "
	   + all_points + " quests points.\n");
  }

  lost_quests = low_check_lost_quests(solved_quests, silently, use_more);

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
  if(sizeof(solved_present_quests) == nr_quests && all_points >= 1000)
  {
    if(!silently)
      WRITE("Since you have solved all the quests, you can advance to wizard now.\n");
    return 0;
  }

  /* The number of quest points could decrease: */
  if(all_points < my_quest_max) my_quest_max = all_points;

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
 * needs to be allowed to advance
 */
int more_count(status silently) { return (int)count(silently,1); }

/*---------------------------------------------------------------------------*/

/* This function lists one or, if qnumber is 0, all of the quests that
 * the player already hasn't solved.
 */
void list(int qnumber, int use_more)
{
  int i, show_nr;
  check_data();
  if(qnumber < 0)
  {
    WRITE("Quest number " + qnumber + "? But really!\n");
    return;
  }
  if(qnumber)
  {
    show_nr = qnumber;
    for(i = 0; i < nr_quests; ++i)
    {
      if(!this_player()->query_quests(quest_names[i]))
	show_nr --;

      if(!show_nr)
      {
	WRITE(quest_longs[i] + "\n");
	WRITE("(This quest gives " + quest_values[i] + " points.)\n");
	return;
      }
    }
    WRITE("You don't have that many unsolved quests.\n");
    return;
  }

  show_nr = 0;
  for(i = 0; i < nr_quests; ++i)
  {
    if(!this_player()->query_quests(quest_names[i]))
    {
      if(!show_nr)
	WRITE("Unsolved quests:\n");

      show_nr++;
      WRITE("  " + show_nr + ". " + quest_shorts[i] + "\n");
    }
  } /* for all quest objects */

  if(!show_nr)
    WRITE("You have solved all quests.\n");

} /* list */

void more_list(int qnumber) { list(qnumber,1); }

/*---------------------------------------------------------------------------*/

/* This function lists one or, if qnumber is 0, all of the quests that
 * the player already has solved.
 */
void list_solved(int qnumber, int use_more)
{
  int i, show_nr;
  check_data();

  if(qnumber < 0)
  {
    WRITE("Quest number " + qnumber + "? But really!\n");
    return;
  }

  if(qnumber)
  {
    show_nr = qnumber;
    for(i = 0; i < nr_quests; ++i)
    {
      if(this_player()->query_quests(quest_names[i]))
        show_nr --;

      if(show_nr == 0)
      {
	WRITE(quest_longs[i] + "\n");
	WRITE("(This quest gives " + quest_values[i] + " points.)\n");
	return;
      }
    }
    WRITE("You have not solved that many quests.\n");
    return;
  }

  /* Print solved quests */
  show_nr = 0;
  for(i = 0; i < nr_quests; ++i)
  {
    if(this_player()->query_quests(quest_names[i]))
    {
      /* A solved quest */
      if(!show_nr)
	WRITE("Solved quests:\n");

      show_nr ++;
      WRITE("  " + show_nr + ". " + quest_shorts[i] + "\n");
    }
  } /* for all quest objects */

  if(!show_nr)
    WRITE("You haven't solved any quests.\n");

}

void more_list_solved(int qnumber) { list_solved(qnumber,1); }

/*---------------------------------------------------------------------------*/

/* This function returns the sum of quest points for all the quests,
 * no matter if the player has solved them or not.
 */
int count_all()
{
  check_data();
  return all_points;
}

/* This function returns the sum of quest points
 * for the quests a player has solved.
 * The player object (not the name) of that player
 * must be given as argument to this function.
 */
int count_solved(object player_ob)
{
  string *solved_quests;
  int solved_points, e;

  check_data();
  solved_points = 0;

  solved_quests=explode((string)player_ob->query_quests() || "", "#") || ({});
  for(e=0;e<sizeof(solved_quests);e++)
  {
    solved_points += quest_to_qp[solved_quests[e]] || POINTS_FOR_LOST_QUEST;
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
  return ({
           quest_names + ({}),
           quest_values + ({}),
           quest_shorts + ({}),
           quest_objects + ({}),
           quest_longs + ({}),
         });
}

mixed *query_old_quest_data()
{
  check_data();
  return ({
           quest_names + ({}),
           old_quest_values + ({}),
         });
}

int query_prevent_shadow() { return 1;  } /* profezzorn */
