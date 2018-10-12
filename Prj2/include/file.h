#pragma once

typedef uint64_t pageid_t;

typedef struct {
    char a[0x1000];
} page_t;

// Static class
class DiskMgr {
public:
    // Allocate an on-disk page from the free page list
    static pageid_t alloc();
    
    // Free an on-disk page to the free page list
    static void free(pageid_t id);

    // Read an on-disk page into the in-memory page structure(dest)
    static void read(pageid_t id, page_t * dest);

    // Write an in-memory page(src) to the on-disk page
    static void write(pageid_t id, const page_t * src);
    
private:
    DiskMgr() {}
    
    // Convert page id to file offset.
    off_t id_to_off(pageid_t id);
};