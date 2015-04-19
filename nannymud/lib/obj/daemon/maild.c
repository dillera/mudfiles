/* (c) Milamber 1993 */
#include <mail.h>

mapping save_mail;
static mapping mail = ([]);
static mapping m_alias = ([]);
static string cached_user;
static mapping saved = ([]);
static mapping timestamps = ([]);

reset(int a)
{
  int e, *st;
  string *mailfiles;

  if(!a)
  {
    make_mail_alias();
    move_object(this_object(),"/obj/daemon/daemonroom");

    /* Remove old mailfiles /Profezzorn */
    mailfiles=get_dir(MAILSAVE(""));
    if(!mailfiles) return;
    mailfiles-=(string *)"/local/gp"->gp();
    for(e=0;e<sizeof(mailfiles);e++)
    {
      st=MAILSAVE(mailfiles[e]);
      if(st && st[3]<time()-24*3600*7)
	rm(MAILSAVE(mailfiles[e]));
    }
  }
}

int id(string s) { return s=="maildaemon" || s=="maild"; }
short() { return "a maildaemon"; }
long()
{
  write("The small maildaemon misses the days when he was a large and\n"+
	"all-consuming maildaemon.\n");
}

private static void restore_mail(string who) 
{
  save_mail=0;
  timestamps[who]=time();
  if(mail[who])
    return;
  if(-1==find_call_out("trash_cache"))
  {
    remove_call_out("trash_cache");
    call_out("trash_cache",300);
  }
  restore_object(MAILSAVE(who));
  cached_user=who;
  if(save_mail)
  {
    mail[who]=save_mail;
  } else {
    mail[who]=({ });
  }
}

real_save_mail(string who)
{
  if(!saved[who] && mail[who])
  {
    saved[who]=1;
    save_mail=mail[who];
    save_object(MAILSAVE(who));
    save_mail=0;
  }
}


/* Profezzorn doesn't think this object should cache _everything_ */
void trash_cache()
{
  string *who;
  int e;
  who=m_indices(mail);
  if(environment())
    tell_room(environment(),"The maildaemon burps.\n");
  for(e=0;e<sizeof(who);e++)
  {
    if(timestamps[who[e]]>time()-200) continue; /* Trash next time */
    if(!saved[who[e]]) real_save_mail(who[e]);
    mail=m_delete(mail,who[e]);
    timestamps=m_delete(timestamps,who[e]);
    saved=m_delete(saved,who[e]);
  }
  if(m_sizeof(mail))
  {
    remove_call_out("trash_cache");
    call_out("trash_cache",200);
  }
}


private static queue_save_mail(string who) 
{
  call_out("real_save_mail",1,who);
}

add_mail(string who,string date, string from, 
	 string subject, string m, string *to) 
{
  string tmp;

  who=lower_case(who);
  restore_mail(who);
  mail[who] += ({ ({ 1, from, subject, date, to, m }) });
  saved[who]=0;
  queue_save_mail(who);
}

remove_mail(string who, int number) 
{
  who=lower_case(who);
  if (!intp(number))
    return 0;
  restore_mail(who);
  mail[who] = mail[who][0..(number-1)] + mail[who][(number+1)..100000];
  saved[who]=0;
  queue_save_mail(who);
  return 1;
}

query_mail(string who) 
{
  if(!who) return 0;
  who=lower_case(who);
  restore_mail(who);
  if(this_player(1) && this_player(1)->query_real_name() != who)
    return sizeof(mail[who]);
  return mail[who];
}

set_mail_flag(string who, int number, int stat) 
{
  who=lower_case(who);
  if (!stringp(who) || !intp(number) || !intp(stat))
    return 0;
  restore_mail(who);
  if(mail[who][number][0] == stat) return 1;
  mail[who][number][0] = stat;
  saved[who]=0;
  queue_save_mail(who);
  return 1;
}

new_mail(string str) 
{
  string *tmp;
  int i;
  restore_mail(str);
  tmp = mail[str];
  for(i=0;i<sizeof(tmp);i++)
    if(tmp[i][0]) 
      break;
  if(i<sizeof(tmp))
    return i;
  return -1;
}

static make_mail_alias() 
{
  string str, *tmp, *tmp2;
  int i;

  str = implode(explode(implode(explode(read_file(MAIL_ALIAS_FILE)
					,"	"),"")," "),"");
  if (!str)
    return;
  tmp = explode(str, "\n");
  
  for (i = 0; i < sizeof(tmp); i++) 
  {
    if(tmp[i][0]=='#')
      continue;
    tmp2 = explode(tmp[i], ":");
    if(tmp2 && sizeof(tmp2)>1)
      m_alias[tmp2[0]] = explode(tmp2[1], ",");
  }
}

query_malias(str) 
{
  str=lower_case(str);
  if(m_alias[str])
  {
    mixed *res;
    int i;
    res=({});
    str=m_alias[str];
    for(i=0;i<sizeof(str);i++)
    {
      if(str[i][0]=='(')
	res+=query_malias(str[i][1..strlen(str[i])-2]);
      else
	res+=str[i..i];
    }
    return res;
  }
  if (file_size(CHAR_SAVE(str)) > 0)
    return ({ str });
}


void sync()
{
   string *who;
   int i;
   for(who = m_indices(mail), i = 0; i < sizeof(who); i++)
      real_save_mail(who[i]);
   if(environment())
      tell_room(environment(), "The maildaemon cackles wickedly.\n");
}

int query_prevent_shadow() { return 1; }
