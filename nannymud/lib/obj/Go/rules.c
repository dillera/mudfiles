/* obj/Go/rules.c
 * patched by MeTa 1991-04-25
 * Now uses an array instead of five different rulefiles.
 * Part one of MAJOR rewriting of standard MUD files.
 */

string rules;

id(str) {
    return str == "Go rules" || str == "rules";
}

short() {
    return "Go rules";
}

long() {
    write("A list of the rules for playing Go.\n");
    write("There are 5 rules.\n");
    write("Do 'rule #', to read a rule number.\n");
}

get() {
    write("The rules are attached to the floor!\n");
    return 0;
}

init() {
    add_action("rule", "rule");
}

rule(str) {
    int n;

    /* Shouldn't theese be using || ? */
    if(!str) return 0;
    if (sscanf(str, "%d", n) != 1) return 0;
    if (n < 1 || n > sizeof(rules)) {
	write("Not that many rules.\n");
	return 1;
    }
    say(this_player()->query_name() + " reads rule " + n + "\n");
    write(rules[n-1]);
    return 1;
}

reset(str)
{
    if (str) return;
    /* First reset, initialize the array with the rules. */
    rules=({
"There are two players, black and white. They take turns, playing one stone\n"+
"each. Black starts with one or more stones, depending on the handicap.\n\n"+
"The stones are played in a matrix on the intersections. It is allowed to\n"+
"play on the outer border.\n\n"+
"The normal game size is 19 x 19 (that is 18 x 18 squares), but the game\n"+
"can also be played on smaller boards.\n"
,
"The winner, is the player with the highest score on the board when\n"+
"the game ends.\n\n"+
"The game ends when both players have passed.\n\n"+
"The score is the number of surrounded empty intersections.\n"
,
"A group is defined as one or more stones of the same colour, that are\n"+
"connected along the lines. Stones beside each other are connected,\n"+
"but stones positioned diagonally from each other are not.\n\n"+
"A liberty is an empty space beside a stone. Again, an empty space\n"+
"diagonally from a stone does not count.\n\n"+
"The liberties of a group are the number of liberties that a group have.\n\n"+
"Liberties can be shared by more than one group.\n"
,
"If a stone is played so that the last liberty of an enemy group is\n"+
"removed, then all stones in that enemy group are removed.\n\n"+
"If a stone is played so that the last liberty of a group of the same\n"+
"colour is removed, then it is an illegal move.\n"
,
"If the position of the board is a repeat of an earlier position, then the\n"+
"last move was illegal.\n"
});
}
