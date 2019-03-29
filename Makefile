CC:= gcc # compiler to be used
SOURCEDIR := src
BUILDDIR := build
TARGET := movieWatcher

SOURCES := $(shell find $(SOURCEDIR) -type f -name *.c)
OBJECTS := $(patsubst $(SOURCEDIR)/%,$(BUILDDIR)/%,$(SOURCES:.c=.o))
CFLAGS := -g -std=c11 # -Wall

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET)"; $(CC) $^ -o $(TARGET)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) -c -o $@ $<"; $(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo " Cleaning..."
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

test:
	$(CC) $(CFLAGS) $^ test/testing.c -o bin/testing

.PHONY: clean
