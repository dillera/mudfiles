inherit "room/room";

object board;

init() {
    ::init();
    if (call_other(this_player(), "query_level", 0) < 20) {
        write("Hey! You! Puny mortal! " +
	      "What are you trying to do in here? Get out!\n");
	call_other(this_player(),"move_player", "X#room/adv_guild");
	return;
    }
}

reset(arg) {
  if(arg) return;
  dest_dir = ({"room/adv_inner", "up"});
  short_desc = "The Election Room";
  long_desc = "This is the Election Room.\n"+
              "Here wizard's can write things that concern the Election\n"+
	      "for levels beyond wizard.\n";
  set_light(1);
  board = clone_object("obj/bboard");
  board->set_name("bboard/election");
  move_object(board, this_object());
}
int inorout=1;

query_prevent_follow(ob)
{
  return (!ob || ob->query_level() < 20);
}
