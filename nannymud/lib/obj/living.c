/* Most of living functionality moved to /obj/living_functions to
 * facilitate the objects in /std/   
 *  /Profezzorn
 */
/*
 * If you are going to copy this file, in the purpose of changing
 * it a little to your own need, beware:
 *
 * First try one of the following:
 *
 * 1. Do clone_object(), and then configure it. This object is specially
 *    prepared for configuration.
 *
 * 2. If you still is not pleased with that, create a new empty
 *    object, and make an inheritance of this objet on the first line.
 *    This will automatically copy all variables and functions from the
 *    original object. Then, add the functions you want to change. The
 *    original function can still be accessed with '::' prepended on the name.
 *
 * The maintainer of this LPmud might become sad with you if you fail
 * to do any of the above. Ask other wizards if you are doubtful.
 *
 * The reason of this, is that the above saves a lot of memory.
 */

/*
 * Inherit this file in objects that "lives".
 * The following variables are defined here:
 * .......
 */

inherit "/obj/living_functions";
inherit "/obj/property";

string name;  /* The name of this object */
static int is_npc;

string query_real_name() { return name; }
int query_npc() { return is_npc;}
