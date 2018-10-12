#pragma once

#include <inttypes.h>
#include <unistd.h>

const int PAGE_SZ = 0x1000;

typedef uint64_t fileid_t;
typedef uint64_t pageid_t;
typedef struct {
    char raw[PAGE_SZ];
} page_t;

namespace DiskMgr 
{
    // Allocate an on-disk page from the free page list
    pageid_t alloc(fileid_t fid);

    // Free an on-disk page to the free page list
    void free(fileid_t fid, pageid_t pid);

    // Read an on-disk page into the in-memory page structure(dest)
    void read(fileid_t fid, pageid_t pid, page_t * dest);

    // Write an in-memory page(src) to the on-disk page
    void write(fileid_t fid, pageid_t pid, const page_t * src);

}  // namespace DiskMgr