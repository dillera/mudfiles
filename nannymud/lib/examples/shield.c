inherit "/std/simple_armour";

reset(arg) {
  if (arg) return;
  set_name("shield");
  set_value(100);
  set_weight(3);
  set_class(3);
  set_type("shield"); 
  set_short("A shield");
  set_long("The shield is made of steel and has a brightly shining\n"+
	   "sun on it.\n");
  add_item("sun","The sun on the shield is shining very brightly.\n");
  set_light(1);
  add_property("steel"); /* The shield is made of steel. */
} 






