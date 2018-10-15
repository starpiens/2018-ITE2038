#pragma once

#include <inttypes.h>
#include <unistd.h>

const int PAGE_SZ = 0x1000;

typedef uint64_t pageid_t;
typedef struct {
    char raw[PAGE_SZ];
} page_t;

namespace disk_mgr
{
    // Open a file at path.
    int open_db(const char * path);

    // Close a file with file id.
    void close_db();

    // Allocate an on-disk page from the free page list
    pageid_t alloc();

    // Free an on-disk page to the free page list
    void free(pageid_t pid);

    // Read an on-disk page into the in-memory page structure(dest)
    void read(pageid_t pid, page_t & dest);

    // Write an in-memory page(src) to the on-disk page
    void write(pageid_t pid, const page_t & src);

}  // namespace DiskMgr