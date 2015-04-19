/*
 * Intermud tell server
 * Original author: Huthar@Portals
 * Rewritten: Blackthorn@Genocide (92-10-29)
 * Include files tweaked: Buddha@tmi-2 (92-11-06)
 * Mostly Rewritten: Pallando@Ephemeral Dale (92-12-12)
 * Line 98 changed to stop a wasted socket: Pallando (93-02-05)
 */

#include <socket.h>
#include <inetd.h>
#include <network.h>
#include <channels.h>

inherit "/cmds/channels/std/broadcast"; // for intermud channels

#define IDLE_TIME 60
#define GIVE_SIGNAL 1
#define NOT_HERE_MESS " doesn't seem to be about here."
#define log( x )
/*
#ifndef log( x )
// for some reason this doen't work
#define log( x ) log_file( "TS", wrap( ctime(time()) + " : " + x ) )
#endif log( x )
*/

mapping requests;
string *muds;

string tell_user( string who, string mud, string target, string msg );
void close_connection( mixed id );

void create() {
  mapping mud_list;

  requests = ([]);
  muds = ({ });
  seteuid( getuid() );
  mud_list = NAME_SERVER-> query_muds();
  muds = keys( mud_list );
  muds -= ({ "DEFAULT" });
}

/*
  read_callback() recieves the messages from other muds - both tells from 
  users on other muds and messages from the TS on other muds giving the
  results of tells from users on this mud to users on the other mud.
*/
void read_callback( int id, string msg )
{
  string originator, mud, target, mesg;
  object ob;
  int len;
  
  log( "read_callback : id = " + id + " : msg = " + msg );
  if( !msg ) return close_connection( ({ id }) );
  if( sscanf( msg, "%s@%s tells %s: %s",
    originator, mud, target, mesg ) != 4 )
    return close_connection( ({ id }) );
  len = strlen( mesg ) - 1;
  if( mesg[len..len] == "\n" ) mesg = mesg[0..(len-1)];
  if( mesg == "Cannot find Interwiz here." ||
      mesg == "Cannot find Intermud here."   )
    return close_connection( ({ id }) );
  target = lower_case( target );
  if( originator == "TS" || originator == "Game Driver" )
  {
    if( ob = find_player( target ) )
      tell_object( ob, capitalize( mud ) + "'s Tell Server informs you:\n" +
        wrap( mesg ) );
  } else if( originator == "interwiz" ) {
    broadcast_channel( "interwizzes", CHAN_INTERWIZ, mesg,
      capitalize( originator ) + "@" + capitalize( mud ) );
  } else if( target == "intermud" ) {
    broadcast_channel( "intermuds", CHAN_INTERMUD, mesg,
      capitalize( originator ) + "@" + capitalize( mud ) );
  } else {
    INETD-> write_socket( id, "TS@" + THIS_MUD + " tells " +
      lower_case( originator ) + ": " +
      tell_user( originator, mud, target, mesg ) + "\n" );
  }
  close_connection( ({ id }) );
}

/*
  service_callback() is called by the remote mud when the connection
  requested by the open_service() in remote_tell() is opened.
*/
void service_callback( int id )
{
  string name;
  log( "service_callback : id = " + id + " : requests = " +
    identify( requests[id] ) );
  if( !requests[id] ) return;
  if( objectp( requests[id][0] ) ) name = requests[id][0]-> query( "cap_name" );
  else if( stringp( requests[id][0] ) ) name = requests[id][0];
  else { close_connection( id ); return; }
  INETD-> write_socket( id, name + "@" + THIS_MUD + " tells " +
    requests[id][1] + ": " + requests[id][3] + "\n" );
  if( objectp( requests[id][0] ) )
    tell_object( requests[id][0], "Remote tell sent.\n" );
  call_out( "close_connection", 10, id );
/*
  call_out changed from 4 seconds to 10.
  feedback from remote tell server is more likely
*/
}

/*
  close_connection() is called by service_callback() to close the
  connection once the tell has been sent.
  and by read_callback once it has made its response.
*/
void close_connection( mixed id )
{
  if( intp( id ) ) map_delete( requests, id );
  else id = id[0];
  INETD-> close_socket( id );
}  // give them a chance to respond if player doesn't exist

/*
  tell_user() is used by users on this mud (via _tell.c & _reply.c) and
  users on other muds (via read_callback()) to tell users on this mud.
*/
string tell_user( string who, string mud, string target, string msg )
{
  object ob, sender;
  string ret, receiver;
  string * ignore;
  int priority_sender, priority_receiver;
  int invis_sender, invis_receiver;
  
  log( "tell_user : " + who + ( mud ? ("@"+mud) : "" ) + " tells " + target +
    ": " + msg );
  if( !who || !target || !msg ) return "Syntax: tell <who> <msg>";
  receiver = lower_case( target );
  who = lower_case( who );
// NB this is excessive.  Someone could probably remove unnecessary
// case changing from the whole tell system once is has settled down.
  ob = find_player( receiver );
  target = capitalize( target );
  if( !ob ) return target + NOT_HERE_MESS;
  sender = find_player( who );
  if( mud ) priority_sender = 0;
  else if( member_group( who, "admin" ) ) priority_sender = 2;
  else if( sender && wizardp( sender ) ) priority_sender = 1;
  else priority_sender = 0;
  priority_receiver = wizardp( ob );
  if( member_group( receiver, "admin" ) ) priority_receiver = 2;
  if( sender ) invis_sender = sender-> query( "invisible" );
  else invis_sender = 0;
  invis_receiver = ob-> query( "invisible" );
  log( "tell_user : ps = " + priority_sender + " is = " + invis_sender +
    " pr = " + priority_receiver + " ir = " + invis_receiver );
  if( invis_receiver> priority_sender )
    return target + NOT_HERE_MESS;
  if( ( priority_sender < 2 ) && hiddenp( ob ) )
    return target + NOT_HERE_MESS;
  if( !interactive( ob ) ) return target + " is net-dead.";
  if( priority_sender  <= priority_receiver )
  {
    if( ob-> query( "busy" ) )
      return target + " is busy. Try later.";
    ignore = ob-> query( "ignore" );
    if( pointerp( ignore ) )
    {
      if(  - 1 != member_array( "@" + mud, ignore ) )
        return target + " is ignoring everyone@" + capitalize( mud );
      if(  - 1 != member_array( who, ignore ) ||
        - 1 != member_array( who + "@" + mud, ignore ) )
        return target + " is ignoring you.";
    }
  }
  if( invis_sender> priority_receiver )
    who = ( priority_sender ? "Someone" : "Somebody" );
// Different anon title for mortals stops impersonation.
  log( "tell_user : tell received from " + who );
  if(GIVE_SIGNAL && ob->query("inactive"))   // Give beep to signal inactive
    tell_object( ob, "");
  tell_object( ob, wrap( capitalize( who ) +
    ( mud ?( "@" + capitalize( mud ) ): "" ) + " tells you: " + msg ) );
  ret = "You tell " + target + ": " + msg;
  if( ob->query("inactive") )
    ret = target + " is presently inactive.\n" + ret;
  else if( query_idle( ob ) > IDLE_TIME )
    ret = target + " has been idle for " + format_time( query_idle( ob ) ) +
      ".\n" + ret;
  if( invis_sender  <= priority_receiver )
    ob-> set( "reply", who + ( mud ?( "@" + mud ): "" ) );
  return ret;
}

/*
  remote_tell() is called by cmd_tell() in _tell.c
                      and by cmd_reply() in _reply.c
*/
string remote_tell( object source, string user, string mud, string msg )
{
  int id;
  
  mud = lower_case( mud );
  id = INETD-> open_service( mud, "tell" );
  if( id < 0 )
    return "Remote mud does not exist.\n";
  requests[id] = ({ source, user, mud, msg });
  return "Trying " + capitalize( mud ) + " ...\n";
}

inter_channel( string who, string channel, string msg )
{
  int i, s, id;

  s = sizeof( muds );
  for( i = 0 ; i < s ; i ++ )
  {
    id = INETD-> open_service( muds[i], "tell" );
    if( id >= 0 ) requests[id] = ({ who, channel, muds[i], msg });
  }
}
