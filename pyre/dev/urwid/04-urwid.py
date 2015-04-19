#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
04-urwid.py

This example impliments a more robust body that contains multiple line objects
and adds signals that allow the widgets to interact with each other.

https://gist.github.com/ksamuel/1521153
https://stackoverflow.com/questions/20130631/how-can-i-change-an-urwid-edits-text-from-the-change-signal-handler


http://urwid.org/reference/signals.html#urwid.connect_signal
http://www.pythondiary.com/tutorials/build-website-ncurses.html
http://iamkevin.ca

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


class Widget(urwid.WidgetWrap):
    pass

class InputWidget(urwid.Edit):

    def keypress(self, size, key):
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

# Our footer is just like our a header - a pile containing a divider and a text
# box. We can easily make this an 'edit' box by just replacing urwid.Text with
# urwid.Edit, however the edit box hooks the input keys and will prevent our
# input handler from working, so I've left it as is for this example. The next
# example will move onto to creating a scaffolding for the prompt widget.

footer = urwid.Pile([
    urwid.Divider(div_char='-'),
    urwid.AttrMap(Prompt(PROMPT), 'footer')])

# And finally our frame widget. This is a box/layout widget. We're just using it
# to break up our user interface into three different sections.

frame = urwid.Frame(body, header=header, footer=footer, focus_part='footer')


def setup_logging(logfile=None):

    if logfile == None:
        logfile = '/urwid.log'

    logdir = os.path.dirname(logfile)

    if not os.path.exists(logdir):
        os.makedirs(logdir)

    logging.basicConfig(filename=logfile, level=logging.DEBUG, filemode='w')

if __name__ == '__main__':
    loop = urwid.MainLoop(frame,
                          palette,
                          unhandled_input=default_input_handler)
    loop.run()
