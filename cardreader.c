#include "cardreader.h"

int descriptor = ERROR;
bool initialized = false;

void close_device() {
    if (descriptor != ERROR) {
        ioctl(descriptor, EVIOCGRAB, NULL);
        close(descriptor);
    }
}

int* read_card(time_t timeout) {
    if (!initialized) {
        return NULL;
    }
    int index = 0;
    bool pass = false;
    int* scanned = malloc(sizeof(int) * CARD_LENGTH);
    int scanned_count = 0;
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(descriptor, &fd);
    struct timeval delay = {timeout, 0};
    while (scanned_count < CARD_LENGTH) {
        int ready = select(descriptor + 1, &fd, NULL, NULL, &delay);
        if ((!ready) || (ready == ERROR)) {
            return NULL;
        }
        struct input_event event;
        int bytes = read(descriptor, &event, sizeof(struct input_event));
        if ((!bytes) || (bytes == ERROR)) {
            continue;
        }
        switch (index) {
            case 0:
                index++;
                break;
            case 1:
                index++;
                if (!pass) {
                    int digit = ERROR;
                    if ((event.code >= 2) && (event.code <= 10)) {
                        digit = event.code - 1;
                    }
                    if (event.code == 11) {
                        digit = 0;
                    }
                    if (digit == ERROR) {
                        continue;
                    }
                    scanned[scanned_count++] = digit;
                }
                pass = !pass;
                break;
            case 2:
                index = 0;
                break;
        }
    }
    return scanned;
}

int open_device(char* path)
{
    initialized = false;
    descriptor = open(path, O_RDONLY);
    if (descriptor == ERROR) {
        return ERROR_OPEN;
    }
    int error = ioctl(descriptor, EVIOCGRAB, 1);
    if (error) {
        return ERROR_GRAB;
    }
    initialized = true;
    return 0;
}
