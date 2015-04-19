/* Written by Mats */

#pragma strict_types

object master;

void set_master(object m) { master = m; }

string short() { return "a store room"; }

void long() {
  write(short());
}

void reset(int arg) { 
  if (!arg) set_light(1);
}

/*
 * Clean up this object if no shop 
 */
int clean_up(int arg)
{
  if(environment()) return 0;
  if(master) return 1;
  while(first_inventory()) destruct(first_inventory());
  if(_isclone(this_object()) ||
     (arg<2 && this_object()==_next_clone(this_object())))
    destruct(this_object());
  return 1;
}
