CC:= gcc # compiler to be used
SOURCEDIR := src
BUILDDIR := build
TARGET := movieWatcher

SOURCES := $(shell find $(SOURCEDIR) -type f -name *.c)
OBJECTS := $(patsubst $(SOURCEDIR)/%,$(BUILDDIR)/%,$(SOURCES:.c=.o))
CFLAGS := -g -std=c11 # -Wall
LIB := -lncurses -lpanel -lmenu
INC := -I include

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

test:
	$(CC) $(CFLAGS) $^ test/testing.c $(INC) $(LIB) -o bin/testing

.PHONY: clean
