/* Rewritten using /std/room.c, Mats 960222 */

inherit "/std/room";

/* Rewritten using add and set functions in room.c. Banshee 950218 */
/* Cave dragon modified. Banshee 950218 */

inherit "room/room";

mapping hunted=([]);
object dragon;

reset(arg) {
  ::reset(arg);
  if (!arg) {
    set_light(0);
    add_property("cave");
    add_property("no_teleport");

    set_short("Dragon cave");
    set_long("This is the cave where the dragon lives.\n"+
	     "Long way to get here, but ever longer to get out of here.\n");
    add_exit("west","tunnel28",0,"@start_hunt()");
  }
  add_object("@make_dragon()");
}

make_dragon() {
  object treas;
  dragon = clone_object("obj/smartmonster");
  dragon->set_name("dragon");
  dragon->set_alias("cave dragon");
  dragon->set_level(17);
  dragon->set_al(-900);
  dragon->set_short("The cave dragon");
  dragon->set_long("A huge dragon. He is blind but he feels every move\n"+
		     "you make and he will never forget your tasty aroma.\n");
  dragon->set_wc(25);
  dragon->set_ac(10);
  dragon->set_chat_chance(20);
  dragon->set_chats(
    ({"The dragon smells you presence, and he really likes your aroma.\n",
      "The dragon tries to catch you, but you move too quickly.\n",
      "Sniffing like a dog, the dragon moves in your direction.\n",
      "The dragon stumbles. Obviously his eyes are not too good.\n",
      "The dragon lashes out a long tounge and licks your body. Yummy!\n"}));
  dragon->set_dead_ob(this_object());

  treas = clone_object("obj/treasure");
  treas->set_id("sapphire");
  treas->set_alias("stone");
  treas->set_short("A sapphire");
  treas->set_value(250);
  move_object(treas, dragon);

  treas = clone_object("obj/treasure");
  treas->set_id("diamond");
  treas->set_alias("stone");
  treas->set_short("A diamond");
  treas->set_value(250);
  move_object(treas, dragon);

  treas = clone_object("obj/h-ring");
  move_object(treas, dragon);

  treas = clone_object("obj/money");
  treas->set_money(800+random(400));
  move_object(treas,dragon);
  return dragon;
}

monster_died() {    
  write("The dragon roars: FOOL! Do you really think you can kill me?\n");
  return 0;
}

start_hunt() {
  int t;
  if (!present("dragon")) return 0;
  t=3+random(3);
    if (!hunted[this_player()->query_real_name()]) {
      hunted[this_player()->query_real_name()]=t; }
  if (find_call_out("hunt")==-1) call_out("hunt",30);
  return 0;
}

hunt() {
  int i,s,t,damage;
  string pn;
  object obj;

  s=m_sizeof(hunted);
  if (s==0) return;
  for (i=0;i<s;i++) {
    pn=m_indices(hunted)[i];
    t=hunted[pn];
    obj=find_player(pn);
    if (t>1) {
      if (!obj) continue;
      if (dragon)
      tell_object(obj,"\nThe cave dragon hasn't forgotten your tasty aroma.\n"+
		  "He is hunting you. You can feel the ground shaking when \n"+
		  "he comes running after you. This place is not safe!\n"+
		  "Run! Run! Run!\n\n");
      else
	tell_object(obj,"\nThe cave dragon has returned and is now hunting\n"+
		    "you to get his revenge. You can feel the ground shaking\n"+
		    "when he comes running after you. This place is not safe!\n"+
		    "Run! Run! Run!\n");
      hunted[pn]=t-1;
    }
    else {
      tell_object(obj,"\nThe cave dragon has finally found you and with great \n"+
	    "pleasure he takes a big bite of your body. You are badly hurt\n"+
	    "when he returns back to his cave.\n\n");
      damage=obj->query_max_hp()/2;
      obj->reduce_hit_point(damage);
      hunted=m_delete(hunted,pn); 
    }
  }
  call_out("hunt",30); 
}
