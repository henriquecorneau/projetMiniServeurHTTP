CC = gcc

SRCDIR = src
INCDIR = include
BUILDDIR = build

CFLAGS = -I$(INCDIR) -Wall

TARGET_SERVER = serveur
TARGET_CLIENT = client

OBJS_SERVER = $(BUILDDIR)/serveur.o $(BUILDDIR)/reseau.o
OBJS_CLIENT = $(BUILDDIR)/client.o $(BUILDDIR)/reseau.o

all: $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_SERVER): $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o $@ $(OBJS_SERVER)

$(TARGET_CLIENT): $(OBJS_CLIENT)
	$(CC) $(CFLAGS) -o $@ $(OBJS_CLIENT)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(TARGET_SERVER) $(TARGET_CLIENT)

.PHONY: multi clean