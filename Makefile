CC = gcc
INCDIRS = -I.
OPT = -O0
CFLAGS = -Wall -g $(INCDIRS) $(OPT)

SRCDIR   = src
BUILDDIR = build

MODULES = 
CFILES = $(SRCDIR)/main.c $(foreach module,$(MODULES),$(wildcard $(SRCDIR)/$(module)/*.c))
OBJECTS = $(patsubst %.c, $(BUILDDIR)/%.o, $(notdir $(CFILES)))
BINARY = $(BUILDDIR)/juggernog

VPATH = $(dir $(CFILES))

.PHONY: all run clean debug

all: $(BINARY)

run: $(BINARY)
	$(BINARY)

$(BINARY): $(OBJECTS) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

debug:
	@echo "Source files: $(CFILES)"
	@echo "Object files: $(OBJECTS)"
	@echo "Moduel folders: $(MODULES)"
	@echo "VPATH:  $(VPATH)"
