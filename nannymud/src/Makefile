# This line is needed on some machines.
MAKE=make
#Enable warnings from the compiler, if wanted.
WARN=
#
#Choose yhour malloc
#MALLOC=sysmalloc
MALLOC=smalloc

RAND=RAND
YACC=bison -y
YFLAGS=-d -v

ECHO=/usr/gnu/bin/echo

# Set MUD_LIB to the directory which contains the mud data. Was formerly
# defined in config.h !
MUD_LIB = /usr/lpmud/lib

#WARN= -Wall -Wshadow -Dlint  
# 
# Enable run time debugging. It will use more time and space.
# When the flag is changed, be sure to recompile everything.
# Simple comment out this line if not wanted.
DEBUG=-DDEBUG -DTRACE_CODE
MPATH=-DMUD_LIB='"$(MUD_LIB)"'
#
# If you have a NeXT, add this to CFLAGS:
# -DNeXT -bsd
#
DEBUGOPT=-O4711 -g
#
OTHER= # -traditional
#
CFLAGS=$(DEBUGOPT) $(OTHER) -D$(RAND) -DMALLOC_$(MALLOC) $(WARN)  $(PROFIL) $(DEBUG) $(MPATH) -DBINDIR='"$(BINDIR)"'
CC=gcc
LD=$(CC)
#
# Add extra libraries here.
#    For Solaris 2.x add '-lsocket -lnsl'
#    For Redhat 5 (glibc) add '-lcrypt'
LIBS=-lm -ltermcap
#
#-lauthuser -ldebug-malloc # -L.# util/gc/gc.a
#
MFLAGS = "BINDIR=$(BINDIR)" "MUD_LIB=$(MUD_LIB)"
#
# Add str.c here if you don't have memcpy() or strtol()
#
SRC=lang.tab.c lex.c main.c interpret.c interpret2.c simulate.c object.c\
    backend.c array.c comm1.c ed.c regexp.c mapping.c wiz_list.c swap.c\
    $(MALLOC).c call_out.c otable.c dumpstat.c stralloc.c hash.c indentp.c\
    port.c access_check.c clilib.c parse_old.c parse.c\
    simul_efun.c sprintf.c dump.c sort.c socket_efuns.c socket_ctrl.c\
    socket_err.c acl_efuns.c acl_lib.c acl.c version.c \
    grp_lib.c config.c bsdcompat.c findmalloc.c

DRIVER = driver

OBJDIR = obj

VPATH = $(OBJDIR)

OBJ=$(addprefix $(OBJDIR)/,$(subst .c,.o,$(SRC)))

$(DRIVER): $(OBJDIR) $(OBJ)
	-rm -f $(DRIVER).old
	-mv -f $(DRIVER) $(DRIVER).old
	$(LD) $(CFLAGS) $(OBJ) -o $(DRIVER) $(LIBS)

pure: $(OBJDIR) $(OBJ)
	-rm -f $(DRIVER).old
	-mv -f $(DRIVER) $(DRIVER).old
	purify -chain-length=10 -user-path=/usr/lpmud/src/server:/usr/lpmud/bin -inuse-at-exit=yes $(LD) $(CFLAGS) $(OBJ) -o $(DRIVER) $(LIBS)
#	purify -logfile=/usr/lpmud/log/purifylog.%v.%p -chain-length=10 -user-path=/usr/lpmud/src/server:/usr/lpmud/bin -inuse-at-exit=yes $(LD) $(CFLAGS) $(OBJ) -o $(DRIVER) $(LIBS)

$(OBJDIR):
	mkdir $(OBJDIR)
	@chmod 775 $(OBJDIR)
	@-chgrp mud $(OBJDIR)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	@chmod 664 $@
	@-chgrp mud $@


list_funcs:
	@$(CC) -E $(CFLAGS) func_spec.c

%.tab.c: %.y
	$(YACC) $(YFLAGS) $<
	-mv -f y.tab.c $@
	-mv -f y.tab.h $*.h

lang.y efun_defs.c: func_spec.c make_func prelang.y postlang.y config.h \
	lint.h interpret.h object.h exec.h config.h instrs.h incralloc.h\
	switch.h
	-rm -f efun_defs.c
	./make_func > efun_defs.c

make_func: make_func.tab.c
	$(CC) -o $@ $<
	@chmod 775 $@
	@-chgrp mud $@

#files := $(sort $(wildcard $(BINDIR)/lpmud-*))
#new_bin := $(word $(words $(files)), $(files))
#new_bin := $(addsuffix $(shell expr $(subst .,,$(suffix $(new_bin))) + 1), $(addsuffix .,$(basename $(new_bin))))

#install: $(DRIVER)
#	@$(ECHO) "Installing driver $(new_bin)"
#	-rm -rf $(BINDIR)/lpmud
#	mv $(DRIVER) $(new_bin)
#	(cd $(BINDIR); ln -s $(notdir $(new_bin)) lpmud)

clean:
	-rm -rf $(OBJDIR) *.o lang.h lang.c mon.out *.ln tags
	-rm -f *.tab.c *.tab.h *.rej *.orig y.output make_func.h
	-rm -f efun_defs.c lang.y $(DRIVER_BIN) core TAGS make_func

clean.util:
	(cd util ; $(ECHO) "Cleaning in util." ; $(MAKE) clean)

tags: $(SRC)
	ctags $(SRC)

TAGS: $(SRC)
	etags $(SRC)

depend: $(SRC)
	-rm Dependencies
	touch Dependencies
	for i in $(SRC); do $(ECHO) -n $(OBJDIR)/ >> Dependencies; $(CC) -E -MM $$i >> Dependencies; done

include Dependencies
