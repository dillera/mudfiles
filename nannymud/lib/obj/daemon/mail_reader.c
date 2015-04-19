/* (c) Milamber 1993-95 */
/* ... and get() { return 1; } added by Brom ... do I have (c) on that? */

#include <mail.h>

/* ({ "a", "b", "c"  }) -> "a, b and c" */
public string implode_nicely(string *dum)
{
  int s;
  s=sizeof(dum);
  switch(s)
  {
   default:   return implode(dum[0..s-2],", ")+" and "+dum[s-1];
   case 2:    return dum[0]+" and "+dum[1];
   case 1:    return dum[0];
   case 0:    return "";
  }
}


/* Variables used when writing mails */
static string receiver;			/* receiver of the mail */
static string mail;			/* mail body */
static string subject;			/* mail-subject */
static string *cc;			/* carbon-copy's */
static private int in_reader;		/* flag for checking status */

/* Misc vars */
static mixed mails;                     /* Cache used to speed things up */
static int current;                     /* The current read mail */
static string owner;			/* who is using me now */
static private int silent_send;         /* When questdeamon sends mail, 
					   dont show "sending mail to: xxx" 
					   CEL  94*/

status id(string s)  
{ 
  if(stringp(s))
    return sscanf(s,"mail%*s"); 
}

string short( /* void */ )
{ 
  return "a mailreader"; 
}

void long( /* void */ ) 
{ 
  write("Type 'help mail' for help on mail\n"); 
}

void init( /* void */ ) 
{
  if (this_player(1) != environment())
    return;

  add_action("mail","mail");
  add_action("read","read");
  add_action("show","head");
  add_action("show","from");

  owner = this_player()->query_real_name();
}

static string mail_status() 
{
  int i, New;
  mixed *tmp;
  tmp = MAILD->query_mail(owner);
  if (!pointerp(tmp))
    return "No mail.\n";
  for (i = 0; i < sizeof(tmp); i++) 
    if (tmp[i][0] == 1)
      New++;
  return sprintf("You have %d message%s, %d new.\n", i, (i==1)?"":"s", New);
}


static void mail_printout(mixed *tmp, int from, int to) 
{
  int i;
  string pointer, tmp2;
  for (i = from; i < to; i++) 
    write(sprintf("%s%s %2d %-12s %-17s %4d/%-5d %-20s\n", 
		   (current == (i+1)) ? ">" : " ",
		   tmp[i][FLAG]?"N":" ",(i+1), capitalize(((tmp[i][FROM])?
							   (tmp[i][FROM]):
							   ("unknown"))), 
		   tmp[i][TIME], sizeof(explode(tmp[i][MESS],"\n")||({})),
		   strlen(tmp[i][MESS]),
		   tmp[i][SUBJ][0..30]));
}

status show()
{
  mixed *f;
  write(mail_status());
  current = -1;
  mail_printout(f=MAILD->query_mail(owner),0,sizeof(f));
  return 1;
}

void output( string s )
{
  while(strlen(s) > 5000)
  {
    write( s[0..4999] );
    s=s[5000..1000000];
  }
  write(s);
}

/* return mail number i, formatted and all :) */
string give_mail(int i)
{
  mixed *tmp;
  tmp=MAILD->query_mail(owner)[i];
  MAILD->set_mail_flag(owner,i,0);
  return (sprintf("Message: %-d\n"+
		  "From: %-12s\n"+
		  "To: %-12s\n"+
		  "Date: %-16s\n"+
		  "Subject: %-50s\n"+
		  "%-s\n",
		  i+1, 
		  capitalize(((tmp[FROM])?(tmp[FROM]):("unknown"))), 
		  capitalize(tmp[CC][0]),
		  tmp[TIME], 
		  tmp[SUBJ], 
		  (sizeof(tmp[CC]) < 2) ? "\n": 
		  "Cc: " + implode_nicely(tmp[CC][1..100000]) + "\n")
	  + /*large_*/line_break(tmp[MESS]));
}

mail_prompt()
{
  mails=MAILD->query_mail(owner);
  write("Mail [q,#,h,n,p,d,r,f,m,?] (" + current + "/" + sizeof(mails) + ") ");
  input_to("main");
}

status next_unread()
{
  int c;
  while(!mails[ c ][ FLAG ]) /* goto the first unread mail */
    if(++c >= sizeof(mails))
      return 0;
  return c + 1;
}

status read()
{
  write("Mail Compat version v1.0\n");
  write(mail_status());
  mails=MAILD->query_mail(owner);
  current=1;
  if(!sizeof(mails))
    return write("You have no mail.\n"),1;
  if(!(current = next_unread()))
  {
    write("You have no new mail.\n");
    current = sizeof( mails );
  }
  current--;
  in_reader=1;
  mail_prompt();
  return 1;
}

void main(string str)
{
  int c;

  if( c = atoi(str) )
  {
    if(c > sizeof(mails))
    {
      write("Illegal message number, you only have "+ sizeof(mails) 
	    + "mails.\n");
      mail_prompt();
      return;
    } else if(c < 1) {
      write("Illegal message number. The first message is '1'.\n");
      mail_prompt();
      return;
    }
    current = c;
    output( give_mail(current - 1) );
    mail_prompt(); 
    return;
  }

  if(!strlen(str))
    if( c = next_unread() )
    {
      current = c;
      output( give_mail(current - 1) ); 
      mail_prompt();
      return;
    } else {
      write("You have no more unread mail.\n");
      mail_prompt();
      return;
    }

  switch( str[0] )
  {
   case 'n':
    if(++current > sizeof(mails))
    {
      current--;
      write("This is the last mail in your mailbox.\n");
    } else {
      write(give_mail(current-1)); 
    }
    break;
    
   case 'p':
    current--;
    if(current<1)
    {
      write("This is the first mail in your mailbox.\n");
      current++;
    } else {
      write(give_mail(current-1));
    }
    break;
    
   case 'd':
    MAILD->remove_mail(owner, --current);
    if(current<1) current=1;
    if(sizeof( mails=MAILD->query_mail(owner)))  break;
    write("You have no more mail in your mailbox.\n");

   case 'x':
   case 'q':
    if(!random(100))
    {
      write("--\n"); "players/profezzorn/fortune"->ffortune(); write("--\n");
    }
    write("Have a nice day!\n");
    return;
    
   case 'h':
    mail_printout(mails, 0,sizeof(mails));
    break;
    
   case 'r':
    if(current) 
    {
      subject="Re: "+mails[current-1][SUBJ];
      receiver=capitalize(mails[current-1][FROM]);
      write("To: "+receiver+"\n"+"Return for "+"["+subject+"]\nSubject: ");
      mail="";
      input_to("get_subject");
    }
    return;
    
   case 'f':
    if(sscanf(str,"f%s",str) && strlen(str=replace(str," ", "")) && current)
    {
      receiver=str;
      subject="Forwarded: "+mails[current-1][SUBJ];
      input_to("get_subject");
      write("To: "+receiver+"\n"+"Return for "+"[" + subject + "]\nSubject: ");
      mail=give_mail(current-1) + "\n---------------------\n";
      return;
    }
    write("Syntax: f <username>\n");
    break;
    
   case 'm':
    if(sscanf(str,"m%s",str) && strlen(str=replace(str," ", "")))
    {
      receiver=str;
      subject="";
      input_to("get_subject");
      write("To: "+receiver+"\nSubject: ");
      mail="";
      return;
    }
    write("Syntax: m <receiver>\n");
    break;
    
    
   default:
    write("n    Next mail\n"+
	  "p    Previous mail\n"+
	  "h    Headers\n"+
	  "f w  Forward mail to w\n"+
	  "m w  Mail w\n"+
	  "d    Delete mail\n"+
	  "r    Reply to mail\n"+
	  "#    Go to mail number #.\n"+
	  "<ret>Next unread mail.\n"+
	  "q    Quit reading\.n");
  }
  mail_prompt();
}

status mail(string who)
{
  if(!who)
  {
    write("Mail whom?\n");
    return 1;
  }
  in_reader = 0;
  cc = MAILD->query_malias( who );
  if(!cc)
  {
    write(who + ": No such player\n");
    return 1;
  }
  receiver = capitalize(who);
  mail="";
  write("Subject: ");
  input_to("get_subject");
  return 1;
}

static void get_subject( string str ) 
{
  if (strlen(str) ||  !subject)
    subject = (str||"");
  write("Give message. '.' on a new line to end mail. '~q' to abort\n");
  write("              ~r to redisplay the text you typed so far.\n");
  write("-----------------------------------------------------------\n");
  output(mail);
  write(". ");
  input_to("get_mail");
}

static get_mail(string str) 
{
  if(str == "~r")
  {
    write("------------------------------------------------------\n");
    output( mail );
    input_to( "get_mail" );
    return;
  }
  if (str == "~q") 
  {
    write("Interrupt -- mail aborted.\n");
    mail_prompt();
    /* CLEAR the variable that tells whom we tried to mail, or we will
     * include that person in next post.
     * Brom 960209
     */
    receiver = "";
    cc = ({});
    return 1;
  }
  if (str == "." || str == "**") 
  {
    if (!pointerp(cc)) 
      cc=({});
    write("Cc: " + implode(cc[1..1000]+({""}), " "));
    input_to("get_cc");
    return 1;
  }
  mail += str + "\n";
  if(strlen(mail) > 20000)
    write("******* The mail is getting to long.\n");
  input_to("get_mail");
  write(". ");
  return 1;
}

static get_cc(string str)
{
  string *tmp;

  if (!str || str == "") 
  {
    send_mail(({ receiver }));
    return 1;
  }
  tmp = replace(str, ",", " ");
  if(!(tmp = explode(tmp , " ")))
    tmp=({});
  
  tmp -= ({ "" });
  tmp = ({ receiver }) + 
    map_array(tmp, "grumble", this_object());
  send_mail(tmp);
  return 1;
}

grumble(string str) 
{
  return capitalize(lower_case(str));
}

resolve_aliases(mixed *s)
{
  int i;
  mixed q,w;
  q=({});
  for(i=0;i<sizeof(s);i++)
    if(w=MAILD->query_malias(s[i]))
      q+=w;
    else
      write(s[i]+ ": No such player.\n");
  
  w=([]);
  for(i=0;i<sizeof(q);i++)
    w[q[i]]=1;

  return m_indices(w);
}

static send_mail(string *arg) 
{
  int i;
  object ob; /* Some optimisation by Brom 951002 */
  string *tmp;
  
  if (!pointerp(cc))
    cc = ({});

  cc = resolve_aliases(arg + cc);

  for (i = 0; i < sizeof(cc); i++) 
  {
    MAILD->add_mail(lower_case(cc[i]) , DATE, owner, subject, mail, cc);
    if (ob = find_player(lower_case(cc[i]))) {
      if(stringp(owner) && find_player(lower_case(owner))) {
	tell_object(ob, "You have new mail from "+capitalize(owner)+".\n");
      } else {
	tell_object(ob, "You have new mail.\n");
      }}
  }

  if(!silent_send) {
     write("sending mail to: " + implode_nicely(cc) + ".\n"); }
  else { silent_send = 0; }

  cc = ({});
  if(in_reader)
  {
    mail_prompt();
    return 1;
  }
}

quest_send(string *args)   /* Added by Celeborn Jan 94, see quest_mailer */
{
  string tmp;
  if (file_name(previous_object()) != "obj/quest_mailer") {
      tmp = "None?";
      if (this_player(1)) tmp = (this_player(1)->query_real_name());    
      write_file("log/SECURE/FALSE_MAIL",
		 ctime(time()) + " " +
		 file_name(previous_object()) + " : " +
		 tmp + "\n");
      return;
  }
  cc = ({ });
  owner = args[1];
  subject = args[2];
  mail = args[3];
  silent_send = 1;
  in_reader = 0;
  send_mail(({args[0]}));
}

resume_mail() 
{
  if (this_player(1) != environment()) 
    return 0;
  mail = read_file(TMP_ED_FILE(owner));
  rm(TMP_ED_FILE(owner));
  input_to("get_mail");
  write(". ");
  return 1;
}

string query_mail_flag(int flag) 
{
  return flag?"N":" ";
}

drop() { return 1; }
query_prevent_shadow() {  return 1; }

int get() { return 1; }
