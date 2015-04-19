/* Written by Profezzorn */
/* This is for making things that can be moved,
 * to make something that can't be moved, look at
 * /std/stationary.c
 */

#pragma strict_types
#pragma save_types

inherit "/std/stationary";

/*** Variables ***/
private int weight;
private mixed value;

/*** Setup functions ***/
void set_value(mixed v) { value=v; }
void set_weight(int w) { weight=w<0 ? 0 : w; }

/*** functions called by mudlib/driver ***/
/* If you are using this, maybe you should consider using
 * /std/stationary.c instead.
 */
int get()
{
  if(query_property("non-gettable")) return 0;
  return 1;
}

int drop(int silently)
{
  if(query_property("non-droppable")) return 1;
  return 0;
}

int query_value() { return eval(value); }
int query_weight() { return weight; }
string extra_look() { return query_property("__extra_look"); }
