# MSP (Mud Sound Protocol)

## About
NannyMUD MSP support. It appears that there were plans to release a sound pack, but it never happened.

## References

+ [NannyMUD MSP Help Page](http://mud.lysator.liu.se/www/wizdoc/obj/mud_sound.html)
+ [TinTin MSP Script](http://tintin.sourceforge.net/scripts/msp.php)

## Sound Codes

    !!SOUND(nannymud/event/wear* V=50 L=1 P=50 T=sound)
    !!SOUND(nannymud/event/remove* V=50 L=1 P=50 T=sound)

## Sound Class Structure

    NannyMUD
    +-event
    | +- kill
    | +- die
    | +- login
    | +- logout
    | +- wrongness
    | +- sober
    | ...
    +-soul
    | +- laugh_he
    | +- laugh_she
    | +- laugh_it
    | +- burp_he
    | ...
    +-combat
    | +-slash
    | | +- slash-sound1
    | | +- slash-sound2
    | | ...
    | +-chop
    | | ...
    | +...
    +-spells
    | +- spell-sound1
    | +- spell-sound2
    | ...
    +-ambient
    | +...
    +-music
    | +...
    +-guild
    | +...
    +-weather
    | +...
    +-misc
    | +...
    ...
