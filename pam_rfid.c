#include "pam_rfid.h"

char* get_cards_file(pam_handle_t*);
struct cards_t get_available_cards(char*);
void free_cards(struct cards_t*);
bool compare(int*, int*);

PAM_EXTERN int pam_sm_authenticate(pam_handle_t* handle, int flags, int argc, const char** argv) {
    if (!argc) {
        printf("[pam_rfid] No cardreader specified\n");
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
    struct cards_t list = get_available_cards(cards_file);
    if (!list.cards) {
        free(cards_file);
        return PAM_AUTH_ERR;
    }
    int cardreader_error = open_device((char*) argv[0]);
    if (cardreader_error) {
        free(cards_file);
        free_cards(&list);
        return PAM_AUTH_ERR;
    }
    int* card = read_card(timeout);
    if (card == NULL) {
        free(cards_file);
        free_cards(&list);
        close_device();
        return PAM_AUTH_ERR;
    }
    bool match_found = false;
    for (int i = 0; i < list.count; i++) {
        if (compare(card, list.cards[i])) {
            match_found = true;
            break;
        }
    }
    if (!match_found) {
        printf("[pam_rfid] Card validation was failed\n");
        free(card);
        close_device();
        free_cards(&list);
        free(cards_file);
        return PAM_AUTH_ERR;
    }
    printf("[pam_rfid] Card validation was successful\n");
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
        printf("[pam_rfid] Can't acquire username, aborting\n");
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

struct cards_t get_available_cards(char* cards_file) {
    struct cards_t result = {NULL, 0};
    FILE* file = fopen(cards_file, "r");
    if (!file) {
        printf("[pam_rfid] Cards' file not found: %s\n", cards_file);
        return result;
    }
    char* line = NULL;
    ssize_t read_length;
    size_t length = 0;
    while ((read_length = getline(&line, &length, file)) != ERROR) {
        result.count++;
    }
    if (!result.count) {
        printf("[pam_rfid] No cards found in file: %s\n", cards_file);
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
