CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS =

SRCS = main.c cpu.c memory.c disk.c network.c load.c user.c battery.c firewall.c
OBJS = $(SRCS:.c=.o)

TARGET = sysmon

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all install uninstall clean
