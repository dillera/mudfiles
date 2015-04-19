/*
 * 92/93	Zellski
 * 930627	Carlsson speeded up plural_word and made it better.
 * 930822	Dworkin	added query_desc() handling.
 * 931213	Carlsson put plural_word in an external handler.
 * 940114	Carlsson put nums and vowels in an external handler.
 */

inherit "lib/language";

string
count(int num)
{
    if (num < 10)
	return int_to_string(num);
    if (num < 20)
	return "some";
    return "lots of";
}

string
composite_line(object *arr)
{
    int i;
    int art;
    mapping folders;
    object ob;
    mixed *num;
    mixed *dat;
    string *str;
    string line;
    string id;
    string desc;
    folders = ([ ]);
    for (i = 0; i < sizeof(arr); i ++) {
	ob = arr[i];
	id = ob->short();
	if (!id)
	    continue;
	desc = ob->query_desc();
	if (desc)
	    id = desc;
	else if ((living(ob) && !ob->query_property("COMPOSITE_SHORT") &&
	  ob->query_name()) || ob->query_property("COMPOSITE_NAME")) {
	    id = ob->query_name();
	    art = 1;
	}
	dat = folders[id];
	if (!dat)
	    dat = ({ 1, 0, 0 });
	else
	    dat[0] ++;
	if (ob->plural_short())
	    dat[1] = ob->plural_short();
	dat[2] |= art | ob->query_property("PLURAL");
	folders[id] = dat;
    }
    str = m_indices(folders); num = m_values(folders);
    for (i = 0; i < sizeof(str); i ++) {
	id = lower_case(str[i]);
	art = (num[i][2] || sscanf(id, "a %s", id) ||
	       sscanf(id, "the %s", id) || sscanf(id, "an %s", id) ||
	       sscanf(id, "some %s", id) || sscanf(id, "%s%d", id, id));

	if (num[i][0] == 1) {
	    if (!art)
		str[i] = addart(str[i]);
	} else {
	    str[i] = num[i][1] ? num[i][1] : pluralize(id);
	    str[i] = count(num[i][0]) + " " + str[i];
	}
    }
    if (sizeof(str) > 1)
	line = implode(str[1..sizeof(str) - 1], ", ") + " and " + str[0] +
	       " are here.\n";
    else if (sizeof(str))
	line = (str[0] + " " + ((num[0][0] == 1 &&
	       !ob->query_property("PLURAL")) ? "is" : "are") + " here.\n");
    else
	return "";
    return break_string(capitalize(line), 70);
}


void
reset(int arg)
{
}
