inherit "/std/properties";


#undef private 
/* I removed the stuff. I guess we'll find out quite soon what screws
 * out... Anarchrist.
 */
/* (quick hack to get things working again. Someone fix those rooms! -- Dark */

private static string short_desc, long_desc;
private static mapping items;

string str(string s) { return s ? s : ""; } /* DON'T TOUCH */

void
create()
{
    ::create();
    if (!items)
	items = ([ ]);
    short_desc = "Something";
    long_desc = "You see a vague nothing?\n";
}
 
void
reset(int arg)
{
    ::reset(arg);
    if (!arg && !items)
	items = ([ ]);
}

void add_item(mixed item, string desc) {
   if (pointerp(item)) {
	int i;
 
        for (i = sizeof(item); i > 0;) {
	    items[item[--i]] = desc;
	}
    } else {
        items[item] = desc;
    }
}

void remove_item(mixed item) {
    if (pointerp(item)) {
        int i;
 
        for (i = sizeof(item); i > 0;) {
            items = m_delete(items, item[--i]);
        }
    } else {
        items = m_delete(items, item);
    }
}

mapping query_items_mapping() { return items ? items + ([ ]) : ([ ]); }

string query_item(string str) { return items[str]; }

void set_short(string s) { short_desc = s; }

void set_long(string l)  { long_desc  = l; }

varargs
string query_long(string arg) {
    if (!arg) {
	return (long_desc) ? fun_call(long_desc) : "You see nothing.\n";
    }
    return str(fun_call(items[arg], arg));
}

string query_short() {  return short_desc; }

varargs
void long(string str) { write(query_long(str)); }

string short()        { return query_short(); }

int id(string str) {
    return items[str] != 0;
}

varargs string
query_long_desc(string str)
{
    return query_long(str);
}
