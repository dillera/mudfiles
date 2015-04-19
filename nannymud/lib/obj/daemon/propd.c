mapping flags;
mapping database = ([ ]);

static mapping qqqq = ([ "Players":1,"Rooms":1,"Objects":1, "Livings":1, "Monsters":1 ]);

#define DFILE "etc/propdatabase"
#define LOGNAME "PROPERTY"
#define VALIDTYPE(X) qqqq[X]

reset(arg) {
  if (arg) return;
  if (_isclone(this_object())) return destruct(this_object());
  restore_object(DFILE);
  move_object(this_object(),"/obj/daemon/daemonroom");
}

short()
{
  return "A Property daemon";
}

query_database() {
  return database;
}

save_database() {
  save_object(DFILE);
}

query_superior_prop(str) {
  if (database[str]) 
    return database[str][3]+({});
  return 0;
}

query_link_prop(str) {
  if (database[str]) 
    return database[str][4]+({});
  return 0;
}

query_prop(str) {
  if (database[str]) 
    return database[str]+({});
  return 0;
}

query_flag(string prop)
{
  return database[prop];
}

query_owner(prop)
{
  return database[prop][0];
}

addp(string name,mixed *all)
{
  database[name]=all;
  save_database();
}

chownp(string namn,string new) 
{
  database[namn][0] = new;
  save_database();
  return 1;
}

changep(string prop,mixed *new)
{
  database[prop][1]=new;
  save_database();
}

removep(str)
{
  mixed *array,*tmp;
  int i,size;
  if (!(array = database[str]))
    return -1;
  database=m_delete(database,str);
  size = sizeof(tmp = array[3]);
  for(i=0;i<size;i++) 
    database[tmp[i]][4] -= ({ str });
  size = sizeof(tmp = array[4]);
  for(i=0;i<size;i++) 
    database[tmp[i]][3] -= ({ str });
  save_database();
  return 1;
}

set_flag(string prop,string flag)
{
  database[prop][5] = flag;
  save_database();
}

addlink(string from,string to)
{
  database[from][4] += ({ to });
}

linkp(string propfran,string proptill)
{
  mixed array1, array2;
  if (!(array1 = database[propfran]) || !(array2 = database[proptill]))
    return -1;
  if (member_array(proptill,array1[3]) != -1)
    return -2;
  if (proptill == propfran) 
    return -3;
  array1[3] += ({ proptill });
  array2[4] += ({ propfran });
  save_database();
  return 1;
}

unlinkp(string propfran, string proptill) 
{
  mixed array1, array2;
  if (!(array1 = database[propfran]) || !(array2 = database[proptill]))
    return -1;
  if (member_array(proptill,array1[3]) == -1)
    return -2;
  array1[3] -= ({ proptill });
  array2[4] -= ({ propfran });
  save_database();
  return 1;
}


/* Added by Aurontas@Viking */
give_regexp_links(name) {
  string *list;

  list = regexp(m_indices(database), name);
  if (sizeof(list) > 1) {
    mapping tmp;
    string *links;
    int i , j;
    tmp = ([ ]);
    i =  sizeof(list) - 1;
    while( i >= 0 ) {
      links = give_me_links(list[i]);
      j = sizeof(links) - 1;
      while( j >= 0 ) {
	++tmp[links[j--]];
      }
      ++tmp[list[i--]];
    }
    return m_indices(tmp);
  }
  if (sizeof(list) == 1)
    return give_me_links(list[0]);
  return ({ });
}

give_me_links(name) {
  string *result;
  mixed array;
  int i,size;
  result = ({ });
  if (!database[name])		/*If not in database*/
    return result;
  array = database[name][3];
  if (!pointerp(array) || !(size = sizeof(array)))
    return result;
  for(i=0;i<size;i++)
    result += give_me_links(array[i]);
  return (result + array);
}


int query_prevent_shadow() { return 1; }
