/* /obj/container.c - A generic object to contain other stuff */

inherit "/obj/property";

int value, weight, max_weight, local_weight;
string name_of_container ,cap_name ,alt_name ,alias_name;
string short_desc, long_desc, read_msg;
string where = "in"; /* in, on, ... */
string empty_mess;
int is_wet;
string wet_desc="It is soaking wet.\n";

long()
{
  object *tmp;
  string arne;
  int c;
  
  write(long_desc);
  if (is_wet && wet_desc)
    write(wet_desc);
  if (!first_inventory(this_object())&&empty_mess)
    write(empty_mess);
}

set_where(s) {
  if (s)
    where = s;
  else
    where = "in";
}

set_empty_mess(txt)
{
  if (txt)
    empty_mess=txt;
  else
    empty_mess = "You can put things " + where + " it.\n";
}

reset(arg) {
  if (arg)
    return;
  set_where();
  set_empty_mess();
  local_weight = 0;
  is_wet = 0;
}

query_wet()
{
  return is_wet;
}

set_wet_desc(desc)
{
  wet_desc = desc;
}

set_wet(time)
{
  if (!time)
    time = 60;
  is_wet = 1;
  remove_call_out("reset_wet"); /* Added by Qqqq 960319 */
  call_out("reset_wet", time, 0);
  return 1;
}


reset_wet()
{
  is_wet = 0;
}

query_weight() { return weight; }

query_max_weight() { return max_weight; }

add_weight(w) {
    if (local_weight + w > max_weight)
	return 0;
    local_weight += w;
    return 1;
}

short() { return short_desc; }

id(str) {
   return str == name_of_container || str == alt_name || str == alias_name;
}

query_value() { return value; }

can_put_and_get() { return 1; }

get() { return 1; }

prevent_insert() {
#if 0 /* Profezzorn 28/6-95*/
  if (local_weight > weight) {
#else
  if(first_inventory()) {
#endif
    write("You can't when there are things " + where +
	  " the " + name_of_container + ".\n");
    return 1;
  }
  return 0;
}

set_weight(w) 
{ 
  local_weight = w;
  weight = w;
}

set_max_weight(w) { max_weight = w; }

set_value(v) { value = v; }

query_name()
{
  return name_of_container;
}

set_name(n) {
  name_of_container = n;
  cap_name = capitalize(n);
  short_desc = "a "+cap_name;
  long_desc = "It is a "+cap_name + "\n";
}

set_alt_name(n) { alt_name = n; }

set_alias(n) { alias_name = n; }

set_short(sh) { short_desc = sh; long_desc = capitalize(short_desc) + ".\n"; }

set_short_only(sh) { short_desc = sh; }

set_long(lo) { long_desc = lo; }

set_read(str) {
  read_msg = str;
}

init() {
  if (!read_msg)
    return;
  add_action("read", "read");
}

read(str) {
  if (str != name_of_container &&  str != alt_name && str != alias_name)
    return 0;
  write(read_msg);
  return 1;
}
