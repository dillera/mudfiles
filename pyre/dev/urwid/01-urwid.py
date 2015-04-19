#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
01-urwid.py

A very simple urwid example. Urwid is really f'n tricky. It took me a little bit 
to figure things out. There's lots of documentation, read it carefully. I tried
to jump right in with the tutorial before reading the manual and was thoroughly
confused as I tried to start building my own code.

- http://urwid.org/manual/overview.html
- http://urwid.org/tutorial/
- http://urwid.org/manual/displayattributes.html
- http://urwid.org/reference/constants.html
- http://urwid.org/manual/widgets.html

"""

import sys

# Attempt to import urwid. If it's not available, display a meaningful error
# message and exit gracefully. If you've only installed urwid in a virtualenv,
# this will provide a helpful reminder to activate it, rather than the standard
# traceback.

try:
    import urwid
except ImportError as error:
    sys.stderr.write('Failed to import urwid: ' + str(error))
    sys.exit(1)

# Get the name of this script.
NAME = sys.argv[0]

# There are several ways to exit. Calling sys.exit(0) will leave us with a
# broken console. The best way I've found is straight from the Urwid manual: 
# raise an ExitMainLoop urwid exception.

def quit():
    raise urwid.ExitMainLoop()

# Our standard input handler. This parses the key input one character at a time.
# If the character is a Q, q, or esc, we terminate the main loop to exit the
# application.

def input(key):
    if key in ('Q', 'q', 'esc'):
        quit()

# Display attributes are mapped onto widgets in several ways. This can be
# through a palette, application through the AttrMap API, or instantiating
# a widget object using an AttrSpec object. For this example, we're using the
# most basic method: an AttrSpec object.
# - http://urwid.org/reference/attrspec.html#urwid.AttrSpec

text_attributes = urwid.AttrSpec('yellow', 'dark magenta')

# Urwid defines various 'widgets' that we can use in our curses application.
# Widgets are all descended from the same base widget class, which defines a 
# standard API for interacting with widgets within the framework. There are two
# four basic types of widgets: 'Basic', 'Graphic', 'Decoration' and 'Container'.
# The different types of widgets behave differently and must be used according
# to their purpose.

# Widget Documentation: http://urwid.org/reference/widget.html
# Widget Source:        http://goo.gl/UhYUdL

# This is a simple text widget that displays text on the screen. The text widget
# class supports a handful of parameters: markup, alignment, wrapping, and a
# layout. The markup parameter can take several forms - just text, an attribute
# specification & text, or an attribute specification palette reference & text.

# - http://urwid.org/reference/widget.html#text
# - http://urwid.org/manual/displayattributes.html#text-markup

# Note that since we're passing in the attrspec as a text markup parameter, this
# will only color the displayed text, rather than the entire widget. The entire
# widget actually extends the entire width of the terminal.

text_widget = urwid.Text((text_attributes, NAME), align='center')

# Our text widget is a 'Basic' widget and therefore cannot be the topmost widget
# passed to the event loop. Therefore, we have to wrap it in a 'box widget'
# container. The Filler widget is a very simple box widget that can be used for
# this purpose.

container_widget = urwid.Filler(text_widget, valign='middle')

if __name__ == '__main__':

    # Urwid runs in an event loop. There are several built-in event loops to
    # choose from. For this example, we're using the default MainLoop. This
    # method accepts several parameters, those we're using here:

    # widget: The topmost widget (must be a box widget).
    # palette: The initial palette for the screen instance.
    # unhandled_input: If the widget (or widget container) does not handle input
    # events, that input will be sent to this hook for processing.

    # - http://urwid.org/reference/main_loop.html

    loop = urwid.MainLoop(container_widget, unhandled_input=input)

    # And finally, we need to start our loop.
    loop.run()
