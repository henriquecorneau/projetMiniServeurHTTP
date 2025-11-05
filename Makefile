CC = gcc

SRCDIR = src
INCDIR = include
BUILDDIR = build

CFLAGS = -I$(INCDIR) -Wall

TARGET = main

OBJ = $(BUILDDIR)/main.o $(BUILDDIR)/reseau.o $(BUILDDIR)/serveur.o

all: $(TARGET) $(TARGET_CLIENT)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(TARGET)

.PHONY: multi clean