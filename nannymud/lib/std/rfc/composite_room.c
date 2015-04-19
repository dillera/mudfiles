#pragma strict_types
#pragma save_types

inherit "/std/rfc/room";
inherit "/std/rfc/pluralize";

string pluralize0(string s)
{
  string a,b;
  if(sscanf(s,"%s of %s",a,b))
  {
    return pluralize(a)+" of "+b;
  }else{
    return pluralize(s);
  }
}

string pluralize_object(object o, string s)
{
  string name,a,b,c,d;

  name=lower_case((string)o->query_name());
  if(sscanf(" "+s+" ","%s "+name+" %s",a,b))
  {
    name=pluralize0(name);
    s=a+" "+name+" "+b;
    s=s[1..strlen(s)-2];
  }
  else
  {
    s=pluralize0(s);
  }
  return s;
}

int dark(string s,int brief)
{
  string *descs;
  object *objs;
  mixed tmp;
  int d,e;
  string desc,a,b;

  if(set_light(0)<=0) return 0;
  if(!s) s="at room";
  switch(s)
  {
  default:
    return 0;

  case "at room": 
  case "around":
  case "at the room":
  case "move_player":
  }

  if(brief)
  {
    write(capitalize(short()));
  }else{
    long(0);
  }

  objs=all_inventory() - ({this_player()});
  if(!sizeof(objs)) return 1;
  descs=((string *)objs->short());
  tmp=mkmapping(descs,objs);
  descs=m_indices(tmp);
  objs=m_values(tmp);

  for(e=0;e<sizeof(descs);e=d)
  {
    desc=descs[e];
    for(d=e+1;d<sizeof(descs);d++)
      if(descs[d]!=desc)
	break;

    if(!desc) continue;

    if(d-e==1)
    {
      write(capitalize(desc)+".\n");
    }else{
      if(sscanf(desc,"%s %s",a,b))
      {
	switch(lower_case(a))
	{
	case "a":
	case "an":
	case "the":
	  write(capitalize("/obj/support"->number_to_string(d-e))+" "+
		pluralize_object(objs[e],b)+".\n");
	  break;

	case "some":
	  switch(d-e)
	  {
	  case 2..3:
	    write(capitalize(desc)+".\n");
	    break;

	  default:
	    write("Lots of "+b+".\n");
	    break;
	  }

	default:
	  if(a[0]>='0' && a[1]<='9')
	  {
	    write(capitalize("/obj/support"->number_to_string((d-e)*atoi(a)))+
		  " "+
		  b+".\n");
	  }
	  write(capitalize("/obj/support"->number_to_string(d-e))+" "+
		pluralize_object(objs[e],desc)+".\n");
	}
      }else{
	desc=capitalize(desc+".\n");
	while(e++<d) write(desc);
      }
    }
  }

  return 1;
}  





