/**********************************************************************
 *	
 *   mw.c -- A monster walking around.                    Ver 03.00.00
 *
 *   This is just an inherited obj/monster with additional code
 *   that enables the wizard make it stroll around.
 *   Additional functions to call are:
 *
 *	set_time_slice(int time) The (call_out)-time between each
 *				time the monster is to walk.
 *	
 *	set_walking()		Makes the monster want to walk.
 *
 *   Optional routines
 *	
 *	set_cmds(mapping m)     Sets the where_cmd-mapping making the movement
 *				of the monster controlable. The m para-
 *				meter is an array of a roomname-string and
 *				a command string to be executed by the monster
 *				at that location. The argument in the mapping
 *                              can also be an array of this format:
 *                               ({ ob , fun }) , in which casee fun in ob
 *                              will be called upon entering that room.
 *
 *	set_directions(string *dir)
 *                           	If you want the monster to select
 *				particular directions. Default is
 *				{ "n","s","u","d","w","e","se","sw","ne","nw" }
 *
 *	set_decission_obj(object obj)
 *                             	If this is set, the decission of
 *				what to do is left to the routine
 *					do_move(room)
 *				in the object obj. room is the current room.
 *				Note that the default random_move() is then
 *				NOT called.
 *
 *	LOG-FILES that might show up:
 * 
 *	/log/<wizard-name>.walk.log  :: Aggressive monster out of area.
 *	/log/<wizard-name>.debug     :: Monster had no environment.
 *
 *	Originally coded by Sir Rogon
 *      Optimized and cleaned by Profezzorn
 *	
 ***********************************************************************/

inherit "obj/smartmonster";

#define DEFAULT_TIME 200

int walking,last_seen;
int time_slice;
int logging;
string rem_room;
mapping where_cmd;
mixed decission_obj;

string *directions=(string *)"/obj/walkingmonster"->get_directions();
string *get_directions()
{
  if(directions) return directions;
  return ({ "north","south","west","east","up","down",
	    "northeast", "northwest", "southeast", "southwest" });
}

void set_directions(string *dir) { directions = dir; }
void set_decission_obj(mixed obj) { decission_obj = obj; }
void set_logging(int n) { logging = n; } 
void set_time_slice(int t) { time_slice = t; }
void set_cmds(mapping com) { where_cmd = com; }
                 
int query_time_slice()
{
  if(time_slice) return time_slice; else return DEFAULT_TIME; 
}
int query_logging() { return logging; }
int query_walking() { return walking; }

void set_walking()
{
  walking=1;
  call_out("walk",query_time_slice());
}

void stop_walking()
{
  walking = 0;
  remove_call_out("walk");
}

void random_move() { command(directions[random(sizeof(directions))]); }

void init()
{
  if(this_player() && interactive(this_player())) last_seen=time();
  ::init();
  if(walking && -1==find_call_out("walk"))
    call_out("walk",query_time_slice());
}

void stroll_ahead()
{
  object att,this,env;
  mixed foo;
  string wizard,where,whose;

  this   = this_object();
  wizard = creator(this);
  env    = environment();
  if(!env) return;           /* If it has no environment, let it stay! */
  where = file_name(env);
  whose = creator(env);
  enable_commands();

  if(whose!=wizard && aggressive)
  {
    if(rem_room)
    {
      say(query_name()+" finds "+
	  query_objective()+"self in a hostile environment and decides\n"+
	  "to flee as fast he can!\n");
      this_player()->move_player("in a hurry#"+rem_room);
      if(logging)
	log_file(wizard+".walk.log",
		 file_name(this) + " fleed back from " +
		 file_name(env) + " !!\n");
      rem_room = "";
    } else {
      stop_fight();
      set_aggressive(0);
      if(logging)
	log_file(wizard+".walk.log",
		 "WARNING! Aggressive monster out of area!\n "+file_name(env)+"\n"+
		 file_name(this)+" had to be patched nonaggressive.\n");
    }
  }

  if(where_cmd && (foo==where_cmd[where]))
  {
    if(stringp(foo))
    {
      command(foo);
    }else if(pointerp(foo)){
      call_other(foo[0],foo[1],where);
    }
  }else{
    if(!((att=(object)query_attack()) && present(att,environment())))
    {
      if(decission_obj)
	call_other(decission_obj,"do_move",where);
      else
	random_move();
    }
  }
    
  if(where != rem_room)
  {
    rem_room = where;
    if(logging)
      log_file(wizard+".walk.log", file_name(this) + ":" + where + "\n");
  }
}

void walk()
{ 
  if(!walking) return;
  stroll_ahead();
  remove_call_out("walk"); /* prevent double call outs */
  if(time()-last_seen<900)
    call_out("walk", query_time_slice()*2/3+random(query_time_slice()/3));
} 

