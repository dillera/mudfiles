inherit "/std/simple_weapon";

  /* This is a unique and very special sword. 
     This kind of sword needs approval. */

reset(arg) {
  if (arg) return;

  set_name("archsword");
  add_alias("sword");
  set_value(5000);
  set_weight(6);
  set_class(18);
  set_short("Leo's archsword");
  set_long("This is the sword that the archwizard Leo made when he became\n"+ 
	   "archwizard. It is a heavy twohanded sword. There are some runes\n"+
	   "on the blade and some decorations on the hilt.\n");
  set_hit_line_object(this_object()); /* This sword has special hit messeges. */

  /* Things can also have items. */
  add_item("blade",
	   "It is a very sharp blade and it is decorated with runes.");
  add_item("hilt",
	   "The hilt is decorated with valuable gems.");

  /* Let's add some properties. */
  add_property("two_handed"); /* This makes it a two-handed weapon. */
  add_property("steel"); /* It is made of steel. */
  add_property("no_multi_attacks"); /* As the weapon has special attacks we 
                                      only want it to come once per round. */
  add_property("magical"); /* This is a magical wapon. */
  add_property("unique_item"); /* This is a unique weapon. */

  /* We want some special functions to be called:
     remote_prevent_wield
     remote_do_wield
     remote_do_unwield
     remote_weapon_hit   */
  add_property("__remote",file_name(this_object())); 
} /* reset */


remote_prevent_wield(weapon) {
  if (this_player()->query_int()<20) {
    write("You don't seem to understand how to use this beautiful weapon.\n");
    return 1;
  }
  return 0;
} /* remote_prevent_wield */

remote_do_wield(weapon) {
  write("You feel much more powerful when wielding the archsword.\n");
} /* remote_do_wield */

remote_do_unwield(weapon) {
  write("You feel much less powerful without the archsword.\n");
} /* remote_do_unwield */

remote_weapon_hit(attacker) {
  /* This function returns extra damage that is added to the normal damage. */
  switch(random(100)) {
  case 0..80: return 0; /* Nothing special in most cases */
  case 81..85: return "miss"; /* This makes this hit a miss. */
  case 86..92: 
    write("You make a perfect swing with the archsword.\n");
    return 5;
  case 93..97:
    write("You feel the presence of Leo the archwizard's spirit in the sword.\n");
    return 10;
  case 98..99:
    write("The runes on in the blades glows brightly.\n");
    return 30;
  }
} /* remote_weapon_hit */



get_hit_lines(dam,target,hitter,type) {
  string tarn,tarpro,tarobj,hitn;
  if(!target) return ({"","",""});
  tarn=target->query_name();
  tarpro=target->query_pronoun();
  tarobj=target->query_objective();
  hitn=hitter->query_name();
  if(!dam) {
    return ({ 
      hitn+" aims for a brutal blow, missing you with an inch.\n",
      "You aim for a brutal blow at "+tarn+" but miss "+tarobj+" with an inch.\n",
      hitn+" aims a brutal blow at "+tarn+" but miss "+tarobj+" with an inch.\n" 
    }); }
  if(dam<5) {
  return ({
    hitn+" grazes you slightly.\n",
    "You graze "+tarn+" slightly.\n",
    hitn+" grazes "+tarn+" slightly.\n"
  }); }
  if(dam<10) {
  return ({
    hitn+" scratches you so you start to bleed.\n",
    "You scratch "+tarn+" so "+tarpro+" starts to bleed.\n",
    hitn+" scratches "+tarn+" so "+tarpro+" starts to bleed.\n"
  }); }
  if(dam<20) {
  return ({
    hitn+"'s sword cuts deeply into your flesh.\n",
    "The archsword cuts deeply into "+tarn+"'s flesh.\n",
  hitn+"'s sword cuts deeply into "+tarn+ "'s flesh.\n"
  }); }
  if(dam>19) {
  return ({
    hitn+" breaks your ribs with a devastating blow.\n",
    "You break "+tarn+"s ribs with a devastating blow.\n",
    hitn+" breaks "+tarn+"s ribs with a devastating blow.\n"
  }); }
  return 0;
} /* hit_lines */
