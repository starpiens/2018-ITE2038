#pragma once

#include <inttypes.h>
#include <unistd.h>

namespace disk_mgr
{
    const int PAGE_SZ = 0x1000;

    typedef uint64_t pageid_t;
    typedef struct {
        char raw[PAGE_SZ];
    } page_t;

    int open_db(const char * path);
    void close_db();
    pageid_t alloc();
    void free(pageid_t pid);
    void read(pageid_t pid, page_t & dest);
    void write(pageid_t pid, const page_t & src);

}  // namespace DiskMgr