inherit "/std/fun_call";

mapping properties;

void
std_properties_reset(int arg)
{
    if (!arg)
	properties = ([ ]);
}

void
create()
{
    std_properties_reset(0);
}
 
void
reset(int arg)
{
    std_properties_reset(arg);
    return;
}

void
set_property(mixed prop, mixed value)
{
    if (properties)
	properties[prop] = value;
    else
	properties = ([ prop : value ]);
}

void
add_property(mixed prop, mixed value)
{
    set_property(prop, value);
}

void
remove_property(mixed prop)
{
    if (properties)
	properties = m_delete(properties, prop);
}

mixed
query_property(mixed prop)
{
    return properties ? fun_call(properties[prop]) : 0;
}

mapping
query_properties()
{
    if (properties)
	return properties + ([ ]);
    else
	return ([ ]);
}
