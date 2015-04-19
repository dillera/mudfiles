Tmux
====
TinTin++ is a 'single window' console application and supports only very basic layouts (`#split`). As a work around, I wrap tintin++ with [tmux][] & [tmuxp][] to build more advanced layouts.

Note: There are several alternatives to [tmuxp][] available ([tmuxinator][], [teamocil][], etc ...). I prefer tmuxp because it's written in python and provides a python API for interacting with tmux. Each of these tools leverage tmux's internal, *protected* layout interface to save & restore layouts, with additional commands to configure the behavior of panes/windows in a saved layout. To manually export the session's layout string from tmux, you can use the command `tmux display-message -p "#{window_layout}"`.

Here's a quick way to get started with [tmuxp][]:

    # install tmuxp & create the ~/.tmuxp directory
    pip install tmuxp
    mkdir -p ~/.tmuxp

    # create the tintin tmux session & manually built your interface
    tmux new -s tintin
    # manually create your windows, panes, etc.

    # save the layout - this will save to ~/.tmuxp/tintin.yaml
    tmuxp freeze tintin

Once you've saved the layout, you'll need to manually edit it. Specifically, you'll want to set the commands for your 'log panes' to `tail -f <logfile>`, and you'll want to set your 'main pain' to startup tintin (or if more fine-grained control is needed, execute a startup script wrapper - see [startin](startin) for an example).

Note: The `window-layout` string exports *exact* sizes, tailoring the layout to a specific window size/resolution. If you're running your tintin environment on displays on varying sizes, you'll probably want to build layouts for each platform. In this case, you'll most likely want to setup an additional wrapper for starting the environment where the layout can be specified (see [startin][] for an example of this).


<!-- References -->
[tintin]:       http://tintin.sourceforge.net
[tmux]:         http://tmux.sourceforge.net/
[tmuxp]:        https://github.com/tony/tmuxp
[teamocil]:     http://www.teamocil.com/
[tmuxinator]:   https://github.com/tmuxinator/tmuxinator
