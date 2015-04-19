/* Rewritten to inherit /room/room and cleaned up a little 940329 Anna */
/* Rewritten using set and add functions from room.c. Banshee 950215 */
/* Rewritten to /std/room.c Rohan 950922 */

inherit "/std/room";

int lamp_is_lit, reboot_time;

reset(arg) {
  if (!present("lyspaper"))
    move_object(clone_object("obj/lyspaper"), this_object());
  
  if (arg) return;

  set_light(1);
  add_property("no_fight");
  add_property("no_magic");
  add_property("holy_ground");
  reboot_time = time();

  move_object(clone_object("local/priest"), this_object()); 

  add_property("indoors");
  add_exit("west","/room/elevator",0,"go_west");
  add_exit("south","/room/vill_green");
  add_exit("north","/room/dark_place");
  add_exit("down","/room/crypt");
  
  set_short("Village church");
  set_long("You are in the local village church.\n"+
	   "There is a huge pit in the centre and a door in the west wall with\n"+
	   "a button beside it. There is also a clock on the wall.\n"+
	   "This church has the service of reviving ghosts. Dead people come\n"+
	   "to the church and pray. This is a holy place and you feel safe here.\n"+
	   "There is an exit south and a back door leads north.\n"+
           "There are some dark stairs going down here.\n");
  add_item("pit",
	   "In the middle of the church is a deep pit.\n"+
	   "It was used for sacrifice in the old times, but nowadays\n" +
	   "it is only left for tourists to look at.\n");
  add_item("back door",
    "It's a small door leading out to a place behind the church.\n");
} 

init(arg) {
  ::init(arg);
  add_action("open", "open");
  add_action("hmm","exa");
  add_action("hmm","look");
  add_action("push", "push");
  add_action("push", "press");
  add_action("close", "close");

  /* It doesn't work, so why try? /Profezzorn 
     It works just fine...        /Slater      */
#if 1
  add_action("no_auto", "auto");
  add_action("no_auto", "dotimes");
#endif
  /* Just to stop people using auto here... /Slater */
  if(!present("soulgem",this_player())) {
    add_action("pray", "pray");
    add_action("pray", "regenerate"); 
  } /* Damneds guild special. Cel. */
} /* init */

#if 1
no_auto() { return write("Sorry, that command has been disabled here.\n"),1; }
#endif

hmm(str) {
  string a,b;
  if (!str) return;
  if (sscanf(str,"%sclock%s",a,b) == 2) {
    int i, j;
    write("The clock shows ");
    i = time() - reboot_time;
    j = i / 60 / 60 / 24;
    if (j == 1) write("1 day "); 
    else 
      if (j > 0) write(j + " days ");
    i -= j * 60 * 60 * 24;
    j = i / 60 / 60;
    if (j == 1) write("1 hour ");
    else 
      if (j > 0) write(j + " hours ");
    i -= j * 60 * 60;
    j = i / 60;
    if (j == 1) write("1 minute ");
    else 
      if (j > 0) write(j + " minutes ");
    i -= j * 60;
    if (i == 1) write("1 second");
    else 
      if (i > 0) write(i + " seconds");
    write("\n");
    return 1; 
  } /* clock */
  if (str=="door") {
    if ("room/elevator"->query_door_open() && 
        "room/elevator"->query_level()==2)
      write("The door is open.\n");
    else write("The door is closed.\n");
    if (lamp_is_lit)
      write("The lamp beside the elevator is lit.\n");
    else 
      write("There is a button beside the door.\n"); 
    return 1; 
  }
  if (str=="lamp") {
    if (lamp_is_lit)
      write("The lamp beside the elevator is lit.\n");
    else 
      write("The lamp beside the elevator is not lit.\n"); 
    return 1; 
  }
}

go_west() {
  if (("room/elevator"->query_door_open()==0) ||
      ("room/elevator"->query_level(0)!=2)) {
    write("The door is closed.\n");
    return 1; }
} /* go_west */

open(str) {
  if (str!="door") return 0;
  if ("room/elevator"->query_level(0) != 2) {
    write("You can't when the elevator isn't here.\n");
    return 1; 
  }
  "room/elevator"->open_door("door");
  return 1;
}

close(str) {
  if (str!="door") return 0;
  "room/elevator"->close_door("door");
  return 1;
}

push(str) {
  if (str && str!="button")
    return 0;
  if ("room/elevator"->call_elevator(2))
    lamp_is_lit = 1;
  return 1;
}

elevator_arrives() {
  say("The lamp on the button beside the elevator goes out.\n");
  lamp_is_lit = 0;
}

pray() { return this_player()->remove_ghost(0); }

query_reboot_time() { return reboot_time; }
query_prevent_shadow() { return 1; }




