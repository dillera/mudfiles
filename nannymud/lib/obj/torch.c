/*
 * This is a generic torch.
 * It will have some good initialisations by default.
 * The torch can't be sold when it is lit.
 */

inherit "/obj/property";

string amount_of_fuel;
string name;
status is_lighted;
status is_wet;
int wet_time;
int weight;

long() {
  if (is_lighted)
    write("The " + name + " is lit.\n");
  else if (is_wet)
    write("The " + name + " is wet.\n");
  else
    write("It's just a normal " + name + ".\n");
}

reset(arg) {
    if (arg)
	return;
    amount_of_fuel = 2000; name = "torch"; is_lighted = 0; weight = 1;
    is_wet = 0; wet_time = 200;
}

query_wet() { return is_wet; }

prevent_insert()
{
  return is_lighted;
}

set_wet(time)
{
  if (is_lighted)
    extinguish(0);
  
  if (!time)
    time = wet_time;
  is_wet = 1;
  call_out("reset_wet", time*3, 0);
  return 1;
}


reset_wet()
{
  is_wet = 0;
}

set_weight(w) { weight = w; }

query_weight() { return weight; }

query_name() { return name; }

short() {
    if (is_lighted)
	return "a lit " + name;
    if (is_wet)
        return "a wet " + name;
    return "a " + name;
}

set_name(n) { name = n; }
set_fuel(f) { amount_of_fuel = f; wet_time = f/10; }

init() {
    add_action("light", "light");
    add_action("extinguish", "extinguish");
}

light(str) {
    if (!str || str != name)
	return 0;
    if (is_wet) {
      write("The " + name + " is too wet to be lit.\n");
      say(capitalize(this_player()->query_name()) +
	  " foolishly tries to light a wet " + name + ".\n");
      return 1;
    }
    if (is_lighted) {
	write("It is already lit.\n");
	return 1;
    }
    is_lighted = 1;
    call_out("out_of_fuel", amount_of_fuel * 2);
    if (set_light(1) == 1) {
	write("You can see again.\n");
	say(capitalize(this_player()->query_name()) +
	    "lights a " + name + "\n");
    } else
	write("Ok.\n");
    amount_of_fuel = 0;
    return 1;
}

out_of_fuel()
{
  object ob;
  ob=environment();
  if(living(ob) && environment(ob)) ob=environment(ob);
  /* changed from say to tell_room by profezzorn */
  if (set_light(-1) == 0)
    tell_room(ob,"There is darkness as a " + name + " goes dark.\n");
  else
    tell_room(ob,"The " + name + " goes dark.\n");
  destruct(this_object());
}

id(str) {
    return str=="torchobj" || str == name || (is_wet && str == "wet " + name);
}

query_value() {
  return amount_of_fuel/100;
}

query_lighted()  /* Added by Ppannion 940209 */
{
  return is_lighted;
}

get() { return 1; }

extinguish(str)
{
  int i;
  
  if (str && !id(str))
    return 0;
  if (!is_lighted)
    return 0;
  i = remove_call_out("out_of_fuel");
  if (i == -1) {
    write("Error.\n");
    return 1;
  }
  amount_of_fuel = i/2;
  is_lighted = 0;
  i = set_light(-1);
  if (query_verb() == "extinguish") {
    if (i == 0)
      {
	write("It turns dark.\n");
	say(call_other(this_player(), "query_name") +
	    " extinguishes the only light source.\n");
      }
    else
      {
	write("Ok.\n");
	say(this_player() -> query_name() + " extinguishes the " +
	    query_name() + ".\n");
      }
  }
  else
    {
      write("Your " + query_name() + " is extinguished.\n");
      say(this_player() -> query_name() +
	  "s " + query_name() + " is extinguished.\n");
      if (i==0) tell_room(environment(this_player()),
			  "It turns dark here.\n");
    }
  return 1;
}





