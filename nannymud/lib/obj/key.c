inherit "obj/property";

string type;
string code;


short()
{
 return "a " + type + " key";  /* Angmar 920220 */
}

set_key_data( str)
{
  if ( sscanf(str, "%s %s", type, code) == 2) 
    return 1;
  return 2;
}
long()
{
 write("\nThis is a " + type + " key, wonder where it fits?\n");
}

query_name() { return "key"; }
  
id( strang)
{
 if ( ( strang == "key" )||( strang == type + " key")||( strang == "H_key") )
   return 1;
 return 0;
}

get()
{
  return 1;
}

query_value()
{
 return 10;
}

query_type() { return type; }
query_code() { return code; }

set_type( str) { type = str; }
set_code( str) { code = str; }

init()
{
  return 0;
}

reset( arg)
{
 if(arg)
   return;
 type = 0;
 code = 0;
 add_property("special_item");
}


query_weight() { return 1; }   /* pen */
