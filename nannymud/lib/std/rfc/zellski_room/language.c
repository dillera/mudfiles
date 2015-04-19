/*
 * Written, conceived, conducted and implemented by Anarchrist in October/
 * November 1993 sometime.
 *
 * Goal: To make composite less cluttered as well as to make a source for some
 *       handy functions.
 *
 * Parts of composite.c, written by Zellski, Carlsson and me mostly moved into
 * this file as well.
 *
 */

#include "/include/limits.h"
inherit "lib/nouns";

/*
 * Input:  An integer.
 * Output: The corresponding English word. (1 will return "one", 1347 will
 *         return "onethousandthreehundredfortyseven".)
 */

string
int_to_string(int n)
{
    if (!intp(n))
	return 0;
    switch (n) {
	case 0:
	    return "zero";
	case 1:
	    return "one";
	case 2:
	    return "two";
	case 3:
	    return "three";
	case 4:
	    return "four";
	case 5:
	    return "five";
	case 6:
	    return "six";
	case 7:
	    return "seven";
	case 8:
	    return "eight";
	case 9:
	    return "nine";
	case 10:
	    return "ten";
	case 11:
	    return "eleven";
	case 12:
	    return "twelve";
	case 13:
	    return "thirteen";
	case 14:
	    return "fourteen";
	case 15:
	    return "fifteen";
	case 16:
	    return "sixteen";
	case 17:
	    return "seventeen";
	case 18:
	    return "eighteen";
	case 19:
	    return "nineteen";
	case 20:
	    return "twenty";
	case 30:
	    return "thirty";
	case 40:
	    return "forty";
	case 50:
	    return "fifty";
	case 60:
	    return "sixty";
	case 70:
	    return "seventy";
	case 80:
	    return "eighty";
	case 90:
	    return "ninety";
	case 100:
	    return "onehundred";
	case 1000:
	    return "onethousand";
	case 1000000:
	    return "onemillion";
	case 1000000000:
	    return "onemilliard";
	default:
	    switch (n) {
		case 21 .. 99:
		    return int_to_string((n / 10) * 10) + int_to_string(n % 10);
		case 101 .. 999:
		    return int_to_string(n / 100) + "hundred" +
			   int_to_string(n % 100);
	        case 1001 .. 999999:
		    return int_to_string(n / 1000) + "thousand" +
			   int_to_string(n % 1000);
	        case 1000001 .. 999999999:
		    return int_to_string(n / 1000000) + "million" +
			   int_to_string(n % 1000000);
	        case 1000000001 .. INT_MAX:
		    return int_to_string(n / 1000000000) + "milliard" +
			   int_to_string(n % 1000000000);
	}
    }
}

/*
 * Input:  An integer between 0 - 30.
 * Output: The corresponding analog clock minute string (1 - one, 15 - quarter,
 *         30 - half).
 */

string
str_minutes(int n)
{
    switch (n) {
      case 1 .. 14:
      case 16 .. 29:
	return int_to_string(n);
	break;
      case 15:
	return "quarter";
	break;
      default:
	return "";
    }
}

varargs string
time_to_string(int t)
{
    int hour;
    int minute;
    string time_string;
    if (!t)
	t = time();
    t %= 86400;
    hour = t / 3600 + 1;
    minute = (t % 3600) / 60;
    switch (minute) {
	case 2 .. 29:
	    time_string = int_to_string(minute) + " minutes past ";
	    break;
	case 31 .. 59:
	    time_string = int_to_string(60 - minute) + " minutes to ";
	    hour ++;
	    break;
	case 30:
	    time_string = "half past ";
	    break;
    }
    return time_string + int_to_string(hour % 12);
}

/*
 * Return true if char is a vowel (when in first letter of word).
 */
int
vowels(int c)
{
    switch (c) {
        case 'e':
        case 'E':
        case 'a':
        case 'A':
        case 'i':
        case 'I':
        case 'o':
        case 'O':
        case 'u':
        case 'U':
            return 1;
        default:
            return 0;
    }
}

/*
 * Add article to a noun.
 */
string
addart(string noun)
{
    return "a" + (vowels(noun[0]) ? "n" : "") + " " + noun;
}

/*
 * Pluralise a noun.
 */
string
pluralize(string sent)
{
    string *words;
    int i;
    words = explode(sent, " ");
    if (sizeof(words) == 1)
        return plural_word(words[0]);
    i = member_array("of", words);
    if (i > 0)
        words[i - 1] = plural_word(words[i - 1]);
    else
        words[sizeof(words) - 1] = plural_word(words[sizeof(words) - 1]);
    return implode(words, " ");
}
