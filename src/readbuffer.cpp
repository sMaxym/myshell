#include "../include/readbuffer.h"
#include <iostream>

int readbuffer(int fd, ssize_t size, char* buf, ssize_t& total_read) {

    ssize_t x = 0;
    while (x < size) {
        ssize_t add_size = read(fd, buf + x, size - x);

        if (add_size == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        else if (add_size == 0) {
            break;
        }
        x += add_size;
    }
    total_read = x;
    return 0;
}
