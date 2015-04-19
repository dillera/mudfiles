// fingerd.c
// An improved interface to provide the information required by the "finger"
// command, as well as other essential information about the user, that may not be
// available when they are not logged in.
// This version of the finger daemon was written by Buddha@TMI-2, with some
// existing code from the old finger daemon (most of which he also did, but not all).
// This is part of the TMI-2 distribution mudlib.
// Please retain this header.
// Bug - bad argument 1 to extract() line 133
// possibly caused by someone being half logged in
// Mobydick thinks he fixed this bug, hasn't had the chance to test it.
// 2-15-93.

#define DEBUG_MACRO
#define TELL "buddha"
#include <debug.h>

#include <uid.h>
#include <config.h>
#include <login_macros.h>
#include <daemons.h>


void create() {
	seteuid(ROOT_UID);
}
	
// This function is called to restore the user's "connection object" that holds the 
// essential login information.  If the user in question is currently logged in, then 
// their current "connection object" is returned, otherwise it is restored.  You
// should probably call clean_up() on the object when finished, and it will either 
// self-destruct, if not interactive or associated to a body, or save and wait for 
// further information requests, if it is.
 
object restore_data(string name) {
	object link;
	if (find_player(name) && interactive(find_player(name))) {
		return (object)find_player(name)->query_link();
	}
	link = new(CONNECTION);
	seteuid(name);
	export_uid(link);
	seteuid(ROOT_UID);
	link->set("name", name);
	if (link->restore()) {
		debug(1, "Link restore succeeded.\n");
		return link;
	}
	debug(1,"No link object found by restore_data...\n");
	// If this fails, then let's clean up afterwards...
	link->remove();
	return 0;
}

string finger_user(string who) {
   
	object link;
	mixed tmp1, tmp2, tmp3, tmp4, tmp5;
   	string msg;
   	mapping mail_stat;
  
	link = restore_data(who); 
	if (!link) return "finger: couldn't find user.\n";
		
	tmp1  = "Login name: " + who; 
	tmp2 = "In real life: ";
  	if ( tmp3 = (string)link->RNAME) 
		tmp2 += extract(tmp3, 0, 22);
	else tmp2 += "?";
	// This part, could be a little more efficient, because the domain daemon ends up
	// having to query the domains mapping from the link twice, to get all this info. 
	tmp4 = DOMAIN_D->query_domain(link);
	tmp5 = DOMAIN_D->query_domain_level(link);
	if ((tmp4==0) || (tmp4=="")) tmp4 = "None";
	tmp4 = "Domain: " + tmp4 + " (" + tmp5 + ")";
	msg = sprintf("%-40s%-40s\n", tmp1, tmp2);
	tmp1 = user_path(who);
	if(file_size(tmp1) == -1) tmp1 = "(none)";
	msg += sprintf("%-40s%-40s\n", "Directory: " + tmp1, tmp4);
	tmp1 = find_player(who);
	if(!link->query("last_on"))
	msg += (tmp1 ? "On since: " : "Last on: ") + "Unavailable\n";
	else
	msg += (tmp1 ? "On since: " : "Last on: ") +ctime((int)link->query("last_on"))+
		" from " + (string)link->query("ip") + "\n";
	if (tmp1) {
		tmp1 = query_idle_string(tmp1, 1);
		if (strlen(tmp1) > 0) msg += tmp1 + "\n";
	}
	 mail_stat = (mapping)MAILER_D->mail_status(who);
	if (mail_stat["unread"])
	 msg += capitalize(who)+" has not read "+mail_stat["unread"] 
		+ " of " + " their "+ mail_stat["total"] 
		+ " piece(s) of mail.\n";
	else msg += "No unread mail.\n";
	tmp1 = (string)link->query("email");
	msg += "email address: " + tmp1 + "\n";
	tmp1 = user_path(who) + ".project";
	if(file_size( tmp1 ) >= 0) msg += "Project: " + read_file(tmp1);
	tmp1 = user_path(who) + ".plan";
	if (file_size( tmp1 ) >= 0) {
		msg += "Plan:\n" + read_file(tmp1);
	}
	else msg += "No Plan.\n";
	return "\n" + msg + "\n";
}

string finger_all() {
   
	object *who, link;
	int i;
	string msg;
   
	who = users(); 
	 i = sizeof(who);
	// this would sort people in order of level... but it's kind of yucky to depend on
	// the code staying in the right place.
	// who = sort_array(users(), "sort_users", find_object_or_load(WHO));
	 msg =
"+=--=+=--=+=--=+=--=+=--=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+\n";
	if(sizeof(who) == 0) 
	msg += "[" + mud_name() + "] No one logged in at " + ctime(time()) + ".\n";
	else {
	msg += "[" +mud_name()+ "] " + sizeof(who) +
		(i == 1 ? " user" : " users") + " connected at " +
		ctime(time()) + ".\n";
	msg += 
"+=--=+=--=+=--=+=--=+=--=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+\n";
	msg += sprintf("%-12s%-20s%-14s%-7s%-20s","Login","Name","Position",
		"Idle","Where\n");
	msg += 
"+=--=+=--=+=--=+=--=+=--=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+\n";
	}
	for (i=0; i<sizeof(who); i++) {
		link = (object)who[i]->query_link();
		if(!link) continue;
		if (!link->RNAME) continue ;
		msg += sprintf( "%-12s%-20s%-14s%-7s%-20s",
			(string)link->NAME,
			extract((string)link->RNAME, 0, 18 ),
			(string)DOMAIN_D->query_domain_level(link),
			query_idle_string(who[i], 0),
			extract(query_ip_name(who[i]), 0, 20 ) 
			+ "\n" );
   }
   return msg + 
   "+=--=+=--=+=--=+=--=+=--=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+=--=+\n";
}
