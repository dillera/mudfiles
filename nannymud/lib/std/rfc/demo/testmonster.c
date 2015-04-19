/** Something that looks like harry .... **/
/** Written by Profezzorn **/

#include "/std/rfc/monster_setup.h"
inherit "/std/rfc/monster";

void reset(int arg)
{
  ::reset(arg);

  if(!arg)
  {
    set_name("harry");
    add_alias("fjant");
    set_gender(1);
    set_short("Harry the affectionate");
    set_long("Harry has an agreeable look.\n");
    set_level(3);
    set_al(50);
    set_ep(2083);	/* Not needed - it's set by set_level */
    set_ac(2);
    set_wc(5);
    SET_RESPONSES(this_object()) {
      DELAY(1) {
	MATCH("%s left the game.\n") SAY("Why did $1 quit the game?");

	MATCH("%s %s %s.\n\b$2") {
	  MATCH("sells") SAY("Why did you sell $3.-2?");
	  MATCH("attacks") SAY("Why does $1.-2 attack $3.-2\n?");
	  MATCH("takes") SAY("Why did $1.-2 take $3.-2\n?");
	  MATCH("drops") SAY("Why did $1.-2 drop $3.-2\n?");

	  MATCH("sings") {
	    RANDOM(50) {
	      SAY("You really have a nice voice!");
	      BREAK;
	    }
	    RANDOM(50) {
	      EMOTE("tries to sing too.");
	      BREAK;
	    }

	    RANDOM(50) {
	      COMMAND("applaud");
	      BREAK;
	    }
	    BREAK;
	  }

	  MATCH("prays") COMMAND("pray");

	  MATCH("smiles\b$3.-2")
	    MATCH("happily")
	      COMMAND("smile");

	  BREAK;
	}

	MATCH("%s falls down laughing.\n") MSG("\bPRON looks at \b2OBJ.\n");

      }
    }

  }
}
