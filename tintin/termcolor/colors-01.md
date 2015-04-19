Terminal Colors
================

- http://www.termsys.demon.co.uk/vtansi.htm

## Character Encoding

Standards:

| Standard          | Bits                  | Standard      | Set |
| ----------------- | --------------------- | ------------- | --- |
| ASCII (US-ASCII)  | Fixed Length, 7 bits  | ISO/IEC 646   | 128 |
| ASCII Extended    | Fixed Length, 8 bits  | ISO 8859-1    | 256 | 
| Unicode           | Variable Length       | ISO/IEC 10646 | *   |

Historically, ASCII was created to work with the English language on 8-bit systems. Because it was only intended to work
with the English language, only 26 lowercase characters, 26 uppercase characters, numbers 0-9, and about 11 characters
for puncutation were included. This required a set space of 96 characters. This leaves 32 values in the 7-bit set for
encoding control characters.

ASCII was subsequently extended with multiple standards that included the use of the eighth bit to include an additional
128 characters for use with languages other than English. These extensions are backwards compatible with US-ASCII.
Some examples of ASCII extended standards are: Latin-1, Latin-2, Latin-3, Latin-4, Latin-5, Latin-6, Latin-7, Latin-8,
Latin-9, Latin/Cryllic, Latin/Arabic, Latin/Greek, Latin/Hebrew, Latin/Turkish, & Latin/Thai.

While the low-order bits (0-128) in ASCII extended character sets remain the same (and therefore backwards compatible
with US-ASCII), the high-order values (127-255) are rendered according to the operating systems currently active
character (encoding) set. This created significant problems for multi-language systems & users.

- The first 32 ASCII codes (0-31) are reserved for non-printable characters.
- Codes 40-126 are used for printable characters.
- The last code (127) is used for the delete character.
- Characters are generally encoded in ASCII 

Note: How is UTF-8 encoded? How does backward compatibility work?


**References:**
- [ISO/IEC_646](https://en.wikipedia.org/wiki/ISO/IEC_646)
- [Extended ASCII](https://en.wikipedia.org/wiki/Extended_ASCII)
- [ASCII](https://en.wikipedia.org/wiki/ASCII)


### Control Characters 
The original ASCII standard included 32 control characters (0..31) but provided no standards for how they should be used
(this was left up to the application). These control characters were defined in the early days of computing when
typewriters and teletype systems were still common, so many of these control characters are no longer commonly used.
Some examples that should be familiar:

- Null (\x00)
- Bell (\x07)
- Tab (\x09)
- Line Feed (\x0A)
- Carriage Return (\x0D)
- Escape (\x1B)

Note: The `Escape` control character has been retrofitted and extended to do all kinds of interesting things in modern
terminals.

The usage and behavior of the control characters (originally specified as part of the ASCII character set) were
subsequently defined as ANSI & ECMA standards, so fortunately their usage & behavior is consistent across most
applications. Still, not all applications behave the same. For example, GNU echo will interpret `\e` as an escape
character, while POSIX echo will not.  When in doubt, the man page is usually a helpful (in this case, `man echo` for
GNU echo):

     ‘\a’       alert (bell)
     ‘\b’       backspace
     ‘\c’       produce no further output
     ‘\e’       escape
     ‘\f’       form feed
     ‘\n’       newline
     ‘\r’       carriage return
     ‘\t’       horizontal tab
     ‘\v’       vertical tab
     ‘\\’       backslash
     ‘\0NNN’    the eight-bit value that is the octal number NNN ...
     ‘\NNN’     the eight-bit value that is the octal number NNN ...
     ‘\xHH’     the eight-bit value that is the hexadeimcal number HH ...

Some examples:

    echo -e "Ring the bell ... \b"
    echo -e "Enter an extra newline\n"
    echo -e "Print some characters, but then return the cusor to the start of the line.\r"
    echo -e "\tPut a tab at the start of this line."

**References:**
- [Code Point](https://en.wikipedia.org/wiki/Code_point)
- [Control character](https://en.wikipedia.org/wiki/Control_character)
- [ECMA-35](http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-035.pdf)
- [ECMA-48](http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-048.pdf)


### The Escape Character
The escape character is a specific ASCII/ANSI/ECMA control character. It's commonly represented by a printable character
sequence of `^[` or `\e`, however the actual (decimal) value of the escape character in the ASCII character set is
**27**. I'm unsure of the origin of the `^[` and `\e` representations. 

Some applications, but not others will interpet the ASCII characters `\e` as an escape character, but others will not.
The use of `\e` is up the application - some applications will interpret this sequence as an escape character (GNU echo
does, but POSIX echo does not, for example)

in other numbering systems as well:

| Numbering System  | Base  | Value | 
| ----------------- | ----- | ----- | 
| Octal             | 8     | 033   | 
| Decimal           | 10    | 27    | 
| Hexidecimal       | 16    | 1B    | 


**References:**  
- [Escape character](https://en.wikipedia.org/wiki/Escape_character)


## Escape Sequences
It's common to see escapes sequences started with the prefixes `\e`, `\033`, and `\x1b`. Which representation is used is
completely application specific, Some applications (such as GNU echo) will interpret `\e` as the start of an escape
sequence, while other applications (BSD's echo command) will not. The iTerm2 terminal emulator (linux) interprets an
escape sequence as `\033` (the octal notation).

The easiest way (for me) to figure this out, was to write some specific examples and then tear them apart bit-by-bit to
figure out the significance of each piece. This is a set of examples that cover a wide range of use: 

    echo -e "\e[31m             \rred/default     (user defined, slash-e escape)"
    echo -e "\033[32m           \rgreen/default   (user defined, octal escape)"
    echo -e "\x1b[31;42m        \rred/green       (user defined, hex escape)"
    echo -e "\x1b[30m           \rblack/green     (bleeding from previous line)"
    echo -e "\x1b[0;34m         \rblue/default    (inline clear)"
    echo -e "\x1b[38;5;9m       \rred/default     (user defined red using xterm index)"
    echo -e "\e[38;5;196m       \rred/default     ('full' red from xterm index)"
    echo -e "\e[38;2;255;0;0;m  \rred/default     ('full' red using rgb)"
    printf "\x1b[0;31m          \rred/default     (with printf)\n"

The examples all follow a basic pattern and be broken down bit by bit.

In all the examples I've seen, we start with a backslash character. 

If you've spent any amount of time writing code for the terminal, these should appear familiar - though to *actually*
understand them, much digging around is required. Each string (from our examples) follows a general pattern:

- `\` (backslash)
- `e`, `033`, `x1b` (escape character representation)
- `[` (Control Sequence Introducer)
- `0`, `32`, `38` (








The [ECMA-48][] ANSI standard defines a very large set of *(modal)* escape sequence operations. For colors, we're only
interested in a subset of this information.


From `man printf`:

    \0num   Write an 8-bit character whose ASCII value is the 1-, 2-, or 3-digit octal
        number num.

    Each format specification is introduced by the percent character (``%'').  The remainder of
        the format specification includes, in the following order:
     
Character escape sequences are in backslash notation as defined in the ANSI X3.159-1989
     (``ANSI C89''), with extensions.  The characters and their meanings are as follows:

    escape-sequence:
        simple-escape-sequence
        octal-escape-sequence
        hexadecimal-escape-sequence
        universal-character-name
    simple-escape-sequence: one of
        \' \" \? \\
        \a \b \f \n \r \t \v
    octal-escape-sequence:
        \ octal-digit
        \ octal-digit octal-digit
        \ octal-digit octal-digit octal-digit
    hexadecimal-escape-sequence:
        \x hexadecimal-digit
    hexadecimal-escape-sequence hexadecimal-digit


    echo -e "\0233[0;31mRED\033[0m"1



First, let's go back to the `\` and `\x` characters. The ANSI standard defines something called the *'Control Sequence
Introducer'* (abbreviated as CSI). It doesn't define the CSI character (that's left to the application), however it's
very commonly represented as a slash (i.e. `echo -e "\033[0;31mRED\033[0m"1`)


For my use-case I'm interested specifically in the `SGR` (Select Graphic Rendition) mode.



- Control sequences can be 

[ANSI C89]:             http://www.open-std.org/JTC1/sc22/wg14/www/docs/n1256.pdf
[ISO/IEC 9899]:         http://www.open-std.org/JTC1/sc22/wg14/www/docs/n1256.pdf
[ANSI escape code]:     https://en.wikipedia.org/wiki/ANSI_escape_code


## xTerm


xTerm supports four color modes (that I know of):

| Bits | Colorspace | 
| 3    | 2^3 (8)    |
| 
- 3-bit 
- 8-bit color:


Bit    7  6  5  4  3  2  1  0
Data   R  R  R  G  G  G  B  B

[8-bit color]: https://en.wikipedia.org/wiki/8-bit_color
[Web Safe Colors]: http://www.permadi.com/tutorial/websafecolor/


## Bookmarks

[ConEmu AnsiEscapCodes]: https://code.google.com/p/conemu-maximus5/wiki/AnsiEscapeCodes


[xtermcolor]: https://github.com/broadinstitute/xtermcolor
