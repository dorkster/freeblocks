PROJNAME=freeblocks
SRCDIR=src
BUILDDIR=build
SOURCES=$(shell ls $(SRCDIR)/*.c)
OBJECTS=$(patsubst $(SRCDIR)%.c,$(BUILDDIR)%.o, $(SOURCES))

CC=gcc
CFLAGS+=-Wall -O2 -mms-bitfields -std=c99
LDFLAGS+=-lSDL -lSDL_ttf -lSDL_image -lSDL_mixer

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
DATADIR=$(PREFIX)/share
PKGDATADIR=$(DATADIR)/$(PROJNAME)
APPDATADIR=$(DATADIR)/applications

INSTALL=install
INSTALLDIR=$(INSTALL) -d -m 0755
INSTALLBIN=$(INSTALL) -c -m 0755 -s
INSTALLDAT=$(INSTALL) -c -m 0644

#CFLAGS+=-DPKGDATADIR=\"$(PKGDATADIR)\"

all: $(BUILDDIR) $(SOURCES) $(PROJNAME)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/

depend: .depend

.depend: $(SOURCES)
	@rm -f ./.depend
	@$(CC) $(CFLAGS) -MM $^ > ./.depend
	@sed -i ':x; /\\$$/ { N; s/\\\n//; tx }' ./.depend
	@sed -i 's/^/$(BUILDDIR)\//' ./.depend

-include .depend

$(PROJNAME): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(CFLAGS) -o $@

$(BUILDDIR)/%.o:
	$(CC) $< -c $(CFLAGS) -o $@

clean:
	rm -rf $(BUILDDIR)/ $(PROJNAME)
	@rm ./.depend

install:
	$(INSTALLDIR) $(DESTDIR)$(BINDIR)
	$(INSTALLBIN) $(PROJNAME) $(DESTDIR)$(BINDIR)
	$(INSTALLDIR) $(DESTDIR)$(PKGDATADIR)/res/fonts
	$(INSTALLDAT) res/fonts/*.ttf $(DESTDIR)$(PKGDATADIR)/res/fonts
	$(INSTALLDIR) $(DESTDIR)$(PKGDATADIR)/res/graphics
	$(INSTALLDAT) res/graphics/*.png $(DESTDIR)$(PKGDATADIR)/res/graphics
	$(INSTALLDIR) $(DESTDIR)$(PKGDATADIR)/res/sounds
	$(INSTALLDAT) res/sounds/*.wav $(DESTDIR)$(PKGDATADIR)/res/sounds
	$(INSTALLDAT) res/sounds/*.ogg $(DESTDIR)$(PKGDATADIR)/res/sounds
	$(INSTALLDIR) $(DESTDIR)$(APPDATADIR)
	$(INSTALLDAT) $(PROJNAME).desktop $(DESTDIR)$(APPDATADIR)

.PHONY: install clean
