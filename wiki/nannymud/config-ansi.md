# ANSI

## Command Reference

| Command     | Arguments                                  | Description                                              |
| ----------- | ------------------------------------------ | -------------------------------------------------------- |
| help colour | N/A                                        | Display a HUGE colour helpfile.                          |
| toggle ansi | N/A                                        | Turn it on ANSI colours.                                 |
| colour      | <class> <foreground> [<background-colour>] | Configure a colour class.                                |
| colour      | remove <class>                             | Remove the configuration for a color class.              |
| colour      | <colourscheme>                             | Install a colourscheme.                                  |
| colour      | list                                       | Display all the configured color classes.                |
| colour      | all                                        | Display the available colors.                            |

## Example Commands

    colour default          - Install the default colour scheme.
    colour long cyan        - Cyan text.
    colour long cyan blue   - Cyan text on blue background.
    colour remove long      - Remove colouring of the text.
    colour all              - Show all colour combinations.
    colour list             - Show your current settings.

## Colour Classes

| Name                 | Category | Description                                                                   |
| -------------------- | -------- | ------------------------------------------------------------------------------|
| prompt               | Default  | Your prompt.                                                                  |
| short                | Default  | Short description of rooms.                                                   |
| long                 | Default  | Long description in standard rooms.                                           |
| exit                 | Default  | Exits in standard rooms.                                                      |
| player               | Default  | Players in a room.                                                            |
| monster              | Default  | Monsters in a room.                                                           |
| armour               | Default  | Armours in a room or container.                                               |
| autoloader           | Default  | Autoloaders on a player.                                                      |
| container            | Default  | Containers in a room or container or on a being.                              |
| drink                | Default  | Drinks in a room or container or on a being.                                  |
| food                 | Default  | Food stuff in a room or container or on a being.                              |
| misc_in_inv          | Default  | Everything else in a container or on a being.                                 |
| misc_in_long         | Default  | Everything else in a room.                                                    |
| weapon               | Default  | Weapons in a room or container or on a being.                                 |
| enter                | Default  | Player entering the location.                                                 |
| leave                | Default  | Player leaving the location.                                                  |
| monster_enter        | Default  | Monster entering the location.                                                |
| monster_leave        | Default  | Monster leaving the location.                                                 |
| attack               | Default  | Your attack messages.                                                         |
| attacked             | Default  | Your enemies attacks on you.                                                  |
| died                 | Default  | Your death.                                                                   |
| killed               | Default  | You killed something.                                                         |
| hpinfo               | Default  | The hpinfo.                                                                   |
| shape_info           | Default  | The 'shape info'.                                                             |
| say                  | Default  | Things you say.                                                               |
| said                 | Default  | Things said to you.                                                           |
| replied              | Default  | Replies to you.                                                               |
| reply                | Default  | Your replies.                                                                 |
| tell                 | Default  | Your tells.                                                                   |
| told                 | Default  | Things you are told.                                                          |
| whisper              | Default  | Your whispers.                                                                |
| whispered            | Default  | Things whispered to you.                                                      |
| shouted              | Default  | Shouts you hear.                                                              |
| shout                | Default  | Shouts you do.                                                                |
| party                | Default  | The party line.                                                               |
| personal_description | Default  | Players personal description.                                                 |
| mail_new             | Default  | For arriving mail.                                                            |
| whoadmin             | Default  | Admins                                                                        |
| whowizard            | Default  | Wizards                                                                       |
| whoparagon           | Default  | Paragons                                                                      |
| whomortal            | Default  | All other.                                                                    |
| delimiter            | Default  | The lines.                                                                    |
| simyarin             | Simyarin |                                                                               |
| simyarin_gold        | Simyarin |                                                                               |
| simyarin_low         | Simyarin |                                                                               |
| simyarin_reply       | Simyarin |                                                                               |
| simyarin_tell        | Simyarin |                                                                               |
| simyarin_told        | Simyarin |                                                                               |
| simyarin_wizard      | Simyarin |                                                                               |


## Additional
You can add colour to guild lines etc. The exact class to use is dependant on the guild. For example, in the Masters Guild, you'd do 'colour masters fgcolour bgcolour' for the line, and 'colour masters_tell fgcolour bgcolour' for linetells.

You can use colours in some clubs, but it is up to the club. You should check the clubs help to see if it is supported.

## Foreground Colours

    black   boldblack   underlineblack   boldunderlineblack
    red     boldred     underlinered     boldunderlinered
    green   boldgreen   underlinegreen   boldunderlinegreen
    blue    boldblue    underlineblue    boldunderlineblue
    yellow  boldyellow  underlineyellow  boldunderlineyellow
    magenta boldmagenta underlinemagenta boldunderlinemagenta
    cyan    boldcyan    underlinecyan    boldunderlinecyan
    white   boldwhite   underlinewhite   boldunderlinewhite
    default bolddefault underlinedefault boldunderlinedefault

You can also use 'bold' and 'underline' alone.

## Background Colours

    red, green, yellow, blue, magenta, cyan, white, and default.

## My Colour Class Configuration
The #nop commands are TinTin comments, which allows me to cut and paste the following content all at once and still have comments describing each section.

    #nop   ITEM COLOURS;
    colour armour magenta default
    colour autoloader magenta default
    colour container magenta default
    colour drink magenta default
    colour food magenta default
    colour misc_in_inv magenta default
    colour weapon magenta default

    #nop   COMBAT COLORS;
    colour attack red default
    colour attacked red default
    colour died red default
    colour killed red default
    colour monster yellow default
    colour monster_enter red default
    colour monster_leave red default
    colour shape_info yellow default

    #nop   CHAT COLORS;
    colour hpinfo yellow default
    colour leave red default
    colour long white default
    colour mail_new white default
    colour party green default
    colour replied green default
    colour reply green default
    colour said green default
    colour say green default
    colour shout magenta default
    colour shouted green default
    colour tell green default
    colour told green default
    colour whisper green default
    colour whispered green default

    #nop   PLAYER LIST COLOURS;
    colour delimiter blue default
    colour whoadmin red default
    colour whomortal white default
    colour whoparagon magenta default
    colour whowizard red default

    #nop   MISC COLOURS;
    colour personal_description white default
    colour prompt white default

    #nop   ROOM COLOURS;
    colour enter red default
    colour exit white default
    colour misc_in_long magenta default
    colour player yellow default
    colour short cyan default

    #nop   SIMYARIN COLOURS;
    colour simyarin green default
    colour simyarin_gold green default
    colour simyarin_low green default
    colour simyarin_reply green default
    colour simyarin_tell green default
    colour simyarin_told green default
    colour simyarin_wizard green default

## Ansi Bleeding
The following command can be added to the (TinTin) prompt to prevent ANSI bleeding:

    #sub {[your prompt]} {<088>[your prompt]}

+ [Ansi Bleed](http://tintin.sourceforge.net/board/viewtopic.php?t=1394)
