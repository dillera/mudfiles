/*
 * This is a standard quest object.
 * Configure it to make it look the way you want.
 * -- Modified for quest points by Padrone, Jan 5, 1992.
 */

#define DEFAULT_POINTS		50

string hint_string, name, short_desc;
int quest_points;

string mailtext, other_wiz;

/*
  Mailtext is an alternative text, if you dont like the default message
  then define this variable.
  Other_wiz should be used if you want the mail to be "send" from another
  wizard than the one that created the quest, useful if some wiz takes over 
  another, inactive wiz's area and want the reply to the mail send to him/her.
  /Cel 94
*/


set_mailtext(str) {
  mailtext=str;
}

set_other_wiz(wiz) {
  other_wiz = wiz;
}

query_mailtext() {
  return mailtext;
}

query_other_wiz() {
  return other_wiz;
}

set_hint(h) {
    hint_string = h;
}

set_name(n) {
    name = n;
}

set_short(s) {
    short_desc = s;
}

id(str) { return str == name || str == "quest"; }

query_name() {
    return name;
}

long() {
    write("This is a quest.\n");
    write("Name:  '" + name + "'\n");
    write("Short: '" + short() + "'\n");
    write("Hint:\n");
    write(hint_string);
}

hint() { return hint_string; }

short() {
    string str;
    if (short_desc)
	str = short_desc;
    else
	str = name;
    str = str + " (" + quest_points + " points";
    if (creator(this_object()))
	str = str + ", by " + capitalize(creator(this_object()));
    str = str + ")";
    return str;
}

reset(arg) {
    if (arg == 0)
	quest_points = DEFAULT_POINTS;
}

query_points() { return quest_points; }
set_points(p) { quest_points = p; }

/*
_exit()
{
  if((this_player(1)->query_level_sec()<23) &&
     (this_player(1)->query_real_name()!=creator(this_object())))
    throw("Ne ne.\n");
}
*/
