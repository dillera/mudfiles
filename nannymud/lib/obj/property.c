static mapping prop = ([ ]);

add_property(arg1,arg2) {
  mapping map;
  int i,size;    
  if (mappingp(arg1)) {
    prop += arg1;
    map=arg1;
    arg1=m_indices(map);
    arg2=m_values(map);
    for(i=0,size = sizeof(arg1);i<size;i++) 
      map_array("/obj/daemon/propd"->give_me_links(arg1[i]),"add_property",this_object(),arg2[i]);    
    return;
  }
  if (!arg2)
    arg2 = 1;
  if (stringp(arg1)) {
    map_array("/obj/daemon/propd"->give_me_links(arg1),"add_property",this_object(),arg2);
    prop[arg1] = arg2;
    return;
  }
  if (pointerp(arg1))    
    if (pointerp(arg2))
      {
	map = mkmapping(arg1,arg2);
	prop+=map;
	arg1=m_indices(map);
	arg2=m_values(map);
	for(i=0,size = sizeof(arg1);i<size;i++) 
	  map_array("/obj/daemon/propd"->give_me_links(arg1[i]),"add_property",this_object(),arg2[i]);
      }
      else
	{
	  for(i=0,size = sizeof(arg1);i<size;i++) {
	    prop[arg1[i]] = arg2;
	    map_array("/obj/daemon/propd"->give_me_links(arg1[i]),"add_property",this_object(),arg2);
	  }
	}   
}

set_property(arg1,arg2) {
  prop = ([ ]);
  add_property(arg1,arg2);
}

query_property(arg) {
  if (!m_sizeof(prop))
    return 0;
  if (!arg) 
    return m_indices(prop);
  return prop[arg];
}

query_all_property() {
  return prop;
}

remove_property(arg1)
{
  if (pointerp(arg1)) 
    {
      int i,size;
      for(i=0,size = sizeof(arg1);i<size;i++) 
	m_delete(prop,arg1[i]);
      return;
    }
  prop = m_delete(prop,arg1);
  return 1;
}







