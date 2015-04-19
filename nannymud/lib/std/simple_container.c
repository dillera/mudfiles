/* Written by Profezzorn */
#pragma strict_types
#pragma save_types

inherit "/std/basic_thing";

/*** variables ***/

private static int local_weight = 7;

/*** setup_functions ***/

void set_max_weight(int w)
{
  int e;
  object *o;
  o=all_inventory();
  for(e=0;e<sizeof(o);e++) w-=o[e]->query_weight();
  local_weight=w;
}

/*** internal functions ***/

int can_put_and_get() { return 1; }

int prevent_insert()
{
  int e;
  object *o;
  o=all_inventory();
  for(e=0;e<sizeof(o);e++)
  {
    if(o[e]->short())
    {
      write("You can't when there are things in the "+query_name()+".\n");
      return 1;
    }
  }
  return 0;
}

int add_weight(int w)
{
  if(w>0 && local_weight - w < 0) return 0;
  local_weight-=w;
  return 1;
}

void set_wet(int t)
{
  if(query_property("waterproof")) return;
  all_inventory()->set_wet(t/2);
}

int _signal_please_take_this(object o)
{
  mixed remote;
  if(remote=query_property("__remote"))
    return (int)remote->remote_prevent_insert(this_object(),o);
}

int _signal_gotten(object o)
{
  mixed remote;
  if(remote=query_property("__remote"))
    remote->remote_handle_insert(this_object(),o);
}
