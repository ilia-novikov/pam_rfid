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
