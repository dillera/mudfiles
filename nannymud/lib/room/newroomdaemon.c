string number_to_string(int x)
{
  switch(x)
  {
    case 0: return "zero";
    case 1: return "one";
    case 2: return "two";
    case 3: return "three";
    case 4: return "four";
    case 5: return "five";
    case 6: return "six";
    case 7: return "seven";
    case 8: return "eight";
    case 9: return "nine";
    case 10: return "ten";
    case 11: return "eleven";
    case 12: return "tweleve";
    case 13: return "thirteen";
    case 14: return "fourteen";
    case 15: return "fifteen";
    case 16: return "sixteen";
    case 17: return "seventeen";
    case 18: return "eightteen";
    case 19: return "nineteen";

    case 20: return "twenty";
    case 30: return "thirty";
    case 40: return "fourty";
    case 50: return "fifty";
    case 60: return "sixty";
    case 70: return "seventy";
    case 80: return "eighty";
    case 90: return "ninety";

    case 21..29: case 31..39: case 41..49: case 51..59:
    case 61..69: case 71..79: case 81..89: case 91..99:
      return number_to_string((x/10)*10)+number_to_string(x%10);

    case 100: case 200: case 300: case 400: case 500:
    case 600: case 700: case 800: case 900:
      return number_to_string(x/100)+" hundred";

    case 101..199: case 201..299: case 301..399: case 401..499:
    case 501..599: case 601..699: case 701..799: case 801..899:
    case 901..999:
      return number_to_string((x/100)*100)+" and "+number_to_string(x%100);

    case 1000..999999:
      if(x%1000)
      {
	return number_to_string(x/1000)+" thousand "+number_to_string(x%1000);
      }else{
	return number_to_string(x/1000)+" thousand";
      }

    case 1000000: return "one million";
    case 1000001..1999999:
      return "one million "+number_to_string(x%1000000);

    case 2000000..999999999:
      if(x%1000000)
      {
	return number_to_string(x/1000000)+" millions "+number_to_string(x%1000000);
      }else{
	return number_to_string(x/1000000)+" millions";
      }

    case -0x7fffffff..-1: return "minus "+number_to_string(-x);

    default:
      return "many";
  }
}

string *my_explode(string a,string b)
{
  string *c;
  if(a==b) return ({"",""}); 
  if(strlen(b) && a[0..strlen(b)-1]==b)
    return ({ "" })+ my_explode(a[strlen(b)..strlen(a)-1],b);
  if(c=explode(a+b,b)) return c;
  return ({});
}

string expand_dir(string s)
{
  switch(s)
  {
  default: return s;
  case "s": return "south";
  case "n": return "north";
  case "e": return "east";
  case "w": return "west";
  case "sw": return "southwest";
  case "nw": return "northwest";
  case "se": return "southeast";
  case "ne": return "northeast";
  case "u": return "up";
  case "d": return "down";
  case "o": return "out";
  case "i": return "in";
  }
}

string compress_dir(string s)
{
  switch(s)
  {
  default: return s;
  case "south": return "s";
  case "north": return "n";
  case "east": return "e";
  case "west": return "w";
  case "southwest": return "sw";
  case "northwest": return "nw";
  case "southeast": return "se";
  case "northeast": return "ne";
  case "up": return "u";
  case "down": return "d";
  case "out": return "o";
  case "in": return "i";
  }
}

string expand_file(object o,string s)
{
  string *tmp;
  switch(s[0])
  {
  case '/': return s;
  case '~':
    if(s[1]=='/')
      return "/players/"+creator(this_object())+s[1..1000];
    else
      return "/players/"+s[1..1000];
    break;

  default:
    tmp=my_explode(file_name(o),"/");
    tmp[-1]=s;
    return "/"+implode(tmp,"/");
  }
}

nomask void set_exit_strings(object room,string *tmp)
{
  string short,long;
  switch(sizeof(tmp))
  {
  case 0:
    long="\n  There are no obvious exits.\n";
    short=" < >";
    break;

  case 1:
    long="\n  There is one obvious exit: "+tmp[0]+".\n";
    short=" <"+compress_dir(tmp[0])+">";
    break;

  default:
    long="\n  There are "+number_to_string(sizeof(tmp))+" obvious exits: "+
      implode_nicely(tmp)+".\n";
    short="<"+implode(map_array(tmp,"compress_dir",this_object()),", ")+">";
  }

  room->set_exit_string(long, short);
}


void add_exit(object room,string cmd,mixed to)
{
  string *tmp;
  tmp=(string *)room->get_dirs();

  cmd=expand_dir(cmd);
  if(member_array(cmd,tmp)==-1)
    set_exit_strings(room,tmp+({cmd}));

  room->add_cmd(cmd,"@"+file_name(this_object())+"->do_move("+to+")");
}

void remove_exit(object room, string cmd)
{
  string *tmp;
  
  tmp=(string *)room->get_dirs();
  cmd=expand_dir(cmd);
  if(member_array(cmd,tmp)==-1) return;
  set_exit_strings(room,tmp-({cmd}));
  room->remove_cmd(cmd);
}

int do_move(string verb,string to)
{
  this_player()->move_player(query_verb(),to);
  return 1;
}

