#include "file.h"
#include <stdio.h>
#include <unistd.h>

Page::Page(int fd, off_t off) : fd(fd), off(off) {
    
}

void Page::read() {
    lseek(fd, off, SEEK_SET);
    ::read(fd, &data, SIZE);
}

void Page::flush() {
    lseek(fd, off, SEEK_SET);
    ::write(fd, &data, SIZE);
}

int main() {
    return 0;
}