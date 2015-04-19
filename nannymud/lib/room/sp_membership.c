/* sp_membership.c -- a Secret Police membership */

string owners_name;
object owner;
status temporary;

id(str) {
    return    str == "membership"
	   || str == "sp_membership"
	   || str == "secret police membership"
	   || str == "Secret Police membership";
}

reset(arg) {
    if (!arg) {
	temporary = 0;
	owners_name = "None";
    }
}

init() {
    add_action("do_eat", "eat");
}

do_eat(str) {
    if (!id(str))
	return 0;
    say(this_player()->query_name() + " tried to eat " +
	this_player()->query_possessive() + " secret police membership.\n");
    write("You try to eat your secret police membership - yuck!\n");
    call_out("destruct_me", 1);
    return 1;
}

long() {
    write("It's a small badge with the text\n" +
	  "\"Secret Federal Imperial Bureau of Domestic Investigations\".\n");
    write("It has the name \"" + owners_name + "\" engraved on it.\n");
    if (temporary)
	write("It is marked \"TEMPORARY\".\n");
}

short() {
    if (temporary)
	return("A temporary Secret Police membership");
    else
	return("A Secret Police membership");
}

/* It can not be dropped. */
drop() {
    if (temporary) {
	call_out("destruct_me", 1);
	return 0;
    }
    else {
	write("The secret police membership cannot be dropped.\n");
	write("If you want to get rid of the membership, you must eat it!\n");
	return 1;
    }
}

destruct_me() {
    write(owners_name + "'s Secret Police membership turns to ashes.\n");
    say(owners_name + "'s Secret Police membership turns to ashes.\n");
    destruct(this_object());
}

set_temporary(t) {
    temporary = t;
}

set_name(n) {
    owners_name = capitalize(n);
    owner = find_living(lower_case(n));
} /* set_name */

/* It is auto-loading! */
query_auto_load() {
    object env;
    env = environment(this_object());
    if (!env || !living(env) || env->is_npc() || temporary)
	return 0;
    else
	return "players/padrone/sp_membership" + ":" + owners_name;
} /* query_auto_load */

/* Set the variable "owners_name" from the auto-load. */
init_arg(n) {
    owners_name = n;
} /* init_arg */

get() {
    return 1;
}

query_value() {
    return 1;
}
