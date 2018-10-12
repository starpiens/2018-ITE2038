#pragma once

#include <sys/types.h>

class Page {
public:
    Page(int, off_t);
    void read();
    void flush();

    static const int SIZE = 0x1000;
    
private:
    int     fd;
    off_t   off;
    char    data[SIZE];
};