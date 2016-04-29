/*

This file is part of pam_rfid.

pam_rfid is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

pam_rfid is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with pam_rfid. If not, see <http://www.gnu.org/licenses/>.

Ilia Novikov, <ilia.novikov at live.ru>

*/

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

#define ERROR_OPEN 1
#define ERROR_GRAB 2

#define CARD_LENGTH 10

int open_device(char*);
int* read_card();
void close_device();

#endif // CARDREADER_H
