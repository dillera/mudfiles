#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
03-urwid.py

This example adds a custom PromptWidget, wraps all the widgets up in a frame,
and uses piles and dividers to break things up a little bit.

"""

import sys

try:
    import urwid
except ImportError as error:
    sys.stderr.write('Failed to import urwid: ' + str(error))
    sys.exit(1)

NAME = sys.argv[0]
PROMPT = '$  '

def quit():
    """ Graceful exit. """
    raise urwid.ExitMainLoop()

def default_input_handler(key):
    """ Default input event handler. """
    if key in ('Q', 'q', 'esc'):
        quit()

palette = [
    ('header', 'white', 'dark blue'),
    ('body', 'white', 'black'),
    ('footer', 'white', 'dark blue')
]

# In order to make this function a little bit more like an actual prompt, we'll
# define our own class based on the edit box and redefine the keypress function.
# In order to interact with other objects we need to jump into signals, which we
# won't do here. Our custom edit box is only going to interact with itself for
# now.

class PromptWidget(urwid.Edit):

    def keypress(self, size, key):

        # Very simple, if the key is esc, then quit. If the key is enter, reset
        # the edit widget's text. Otherwise, call the base classes' keypress
        # handler.

        if key == 'esc':
            quit()
        elif key == 'enter':
            self.set_edit_text('')
        else:
            urwid.Edit.keypress(self, size, key)

# Our header widget is a pile that contains two objects - a text box and
# a divider.

header = urwid.Pile([
    urwid.AttrMap(urwid.Text(NAME, align='center'), 'header'),
    urwid.Divider(div_char='-')])

# Our 'body' is just a filler widget that wraps a text widget.

body = urwid.Filler(urwid.AttrMap(urwid.Text(u'The console area..'), 'body'))

# Our footer is just like our header, except that we're using our PromptWidget
# rather than a text widget.

footer = urwid.Pile([
    urwid.Divider(div_char='-'),
    urwid.AttrMap(PromptWidget(PROMPT), 'footer')])

# And finally our frame widget. This is a box/layout widget. We're just using it
# to break up our user interface into three different sections. We also focus
# the footer (rather than the default of body) so that we can enter text.

frame = urwid.Frame(body, header=header, footer=footer, focus_part='footer')

if __name__ == '__main__':
    loop = urwid.MainLoop(frame,
                          palette,
                          unhandled_input=default_input_handler)
    loop.run()
