# Terminal Colors

Getting consitant coloring in your terminal is a *fucking nightmare*, here's a list of the various things (that I know
about) that configure your terminal colors:

- essential applications: ls (gnu, bsd, dircolors), grep (gnu, bsd), less, diff, etc ...
- essential scripts: lesspipe, source-highlight, colordiff, etc ...
- applications: git, screen/tmux/byobu, irssi/weechat, vim, etc ...
- shells: bash, zsh, ash/dash, csh, tcsh, ksh, etc ...
- terminal emulators: xterm, iterm2, Terminal.app, terminator, mintty, etc ...
- X11 (via .Xresources, affects lots of things ...)

Of course, they're all configured different ways, using various encodings that are incompatible.

- iTerm2 color definitions are configured using [HSL](http://www.css3.info/preview/hsl/) color encodings
  - these take advantage of the native display and are not limited to the 256 (8-bit) xterm palette
  - I assume that iTerm2 uses the HSL encodings to take advantage of the OSX/cocoa native color API
  - this configuration redefines the "base 16" terminal color palette, but does not affect the remaining 240 colors from the xterm palette
- The X11 specification maps the set of 16-bit values (256) the [web-safe RGB color palette](https://en.wikipedia.org/wiki/Web_colors#Web-safe_colors).
  - It also provides "friendly" [names](https://en.wikipedia.org/wiki/X11_color_names) for these color values.



- https://github.com/trapd00r/LS_COLORS
- [colorcoke](https://github.com/trapd00r/colorcoke)
- https://stackoverflow.com/questions/27159322/rgb-values-of-the-colors-in-the-ansi-extended-colors-index-17-255


http://docstore.mik.ua/orelly/unix3/upt/ch06_08.htm
[official xorg - rgb.txt](http://cgit.freedesktop.org/xorg/app/rgb/tree/rgb.txt)
http://gidden.net/tom/2006/08/04/x11-color-list-for-macosx/
http://people.csail.mit.edu/jaffer/Color/Dictionaries


https://gist.github.com/XVilka/8346728#now-supporting-truecolor
https://stackoverflow.com/questions/15682537/ansi-color-specific-rgb-sequence-bash
https://bitbucket.org/ZyX_I/vim/commits/5e3877fb6ab824994b0aa40a2786768fbce473e3
https://github.com/robertknight/konsole/blob/master/user-doc/README.moreColors
https://github.com/neovim/neovim/issues/793#issuecomment-48106948
https://wiki.mpich.org/mpich/index.php/Configure_xterm_Fonts_and_Colors_for_Your_Eyeball
https://sunaku.github.io/zenburn-terminal-color-scheme.html

http://vim.wikia.com/wiki/View_all_colors_available_to_gvim
http://www.robmeerman.co.uk/unix/256colours

http://vim.wikia.com/wiki/Using_GUI_color_settings_in_a_terminal
http://vim.wikia.com/wiki/Xterm256_color_names_for_console_Vim
https://stackoverflow.com/questions/11492166/how-to-change-xterm-colors-using-xdefaults-file
https://unix.stackexchange.com/questions/20257/how-do-you-display-xterm-colors-with-putty-bash


## Color Utils

### Color Capability Testing

Various color test scripts:

- [256colors2.pl](https://code.google.com/p/joeldotfiles/source/browse/trunk/256colors2.pl)
- [iterm2colors.sh](https://code.google.com/p/iterm2/source/browse/trunk/tests/colors.sh)
- [colorspaces.pl](https://github.com/robertknight/konsole/blob/master/tests/color-spaces.pl)
- [img.sh](https://git.gnome.org/browse/vte/tree/perf/img.sh?h=vte-0-36)
- https://github.com/robertknight/konsole/blob/master/tests/colortest.sh
- [vimcolorschemetest](https://code.google.com/p/vimcolorschemetest/)
- https://github.com/vim-scripts/Color-Scheme-Test

More: http://crunchbang.org/forums/viewtopic.php?pid=126921%23p126921#p126921

### Color Converion Tools

- [csapprox](https://github.com/godlygeek/csapprox)
- [colortrans.py](https://gist.github.com/MicahElliott/719710)
- [lighten-colors.rb](https://github.com/qtpi/Jellybeans.itermcolors/blob/master/scripts/lighten_colors.rb)
- https://github.com/jordanstephens/paleta
- [itermcolors2xdefaults](https://github.com/richo/itermcolors2xdefaults)
 
### Color Palette Generators

- http://bytefluent.com/vivify/
- http://paletton.com/
- http://colourco.de/
- https://color.adobe.com/
- http://colllor.com/
- [Colourlovers.com](http://www.colourlovers.com/)

### Base16

Holy shit, [base16](https://github.com/chriskempson/base16) is tits!

- [base16-builder](https://github.com/chriskempson/base16-builder)
- [base16-shell](https://github.com/chriskempson/base16-shell)
- [base16-iterm2](https://github.com/chriskempson/base16-iterm2)
- [base16-vim](https://github.com/chriskempson/base16-vim)
- [base16-xresources](https://github.com/chriskempson/base16-xresources)


## Vim Colorschemes

- [badwolf](https://github.com/sjl/badwolf)
- [vim-kalesi](https://github.com/freeo/vim-kalisi)
- [vim-abra](https://github.com/abra/vim-abra)
- [jellyx.vim](https://github.com/guns/jellyx.vim)
- [xoria256.vim](https://github.com/vim-scripts/xoria256.vim)
- [vim-hybrid](https://github.com/w0ng/vim-hybrid)
- [github-dark](https://github.com/StylishThemes/GitHub-Dark)
- [twilight256.vim](https://github.com/philc/vim-config/blob/master/colors/twilight256.vim)
- [ir_black](https://github.com/wesgibbs/vim-irblack)
 
 ### Jellybeans Variations
 
- [jellybeans.itermcolors](https://github.com/mbadolato/iTerm2-Color-Schemes/blob/master/schemes/Jellybeans.itermcolors)
- [qtpi/Jellybeans.itermcolors](https://github.com/qtpi/Jellybeans.itermcolors)
- [jellybeans.vim](https://github.com/nanotech/jellybeans.vim)
- [bling/vim-airline - jellybeans.vim](https://github.com/bling/vim-airline/blob/master/autoload/airline/themes/jellybeans.vim)
- [edkolev/promptline.vim - jelly.vim](http://github.com/edkolev/promptline.vim/blob/master/autoload/promptline/themes/jelly.vim)
- [edkolev/tmuxline.vim - jellybeans.vim](https://github.com/edkolev/tmuxline.vim/blob/master/autoload/tmuxline/themes/jellybeans.vim)
- [itchyny/lightline - jellybeans.vim](https://github.com/itchyny/lightline.vim/blob/master/autoload/lightline/colorscheme/jellybeans.vim)
 
## Color Theory

- https://stackoverflow.com/questions/5901136/how-to-make-a-color-progression-out-of-a-color-palette
- https://stackoverflow.com/questions/3054873/programmatically-find-complement-of-colors
- https://graphicdesign.stackexchange.com/questions/27786/how-to-find-complementary-colors
- https://graphicdesign.stackexchange.com/questions/1316/what-are-the-true-complementary-colors-and-their-values
- http://serennu.com/colour/rgbtohsl.php
- [Color Science](http://www.midnightkite.com/color.html)
- http://homepage.tinet.ie/~musima/colourtheory/colourtheory.htm
- https://stackoverflow.com/questions/5860100/how-to-interpolate-a-color-sequence/
- https://stackoverflow.com/questions/10901085/python-range-values-to-pseudocolor/10902473#10902473


http://w3facility.org/question/cant-use-vim-themes-in-iterm-or-terminal/
https://gist.github.com/cskeeters/9674586
http://www.tuxarena.com/2012/04/tutorial-colored-man-pages-how-it-works/



## Coreutils


### ls
There two commonly encountered `ls` implimentations - and they function slightly differently:

- [posix](http://pubs.opengroup.org/onlinepubs/009695399/utilities/ls.html) `ls` has no color support (not commonly encountered, but the basis for the bsd & gnu distributions)
- the [bsd](https://www.freebsd.org/cgi/man.cgi?query=ls) `ls` (OSX, FreeBSD, OpenBSD, etc ...) supports colored output
  - if the `CLICOLOR` environment variable is set (to anything), `ls` displays colored output
  - if `ls` is executed with `-G`, `ls` displays colored output
  - the output is colored according to the value of `LSCOLORS`
- the [GNU](https://www.gnu.org/software/coreutils/) `ls` supports colored output
  - the flag `-C` (or `--color=<option>`) determines when colored output is displayed
    - if `<option>` is always, colors are always displayed (color escapes are written), even in non-interactive shells (default)
    - if `<option>` is never, no color is displayed (default behavior)
    - if `<option>` is auto, colors are only displayed in interactive shells
  - the colors are determined by the contents of the `LS_COLORS` environment variable

- dircolors?
- GNU's `ls` is *way* more flexible than BSD's `ls`.

- [LS_COLORS](https://www.gnu.org/software/coreutils/manual/html_node/General-output-formatting.html)


### grep
Grep, much like ls, also is distributed with two primary implimentations:

- [posix](http://pubs.opengroup.org/onlinepubs/009604499/utilities/grep.html) `grep` has no support for colored output
- the [bsd](https://www.freebsd.org/cgi/man.cgi?query=grep) grep supports colored output
  - the `--colour=<option>` flag determines when colored output is displayed (just like coreutils `ls`)
  - option can be `always`, `never`, or `auto`, just like coreutils `ls`
  - options can be specified via the `GREP_OPTIONS` environment variable
  - colors can be specified via the `GREP_COLOR` environment variable
- the [gnu](https://www.gnu.org/software/grep/manual/grep.html) `grep` supports colored output
  - the `GREP_OPTIONS` environment variable is decprecated
  - the `--colour=<option>` flag determines when colored output is displayed (just like coreutils `ls`)

- [GREP_COLORS](https://www.gnu.org/software/grep/manual/html_node/Environment-Variables.html)

### less, less-pipe & source-highlight



- http://funkworks.blogspot.com/2013/01/syntax-highlighting-in-less-on-osx.html
- https://wincent.com/wiki/Installing_GNU_Source-highlight_on_Mac_OS_X_10.6.7_Snow_Leopard

### xterm



- .Xdefaults - this file is deprecated in favor of .Xresources.
- [.Xresources](https://wiki.archlinux.org/index.php/X_resources)
- [xrdb](https://en.wikipedia.org/wiki/Xrdb)

    xrdb -query -all

    /opt/X11/share/X11/app-defaults/XTerm
    /opt/X11/share/X11/app-defaults/XTerm-color

    /opt/X11/share/X11/rgb.txt


### iTerm2

- [Tab Colors in iTerm2 v1](http://kendsnyder.com/tab-colors-in-iterm2-v10/)
- [Use a nicely colored ls directory display in iTerm2 for OS X](http://www.friendlyskies.net/notebook/use-a-nicely-colored-ls-directory-display-in-iterm2-for-os-x)
- [Customizing iTerm. Creating a display profile with pastel colors](http://kpumuk.info/mac-os-x/customizing-iterm-creating-a-display-profile-with-pastel-colors/)
- [Exporting and importing iTerm 2 Color Schemes](http://ngs.io/2014/05/05/iterm2-color-schemes/)

### mintty


## Jellybeans
- https://github.com/qtpi/Jellybeans.itermcolors
- https://github.com/saimn/dotfiles/blob/master/vim/colors/jellybeans.vim

- [Hidden gems of xterm](http://lukas.zapletalovi.com/2013/07/hidden-gems-of-xterm.html)

## Tmux

- [tmux, TERM and 256 colours support](https://unix.stackexchange.com/questions/118806/tmux-term-and-256-colours-support)



## Misc

- [lscolors generator](http://geoff.greer.fm/lscolors/)
- [christhunt/color-schemes](https://github.com/chrishunt/color-schemes)
- https://github.com/mbadolato/iTerm2-Color-Schemes/tree/master/tools
- [ruby - lighten colors](https://github.com/qtpi/Jellybeans.itermcolors/blob/master/scripts/lighten_colors.rb)


- http://linuxaria.com/pills/coloring-grep-to-easier-research
- https://github.com/CQQL/xresources-theme
- https://wiki.mpich.org/mpich/index.php/Configure_xterm_Fonts_and_Colors_for_Your_Eyeball
- http://www.astrobetter.com/xterm-color/
- http://serennu.com/colour/rgbtohsl.php
