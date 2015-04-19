mapping database=([]);

object query_database(string name)
{
  name=lower_case(name);
  return database[name] || ((database[name]=
			     clone_object("players/milamber/database")),
			    (database[name]->SetName(name)),
			    database[name]);
}

void dest(object o) { o && destruct(o); }

_exit()
{
  int i;
  object *o;
  string s;

  o=m_values(database);
  for(i=0; i<sizeof(o); i++)
    if(s=catch( dest( o[i] ) ))
    {
      write("**Metadatabase: Failed to destruct an object: "+s);
      catch(_destruct( o[i] ));
    }
}


