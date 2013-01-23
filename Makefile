PROJNAME=freeblocks
SRCDIR=src
BUILDDIR=build
SOURCES=$(shell ls $(SRCDIR)/*.c)
OBJECTS=$(patsubst $(SRCDIR)%.c,$(BUILDDIR)%.o, $(SOURCES))

CC=gcc
CFLAGS+=-Wall -O2 -mms-bitfields -std=c99
LDFLAGS+=-lSDL -lSDL_ttf -lSDL_image -lSDL_mixer

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

