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

#ifndef RFID_PAM_H
#define RFID_PAM_H

#include <security/pam_appl.h>
#include <security/pam_modules.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cardreader.h"

#define PAM_SM_AUTH

#define DEFAULT_TIMEOUT 10

struct cards_t {
    int** cards;
    int count;
};

#endif
