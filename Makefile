CC = gcc
CFLAGS = -fPIC -Dpic -shared -rdynamic
FILES = pam_rfid.c cardreader.c
OUTPUT = pam_rfid.so

all:
	$(CC) $(CFLAGS) -o $(OUTPUT) $(FILES)

install:
	cp $(OUTPUT) /lib/security/

clean:
	if [ -f $(OUTPUT) ]; then \
		rm $(OUTPUT); \
	fi
