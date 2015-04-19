varargs mixed
fun_call(mixed str, mixed extra)
{
    if (str && stringp(str) && strlen(str) > 1) {
	switch(str[0]) {
	    case '#':
		return call_other(this_object(), str[1 .. ], extra);
	    case ':': {
		string *strv;
		strv = explode(str, ":");
		str = call_other(strv[1], strv[0], extra);
	    }
	}
    }
    return str;
}
