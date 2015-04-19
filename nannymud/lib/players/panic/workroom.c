
/* Workroom */

inherit "/std/room";

reset(arg) {
      if (arg) return;

      set_light(1);
      set_short("Lair of Death");
      set_long(
      "You have stumbled into the lair of a mighty dragon. It seems\n"+
      "that death is waiting outside just one of these exits.\n"+
      "Perhaps you should just quit instead?\n");

      add_property("indoors");
      add_property("magical");
      add_property("no_teleport");

      add_exit("church","/room/church");
      add_exit("wizroom","/room/adv_inner");
      add_exit("advent","/room/adv_guild");

}
