#ifndef READBUFFER_H
#define READBUFFER_H
#include <unistd.h>


int readbuffer(int fd, ssize_t size, char* buf, ssize_t& total_read);


#endif // READBUFFER_H
