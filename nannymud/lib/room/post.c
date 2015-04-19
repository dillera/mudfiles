#define EMAIL_DIR "/mail/incoming/"
static inherit "/std/room";

reset(arg) {
  if (arg) return;
  set_light(1);
  add_exit("east","/room/new/narr_alley2");

  set_short("The post office");
  add_property("indoors");
  add_property("no_magic");

  add_property("no_fight");

  set_long("This is the local post office.  It is a huge building with a marble floor\n"+
    "and giant marble columns spaced periodically around the room to support\n"+
    "the vaulted ceiling. The walls are covered with small boxes, each with\n"+
    "someone's name on a little label.\n"+
    "There is a golden plaque on the wall.\n"+
    "mail <name>/<malias>  Mail to player 'name'.\n" +
    "read                  Read your mail.\n"+
    "from                  Show mail headers.\n");
  add_item(({"plaque","golden plaque"}),"These are the Mail aliases you can use:\n"+
  " god: Sends a mail to the God.\n"+
  "arch: Sends a mail to the Arches.\n"+
  "high: Sends a mail to the High Wizards.\n");
}

init() {
  ::init();
  get_email(this_player()->query_real_name());
  if(!present("mail_reader",this_player()))
    move_object(clone_object("obj/daemon/mail_reader"), this_player());
}

exit() {
  object ob;
  if (query_verb()!="wiz" && (ob = present("mailread", this_player())))
    destruct(ob);
}

get_email(name) {
  string *files;
  string header, mess;
  string junk1,junk2,junk3,who,month,day;
  int i;

  files = get_dir(EMAIL_DIR + name + ".*");
  for(i = 0; i < sizeof(files) ; i++) 
  {
    header = read_file(EMAIL_DIR + files[i], 1, 1);
    if(!header)
      continue;
    sscanf(header,"%s %s  %s %s %d",junk1,who,junk2,month,day);
    "/obj/daemon/maild"->add_mail(name,month+" "+day,who,"EMail",
				  read_file(EMAIL_DIR + files[i]),
				  ({ name }));
    rm(EMAIL_DIR + files[i]);
  }
}

query_mail(silent) {
  string name;
  int q,w;

  name = lower_case(this_player()->query_real_name());
  q="/obj/daemon/maild"->new_mail(name);
  w=sizeof("/obj/daemon/maild"->query_mail(name));
  if(!w) return 0;
  if (silent) return w;
  write("\nGuess what? There is" +(q>-1?" NEW":"")+" mail for you in the post office!\n"+
	"   (s,e,e,s,s and then w from the church.)\n\n");
  return w;
}




