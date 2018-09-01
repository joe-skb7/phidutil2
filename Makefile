CFLAGS = $(shell pkg-config --cflags libphidget22)
LDLIBS = $(shell pkg-config --libs libphidget22)
APP = phidutil2
PREFIX ?= /usr/local
SOURCES =			\
	src/main.c		\
	src/phidget.c		\
	src/tools.c
OBJS = $(SOURCES:%.c=%.o)

include Makefile.common

default: $(APP)

$(APP): $(OBJS)
	@printf "  LD      $@\n"
	$(Q)$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

clean:
	@printf "  CLEAN\n"
	$(Q)rm -f $(APP)
	$(Q)find src/ -name '*.o' -exec rm {} \;
	$(Q)find src/ -name '*.d' -exec rm {} \;

distclean: clean
	@printf "  DISTCLEAN\n"
	$(Q)$(RM) -f cscope.* tags

install: $(APP)
	@printf "  INSTALL\n"
	$(Q)install -d $(DESTDIR)$(PREFIX)/bin
	$(Q)install -m 0755 $(APP) $(DESTDIR)$(PREFIX)/bin

debian:
	@printf "  DEBIAN\n"
	$(Q)dpkg-buildpackage -us -uc -tc

%.o: %.c
	@printf "  CC      $(*).c\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).c

.PHONY: default clean distclean install debian

-include $(OBJS:.o=.d)
