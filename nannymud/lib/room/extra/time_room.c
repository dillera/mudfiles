inherit "/std/basic_room";
#define WRAP(X) ( (X && !sscanf((X), "%*s\n") && (X)[0]!='@') ? sprintf("%-=77s\n@@time_extra_long@@\n", (X)) : (X) )

string season(int autumn) {
  string s;
  s = (string) "/obj/daemon/timed"->query_season();
  if( autumn && s == "fall" )
    s = "autumn";
  return s;
} /* summer, spring, fall, winter */

string hour() {
  return (string) "/obj/daemon/timed"->_part_of_day(time(), 1, 1);
} /* night, morning, day, noon, afternoon, evening */

string moon() {
  switch( (int) "/obj/daemon/timed"->_phase(time(), 1) ) {
    case 0: default: return "full moon";
    case 1: return "waning gibbous moon";
    case 2: return "waning half moon";
    case 3: return "waning crescent moon";
    case 4: return "new moon";
    case 5: return "waxing crescent moon";
    case 6: return "waxing half moon";
    case 7: return "waxing gibbous moon";
  }
}

int query_daytime() {
  string s;
  s = hour();

  return s != "evening" && s != "night";
}

int query_moon() {
  return !!((int) "/obj/daemon/timed"->_phase(time(), 1) != 4);
} /* is the moon out? */

void time_item_setup() {
  this_object()->add_item("moon", "@moon_string()");
  this_object()->add_item("sky", "@sky_string()");
  this_object()->add_item("sun", "@sun_string()");
  this_object()->add_item("stars", "@stars_string()");
}

string moon_string() {
  string m;

  if( query_daytime() )
    return "It is daytime and the moon is not up.\n";
  else {
    if( query_moon() ) {
      if( sscanf(m = moon(), "%*screscent") )
        return "The sliver of the "+m+" barely gives enough light to see by.\n";
      else
        return "The bright light of the "+m+" shines down on you.\n";
    } else
      return "The moon is new and cannot be seen in the night sky.\n";
  }
}

string sky_string() {
  if( query_daytime() )
    return "The "+hour()+" sun shines down, warming you slightly.\n";
  else {
    if( query_moon() )
      return line_break("The pale "+moon()+" and many a thousand distant "+
	"stars shine down from above.");
    else 
      return "Thousands of pale pinpoints of light fill the starry sky "+
	"above.\n";
  }
}

string sun_string() {
  string s;

  if( query_daytime() ) {
    switch( s = season(1) ) {
      case "autumn": case "winter": s = "cold " + s; break;
      default: s = "warm " + s; break;
    }
    return "The "+s+" sun glows in the sky above.\n";
  }
  return "It is night-time, and the sun has long set.\n";
}
    
string stars_string() {
  if( query_daytime() )
    return "It is "+hour()+"; you must wait until the sun sets to see the "+
	"stars.\n";
  return "The starry sky lightly illuminates your path.\n";
}

string time_extra_long() {
  if( query_daytime() ) {
    if( hour() == "morning" )
      return "The morning sun is rising in the east.";
    if( hour() == "afternoon" )
      return "It is late afternoon and the sun is setting in the west.";
    return "It is "+hour()+"; the sun hangs high in the sky.";
  } else {
    if( !query_moon() )
      return "It is night, and the moon is new.  The stars barely guide "+
	"your way.";
    else
      return "It is night, and the moon and stars sparkle in the sky.";
  }
}

string query_long(string str) {
  if( !str )
    return process_string(::query_long());
  return ::query_long(str);
}

void set_long(string s) { ::set_long(WRAP(s)); }
