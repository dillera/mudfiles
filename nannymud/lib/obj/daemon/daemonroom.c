inherit "/room/room";

void reset(int arg)
{
  if(arg) return;
  set_light(1);
  short_desc="The outskirts of hell";
  long_desc="You are in the outskirts of hell. This is where all the minor\n"+
    "daemons hang around when they have nothing better to do.\n";
  dest_dir=({});
}

int query_prevent_shadow() { return 1; }
