#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
02-urwid.py

This example builds on the previous. Rather than applying the text styling by
passing an AttrSpec in through the TextWidget markup parameter, we're using
AttrMap to create a new object with styling applied using a palette.

"""

import sys

try:
    import urwid
except ImportError as error:
    sys.stderr.write('Failed to import urwid: ' + str(error))
    sys.exit(1)

NAME = sys.argv[0]

def quit():
    """ Graceful exit. """
    raise urwid.ExitMainLoop()

def default_input_handler(key):
    """ Default input event handler. """
    if key in ('Q', 'q', 'esc'):
        quit()

# Attribute specifications can be stored in a palette and referenced later by
# name. The palette is essentially a dictionary of named AttrSpec objects.
# Note: I haven't looked at the internals to determine if that is actually true,
# but it's the way I'm thinking about it atm.

palette = [
    ('text_palette', 'yellow', 'dark magenta')
]

# Our text widget is no longer being instantiated with the AttrSpec from the
# previous example. It has no colors.

text_widget = urwid.Text((NAME), align='center')

# We can use the AttrMap api to create a new object from an existing object with
# an AttrSpec applied to it. Note that this doesn't modify the original object
# it still exists in the same state. Note that this will apply the coloring to
# the entire widget, rather than just the visible text (the way we did in the
# previous example).
# - http://goo.gl/bcneMI

text_widget_mapped = urwid.AttrMap(text_widget, 'text_palette')

# We pass our text_widget_mapped object to the container, rather than the
# original object.
container_widget = urwid.Filler(text_widget_mapped, valign='middle')

if __name__ == '__main__':

    # In order to use the palette, we have to pass it to the MainLoop method as
    # a parameter.
    loop = urwid.MainLoop(container_widget,
                          palette,
                          unhandled_input=default_input_handler)
    loop.run()
