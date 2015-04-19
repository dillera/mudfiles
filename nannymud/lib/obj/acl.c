/*
** acl.c
**
** Some ACL controlling commands
*/

reset(arg)
{
    if (arg)
	return;
}


short()
{
    return "a big brown sphere";
}


long()
{
    write("A big brown sphere.\n");
    if (this_player()->query_level() > 20)
    {
	write("The following commands are available:\n\n");
	write("\tlac [<path>]\n");
	write("\tsac <path] [-like <path>] [-nq | -no_query]\n");
	write("\tedac <path> <object>:<rights> [.. <object>:<rights>]\n");
    }
}

init()
{
    add_action("lac", "lac");
    add_action("sac", "sac");
    add_action("edac", "edac");
}

static acl_wren(aclen)
{
    write("\t" + aclen[0] + ":\t");
    if (strlen(aclen[0]) < 8)
	write("\t");
    write("" + _acl_num2str(aclen[1]) + "\n");
}

static acl_print(ael)
{
    int i;

    for (i = 0; i < sizeof(ael); i++)
	acl_wren(ael[i]);
}

lac(path)
{
    mixed *acl;
    int show_rest;


    if (path == 0)
	path = "/players/" + this_player()->query_name();
    
    show_rest = 0;
    acl = _acl_get(path);

    if (acl != 0)
    {
	if (stringp(acl[0]))
	    write("\"" + path +
		  "\" protected by default ACL (from \"" +
		  acl[0] + "\"):\n");
	else
	    write("ACL protecting \"" + path + "\":\n");

	acl_print(acl[2]);
	write("\t$REST:\t\t" + _acl_num2str(acl[1]) + "\n");
    }
    else
	show_rest = 1;
    
    acl = _acl_get("");
    if (acl != 0)
    {
	write("\nPriority ACL in effect for \"" + path + "\":\n");
	acl_print(acl[2]);
	if (show_rest)
	    write("\t$REST:\t\t" + _acl_num2str(acl[1]) + "\n");
    }

    return 1;
}

sac(arg)
{
    write("Sorry. Not yet implemented.\n");
    
    return 1;
}

edac(arg)
{
    write("Sorry. Not yet implemented.\n");
    
    return 1;
}
