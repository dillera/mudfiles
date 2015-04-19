/* timed.c
 *
 * The local NannyMUD time.
 * 
 *-------------------------------------------
 * First version by Brom, June 27 -95
 *
 * New Concept, Brom and Taren, June 28 -95
 *
 * The idea is simply to have the 'normal' day converted in to small
 * 'Nannydays' each of these days 7 real hours, with 2 hours of night.
 * A Nannyweek is 7 Nannydays.
 * A Nannymonth is 21 Nannydays.
 * A Nanny Year is 13 Nannymonths.
 * A Mooncycle is completed in 8 Nannydays.
 * Years have their own names, wich are repeated in cycles of 17 years.
 * 
 * The start for for the Nannytime is when Nanny was first started,
 * sometime in April 1990 (??, Ask Mats for more exact date ??).
 * Mats said in the late part of April, he didn't really remember.
 * Each year has its own aspect, like "the year of the Daemon".
 * All names above is subject to immediate change...
 * Soo why do we bother with this?
 * Well, it would add a new dimension to the game, and thus make it
 * more fun to play it. The idea is to let certain powers be better
 * at certain times, a classic example is of course were-creatures.
 * Add a simul efun _ctime(int time) ? This simul efun will of course
 * use the NannyTime counting (TM).
 * You should also be able to ask the timed for information, like
 * is it full-moon? What day is it? 
 * Well, in conclusion it will surely add a new dimension to the game.
 *
 * Second version Made by Taren, July 01 1995. 
 *
 * Additional code by Rand, September 15, 1995 ++
 *
 *
 * Most common usage might be to check if it is night, this can be done with:
 * "/obj/daemon/timed"->_part_of_day(time(),1,1)=="night"
 */

#define PLURAL(a) ((a==1)?" ":"s ")

#define NOCONV  0
#define CONVERT 1
#define NOSTR   0
#define STRING  1

#define START 640130400    /* ctime(START) == Sun Apr 15 00:00:00 1990 */

#define MINUTE 23
#define HOUR   (MINUTE*58) /* The number of seconds on a NannyHour     */
#define DAY    (HOUR*19)   /* The number of seconds a nannyday is      */
#define DAYS_IN_MONTH 21
#define MONTH  (DAY*DAYS_IN_MONTH)
#define MONTHS_IN_YEAR 13
#define YEAR   (MONTH*MONTHS_IN_YEAR)
#define CYCLE  17          /* Cycle length of Nanny year-names         */

/* -------------------------------------------------- RLTime defines.  */
/* The above makes a Nannyyear 23*58*19*21*13=6919458 seconds.
   A Nannyyear is made in 0.21926439 RL years.
   A RL year equals 4.560704 Nannyyears. 
*/

#define RLDAY      86400
#define RLMONTH  2629800
#define RLYEAR  31557600

/* Mooncycles :

   1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
   F 3 H 1 E 1 H 3 F  3  H  1  E  1  H  3  F  3  H  1  E  1  H  3  F  3  H  1
   0 1 2 3 4 5 6 7 0  1  2  3  4  5  6  7    
  F == Full, H == Half, E == Empty 1= 1 Quarter full, 3 = 3 Quarters full

  By define is : Day One is a Full moon night.

*/

void reset(int arg)
{
  if(_isclone(this_object())) 
    return destruct(this_object());
  move_object(this_object(), "/obj/daemon/daemonroom");
}

init()
{
  add_action("nanny_time2","nt");  
}

/*
   Standard things...
 */

string short() { return "a time daemon"; }

int id(string s) { return s=="timed" || s=="time daemon" || s==short(); }

void long() 
{
  write("The time stands still as you and the time daemon lock eyes.\n");
}

int _time() { return(time()-START); }

string _name_of_year(int i)
{
  i%=CYCLE;

  switch(i)
  {
  case 0:
    return "The Year of Creation";
  case 1:
    return "The Year of the Daemon";
  case 2:
    return "The Year of the Wind";
  case 3:
    return "The Year of the Dragon";
  case 4:
    return "The Year of the Giants";
  case 5:
    return "The Year of the Elves";
  case 6:
    return "The Year of Magic";
  case 7:
    return "The Year of Humans";
  case 8:
    return "The Year of the Golden Age";
  case 9:
    return "The Year of Sacrilege";
  case 10:
    return "The Year of Darkness";
  case 11:
    return "The Year of Blood";
  case 12:
    return "The Year of Sacrifice";
  case 13:
    return "The Year of Purification";
  case 14:
    return "The Year of Light";
  case 15:
    return "The Year of Balance";
  case 16:
    return "The Year of Law";
  default:
    return "Year-error";
  }
}

string _name_of_month(int i)
{
  switch(i)
  {
  case 1:
    return "The Month of Darkness";
  case 2:
    return "The Month of Truth";
  case 3:
    return "The Month of Wind";
  case 4:
    return "The Month of Water";
  case 5:
    return "The Month of Spirit";
  case 6:
    return "The Month of Fertility";
  case 7:
    return "The Month of Plant";
  case 8:
    return "The Month of Earth";
  case 9:
    return "The Month of Harmony";
  case 10:
    return "The Month of Fire";
  case 11:
    return "The Month of Illusion";
  case 12:
    return "The Month of Law";
  case 13:
    return "The Month of Demons";
  default:
    return "Month_error";
  }
}

string _name_of_day(int day)
{
  switch(day)
  {
  case 1:
    return "Larsday";
  case 2:
    return "Q'sday";
  case 3:
    return "Padsday";
  case 4:
    return "Orsday";
  case 5:
    return "Harday";
  case 6:
    return "Leoday";
  case 7:
    return "Matsday";
  default:
    return "Day_error";
  }
}

/* _year
   flag == convert to Nanny Time
   give_str == Give a string instead of a number
*/
mixed _year(int i, int flag, int give_str)
{
  if(flag)
    i-=START;

  i/=YEAR;
  if(give_str) return(_name_of_year(i));
  else return(i);
}

/* _month
   flag == convert to Nanny Time
   give_str == Give a string instead of a number
*/
mixed _month(int i, int flag, int give_str)
{
  if(flag)
    i-=START;

  i%=YEAR;
  i/=MONTH;
  i++;
  if(give_str) return(_name_of_month(i));
  else return(i);
}

string query_season()
{
/*
   Winter 11,12,0,1
   Spring 2,3,4
   Summer 5,6,7
   Fall   8,9,10   
 */
  switch(_month(time(),1,0))
  {
  case 2..4: return "spring";
  case 5..7: return "summer";
  case 8..10: return "fall";
  default:
    return "winter";
  }
}

mixed _time_of_day(int time, int flag, int give_str)
{
  int hh, mm;

  if(flag) time-=START;
  time%=YEAR;
  time%=MONTH;
  time%=DAY;
  hh=time/HOUR;
  time%=HOUR;
  mm=time/MINUTE;
  time%=MINUTE;
  if(give_str) return(sprintf("%d:%'0'2d:%'0'2d", hh, mm, time));
  else return(({hh, mm, time}));
}

string _part_of_day(int time, int flag, int give_str)
{
  int t;

  t=_time_of_day(time, flag, 0)[0];
  if(!give_str) return(t/2)+1;
  switch(t)
  {
  case 16..18 :
  case 0..2 :
    return("night");
    break;
  case 3..5 :
    return("morning");
    break;
  case 6..8 :
    return("day");
    break;
  case 9:
    return("noon");
    break;
  case 10..12 :
    return("afternoon");
    break;
  case 13..15 :
    return "evening";
    break;
  default:
    return "DayPartError";
    break;
  }
}


/*  _mday(int i, int flag, int give_str)
    returns the day of month (1..MONTH) 

    i        == seconds
    flag     == convert to Nanny Time
*/
mixed _mday(int i, int flag)
{
  if(flag)
    i-=START;

  i%=YEAR;
  i%=MONTH;
  i/=DAY;
  return (i+1);
}

/*
   _phase(int time, int flag, int give_str)
   time == An integer with NannyTime
   flag == Convert from 'real' time to NannyTime
   give_str == Give a string with the phase instead of a number.

 */
mixed _phase(int i, int flag, int give_str)
{
  int year,month,day;

  if(flag)
    i-=START;
  year=i/YEAR;
  i%=YEAR;
  month=i/MONTH;
  i%=MONTH;
  day=i/DAY;
  i%=DAY;
  
  if(give_str)
  {
    switch(((month-1)*28+day)%8)
    {
    case 0:
    default:
      return "Full moon";
    case 1:
      return "Three quarters full moon, waning";
    case 2:
      return "Half moon, waning";
    case 3:
      return "Quarter full moon, waning";
    case 4:
      return "Empty moon";
    case 5:
      return "Quarter full moon, waxing";
    case 6:
      return "Half moon, waxing";
    case 7:
      return "Three quarters full moon, waxing";
    }      
  }

  return ((month-1)*28+day)%8;
}

/*
   _weekday(int i,int flag,int str)
   flag == convert to Nanny Time
   str == Give a string instead of a number
*/

mixed _weekday(int i, int flag, int str)
{
  int day;

  day=((_mday(i, flag)-1)%7) +1;

  if(str)
    return(_name_of_day(day));
  else
    return(day);
}

/*  _dtime(int time,int flag)
    relative time-conversion
    time == An integer with NannyTime
*/
string _dtime(int i)
{
  int yy, mo, dd, hh, mm;

  yy=i/YEAR;
  i%=YEAR;
  mo=i/MONTH;
  i%=MONTH;
  dd=i/DAY;
  i%=DAY;
  hh=i/HOUR;
  i%=HOUR;
  mm=i/MINUTE;
  i%=MINUTE;
  if(yy) return("" + yy + " year" + PLURAL(yy) + mo + " month" + PLURAL(mo) +
		dd + " day" + PLURAL(dd) + hh + " hour" + PLURAL(hh) +
		mm + " minute" + PLURAL(mm) + i + " second" + PLURAL(i));
  else if(mo) return("" + mo + " month" + PLURAL(mo) +
		     dd + " day" + PLURAL(dd) + hh + " hour" + PLURAL(hh) +
		     mm + " minute" + PLURAL(mm) + i + " second" + PLURAL(i));

  else if(dd) return("" + dd + " day" + PLURAL(dd) + hh + " hour" + 
		     PLURAL(hh) + mm + " minute" + PLURAL(mm) + i + 
		     " second" + PLURAL(i));

  else if(hh) return("" + hh + " hour" + PLURAL(hh) +
		mm + " minute" + PLURAL(mm) + i + " second" + PLURAL(i));

  else if(mm) return("" + mm + " minute" + PLURAL(mm) + i + " second" + 
		     PLURAL(i));

  else return("" + i + " second" + PLURAL(i));
}

/*  dtime(int time)
    relative time-conversion (for RL-time)
    time == An integer with RealLifeTime
*/
string dtime(int i)
{
  int yy, mo, dd, hh, mm;

  yy=i/RLYEAR;
  i%=RLYEAR;
  mo=i/RLMONTH;
  i%=RLMONTH;
  dd=i/RLDAY;
  i%=RLDAY;
  hh=i/3600;
  i%=3600;
  mm=i/60;
  i%=60;
  if(yy) return("" + yy + " year" + PLURAL(yy) + mo + " month" + PLURAL(mo) +
		dd + " day" + PLURAL(dd) + hh + " hour" + PLURAL(hh) +
		mm + " minute" + PLURAL(mm) + i + " second" + PLURAL(i));
  else if(mo) return("" + mo + " month" + PLURAL(mo) +
		     dd + " day" + PLURAL(dd) + hh + " hour" + PLURAL(hh) +
		     mm + " minute" + PLURAL(mm) + i + " second" + PLURAL(i));

  else if(dd) return("" + dd + " day" + PLURAL(dd) + hh + " hour" + 
		     PLURAL(hh) + mm + " minute" + PLURAL(mm) + i + 
		     " second" + PLURAL(i));

  else if(hh) return("" + hh + " hour" + PLURAL(hh) +
		mm + " minute" + PLURAL(mm) + i + " second" + PLURAL(i));

  else if(mm) return("" + mm + " minute" + PLURAL(mm) + i + " second" + 
		     PLURAL(i));

  else return("" + i + " second" + PLURAL(i));
}

/*  _ctime(int time)
    time == An integer with NannyTime
*/
string _ctime(int i)
{
  int year, day, hour, minute;
  string weekd;

  weekd=_weekday(i, NOCONV, STRING);
  day=_mday(i, NOCONV);
  year=i/YEAR;
  i%=YEAR;
  i%=MONTH;
  i%=DAY;
  hour=i/HOUR;
  i%=HOUR;
  minute=i/MINUTE;
  i%=MINUTE;
  return("" + weekd + " " + _month(_time(),NOCONV, STRING) + " " + 
	 (day<10?"0"+day:day) + "  " + hour + ":" + (minute<10?"0":"") + 
	 minute + ":" + (i<10?"0":"") + i + " " + _name_of_year(year));
}

void nanny_time(int i, int flag)
{
  int t;

  if(flag)
    i-=START;

  t=i/YEAR;
  i%=YEAR;
  write(t+" year"+(t!=1?"s":""));  
  t=i/MONTH;;
  i%=MONTH;
  write(" "+t+" month"+(t!=1?"s":""));  
  t=i/DAY;
  i%=DAY;
  write(" "+t+" day"+(t!=1?"s":""));
  t=i/HOUR;
  i%=HOUR;
  write(" "+t+" hour"+(t!=1?"s":""));
  t=i/MINUTE;
  i%=MINUTE;
  write(" "+t+" minute"+(t!=1?"s":""));  
  write(" "+i+" second"+(i!=1?"s":"")+".\n");
}

int nanny_time2()
{
  write(ctime(time())+"\n");
  write(_ctime(_time())+"\n");
  nanny_time(_time(), NOCONV);
  write("Phase : "+_phase(_time(), NOCONV, STRING)+"\n");
  return 1;
}
int query_prevent_shadow() { return 1; }
