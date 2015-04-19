/*|   Messd a message daemon for weapons...
  |   The classes of weapon sounds are:
  |   crush, slash, pierce, chop  
  |
  |Date    By             What
  |------------------------------------------------------
  |950506 Taren        First version
  |950927 Leowon       Fiddled a bit with the messages
  |960317 Leowon       Added new damage types
  |
 */


mapping sounds =
([
  "default":
  ({
    "missed", "",
    "tickled", " in the stomach",
    "grazed", "",
    "hit", "",
    "hit", " hard",
    "hit", " very hard",
    "smashed", " with a bonecrushing sound",
    "massacred", " into small fragments",
  }),
  "crush": ({
    "missed", "",
    "patted", " on the stomach",
    "bruised", "",
    "hit", "",
    "hit", " hard",
    "dealt"," a crushing blow",
    "smashed", " with a bonecrushing sound",
    "demolished", " into a bloody pulp",    
  }),
  "slash": ({
    "missed", "",
    "nicked", " in the stomach",
    "cut", " lightly",
    "slashed", "",
    "slashed", " in the chest",
    "slashed", " across the face",
    "cut a deep bleeding wound in","",
    "gutted", " with a mighty slash making blood gush all over",
  }),
  "pierce": ({
    "missed", "",
    "poked", " in the stomach",
    "pricked a small hole in", "",
    "pierced", "",
    "pierced", " badly",
    "punctured", " very badly",
    "dealt", " a deep piercing blow",
    "impaled", " mere inches from the heart",
  }),
  "chop": ({
    "missed", "",
    "scratched", " in the stomach",
    "grazed", "",
    "hacked a wound in", "",
    "hacked a deep wound in", "",
    "chopped a very deep wound in", "",
    "opened a wide bleeding gash in", "",
    "nearly decapitated", " with an incredible swing",
  }),
  "hand-to-hand": ({
    "missed", "",
    "jabbed", " lightly",
    "punched", "",
    "punched", " hard",
    "hit", " with a "+(random(2) ? "right" : "left")+" cross",
    "hit", " with a nasty uppercut",
    "struck", " with a vicious blow",
    "knocked", " out, making teeth fly all over",
  }),
  "fire": ({
    "missed","",
    "warmed", " slightly",
    "singed", "",
    "burned", "",
    "burned", " badly",
    "scorched", " severely",
    "enveloped", " in roaring flames",
    "immolated", " leaving little but ashes",
  }),
  "electricity": ({
    "missed", "",
    "jolted", " slightly",
    "shocked", "",
    "shocked", " badly",
    "made", " jerk convulsively",
    "shocked", " causing horrible spasms",
    "engulfed", " in a huge ball of lightning",
    "electrocuted", " in a torrent of violent lightning",
  }),
  "cold": ({
    "missed", "",
    "chilled", " slightly",
    "numbed", "",
    "dealt", " serious frostbites",
    "drained all warmth from", "",
    "coated", " with ice",
    "assaulted", " with unearthly cold",
    "froze", " into a solid block of ice",    
  }),
  "drain": ({
    "missed", "",
    "weakened", " slightly",
    "drained", "",
    "drained", " of energy",
    "made", " shudder with loss of life",
    "withered", "",
    "sucked the soul from", "",
    "disintegrated", " to ashes", 
  }),
  "acid": ({
    "missed", "",
    "sprinkled drops of acid on", "",
    "scarred", "",
    "burned", " with acid",
    "covered", " with burning acid",
    "burned", " horribly with acid",
    "corroded", " melting flesh and bone",
    "dissolved", " completely",
  }),
  ]);


/*
   get_mess returns 'what' and 'how'
*/

get_mess(int dam,string type)
{
  mixed arr;
  arr=sounds[type];
  if(!arr) arr=sounds["default"];
  switch(dam)
    {
    case 30..99999:  return ({arr[14],arr[15]});
    case 20..29:     return ({arr[12],arr[13]});
    case 15..19:     return ({arr[10],arr[11]});
    case 10..14:     return ({arr[8],arr[9]});
    case 5..9:       return ({arr[6],arr[7]});
    case 3..4:       return ({arr[4],arr[5]});
    case 1..2:       return ({arr[2],arr[3]});
    default:         return ({arr[0],arr[1]});
    }
}

get_hit_lines(int dam,object user,object attacker,string type)
{
  mixed *tmp,*a;
  string an,un;

  if(!objectp(user)||!objectp(attacker)) {
    log_file("qqqq_messd",ctime(time())
	     +": User=");
    if (objectp(user))
      log_file("qqqq_messd",file_name(user)+" ("+user->query_real_name()+")"
	       +": Attacker=");

    if (objectp(attacker))
      log_file("qqqq_messd",file_name(attacker)+" ("+
	       attacker->query_real_name()+")"
	       +".\n");
    if (objectp(previous_object())) {
      log_file("qqqq_messd","Prev Ob.:"+file_name(previous_object())+" ("+
	       previous_object()->query_name()+")"
	       +".\n");      
    }
    return ({"ERROR","ERROR","ERROR"});
  }
  
  un=attacker->query_name();
  an=user->query_name();
  a=get_mess(dam,type);
  if(!a) return ({"ERROR","ERROR","ERROR"});
  tmp=({ capitalize(un)+" "+a[0]+" you"+a[1]+ ".\n",
	 "You " + a[0] + " " + an + a[1]+ ".\n",
	 capitalize(un)+" "+a[0]+" "+an+a[1]+".\n"});
  return tmp;
}

reset(arg)
{
  if (arg) return;
  if (_isclone(this_object())) return destruct(this_object());  
  move_object(this_object(),"/obj/daemon/daemonroom"); 
}

short()
{
  return "A Weapons Sound daemon";
}

id(str)
{
  return str=="sounds"||str=="sound daemon" || str=="messd";
}

int query_prevent_shadow() { return 1; }
