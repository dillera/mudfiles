inherit "/room/room";
mixed *arr;

reset(arg) {
  if(arg) return;
  set_indoors();
  set_light(1);
  add_exit("room/church", "up");
  add_exit("room/crypt1", "north");
  add_exit("room/crypt2", "northwest");
  add_exit("room/crypt3", "west");
  add_exit("room/crypt4", "southwest");
  add_exit("room/crypt5", "south");
  add_exit("room/crypt6", "southeast");
  add_exit("room/crypt7", "east");
  add_exit("room/crypt8", "northeast");
  set_short("Center of the crypt");
  set_long("You are standing in a quite dark crypt.\n"+
	   "There are statues in the adjacent rooms of all the persons\n"+
	   "who have helped Nanny and donated money to her.\n"+
	   "There is a small plaque on the floor.\n");
  add_item("plaque","You can read the plaque");
  arr=({"leowon","brom","banshee","alexii","kelvin","restless","aldred","taren","nib","speedfreak","zahrim","silvana","greowe","anna","rohan","fenrir","padrone","inc","oriole","goreon","rand","traste","asd","party","charmangle","qqqq","cray","taunter","grover","neill","akasha","azroth","teslor","profezzorn","gondul","brago"});
  set_up_statues();
  extra_commands=({"read","read"});
} /* reset */

read(str)
{
  if(!str)
    return notify_fail("Read what?\n");
  if(str=="plaque")
    {
      write("You can do 'help donate' to get more information about\n"+
	    "how to donate money to NannyMud.\n");
      return 1;
    }
  write("There is no "+str+" here.\n");
  return 1;
}


get_old(who)
{
  int i;
  object room;
  for(i=1;i<9;i++)
  {
    room=load_object("/room/crypt"+i);
    if(present("Statue of "+who,room)) return 1;
  }  
}

special()
{
  mixed *a;
  object ob;
  int i;
  /*Smalt and Svamplover WANTS to have their statues in crypt3*/
  a=({"smalt","svamplover"});
  for(i=0;i<sizeof(a);i++)
    {
      if(!get_old(a[i]))
	{
	  ob=clone_object("/room/statue");
	  ob->set_the_name(a[i]);
	  move_object(ob,"/room/crypt"+3);
	}
    }
}

set_up_statues()
{
  int i,p;
  object ob,old;
  special();
  for(i=0;i<sizeof(arr);i++)
  {
    if(!get_old(arr[i]))
    {
      p=random(8)+1;
      if(p<1) p=1; /* Paranoia ehh? */
      if(p>8) p=8;
      ob=clone_object("/room/statue");
      ob->set_the_name(arr[i]);
      move_object(ob,"/room/crypt"+p);
    }
  }
}


