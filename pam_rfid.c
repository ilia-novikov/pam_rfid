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

#include "pam_rfid.h"

char* get_cards_file(pam_handle_t*);
struct cards_t get_available_cards(pam_handle_t*, char*);
void free_cards(struct cards_t*);
bool compare(int*, int*);
int send_message(int, pam_handle_t*, char*);
int send_info(pam_handle_t*, char*);
int send_error(pam_handle_t*, char*);

PAM_EXTERN int pam_sm_authenticate(pam_handle_t* handle, int flags, int argc, const char** argv) {
    if (!argc) {
        send_error(handle, "No cardreader specified");
        return PAM_AUTH_ERR;
    }
    int timeout = DEFAULT_TIMEOUT;
    if (argc == 2) {
        timeout = atoi(argv[1]);
    }
    char* cards_file = get_cards_file(handle);
    if (!cards_file) {
        return PAM_AUTH_ERR;
    }
    struct cards_t list = get_available_cards(handle, cards_file);
    if (!list.cards) {
        free(cards_file);
        return PAM_AUTH_ERR;
    }
    int cardreader_error = open_device((char*) argv[0]);
    if (cardreader_error) {
        switch (cardreader_error) {
            case ERROR_OPEN: {
                send_error(handle, "Can't open cardreader");
                break;
            }

            case ERROR_GRAB: {
                send_error(handle, "Can't grab cardreader");
                break;
            }

            default: {
                send_error(handle, "Unknown device error");
                break;
            }
        }
        free(cards_file);
        free_cards(&list);
        return PAM_AUTH_ERR;
    }
    send_info(handle, "Waiting for card...");
    int* card = read_card(timeout);
    if (!card) {
        send_error(handle, "Cardreader timeout");
        free(cards_file);
        free_cards(&list);
        close_device();
        return PAM_AUTH_ERR;
    }
    send_info(handle, "Card has been successfully read");
    bool match_found = false;
    for (int i = 0; i < list.count; i++) {
        if (compare(card, list.cards[i])) {
            match_found = true;
            break;
        }
    }
    if (!match_found) {
        send_error(handle, "Card validation was failed");
        free(card);
        close_device();
        free_cards(&list);
        free(cards_file);
        return PAM_AUTH_ERR;
    }
    send_info(handle, "Card validation was successful");
    free(card);
    close_device();
    free_cards(&list);
    free(cards_file);
    return PAM_SUCCESS;
}

bool compare(int* a, int* b) {
    for (int i = 0; i < CARD_LENGTH; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

void free_cards(struct cards_t* list) {
    for (int i = 0; i < list->count; i++) {
        free(list->cards[i]);
    }
    free(list->cards);
}

char* get_cards_file(pam_handle_t* handle) {
    const char* username;
    int result;
    if ((result = pam_get_user(handle, &username, "Username: ")) != PAM_SUCCESS) {
        send_error(handle, "Can't acquire username");
        return NULL;
    }
    char* prefix = "/home/";
    char* postfix = "/.rfid.id";
    char* file = (char*) malloc(sizeof(char) * (strlen(prefix) + strlen(username) + strlen(postfix) + 1));
    file[0] = 0;
    strcat(file, prefix);
    strcat(file, username);
    strcat(file, postfix);
    return file;
}

struct cards_t get_available_cards(pam_handle_t* handle, char* cards_file) {
    struct cards_t result = {NULL, 0};
    FILE* file = fopen(cards_file, "r");
    if (!file) {
        send_error(handle, "Config file not found");
        return result;
    }
    char* line = NULL;
    ssize_t read_length;
    size_t length = 0;
    while ((read_length = getline(&line, &length, file)) != ERROR) {
        result.count++;
    }
    if (!result.count) {
        send_error(handle, "No cards found in config");
        return result;
    }
    rewind(file);
    result.cards = (int**) malloc(sizeof(int*) * result.count);
    int index = 0;
    while ((read_length = getline(&line, &length, file)) != ERROR) {
        int actual_length = read_length - 1;
        result.cards[index] = (int*) malloc(sizeof(int) * CARD_LENGTH);
        for (int i = 0; i < CARD_LENGTH; i++) {
            result.cards[index][i] = 0;
        }
        for (int i = 0; i < actual_length; i++) {
            result.cards[index][i] = line[i] - '0';
        }
        index++;
    }
    fclose(file);
    return result;
}

int send_message(int type, pam_handle_t* handle, char* text) {
    const struct pam_message message = {
		type,
		text
	};
    const struct pam_message* message_p = &message;
    struct pam_conv *conversation;
	struct pam_response *response;
	int r;
	if (pam_get_item(handle, PAM_CONV, (const void **) &conversation) != PAM_SUCCESS) {
        return ERROR;
    }
	if ((!conversation) || (!conversation->conv)) {
        return ERROR;
    }
	return conversation->conv(1, &message_p, &response, conversation->appdata_ptr);
}

int send_info(pam_handle_t* handle, char* text) {
    return send_message(PAM_TEXT_INFO, handle, text);
}

int send_error(pam_handle_t* handle, char* text) {
    return send_message(PAM_ERROR_MSG, handle, text);
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
    return (PAM_SUCCESS);
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
    return (PAM_SUCCESS);
}

PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
    return (PAM_SUCCESS);
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
    return (PAM_SUCCESS);
}
