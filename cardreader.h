#ifndef CARDREADER_H
#define CARDREADER_H

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define false 0
#define true 1
typedef char bool;

#define ERROR -1

#define EXIT_OK 0
#define EXIT_ERROR_OPEN 1
#define EXIT_ERROR_GRAB 2

#define CARD_LENGTH 10

int open_device(char*);
int* read_card();
void close_device();

#endif // CARDREADER_H
