#include "table.h"

#include <unistd.h>
#include <fcntl.h>

namespace JiDB
{

    DiskMgr::DiskMgr(const char * filename) {
        fd = open(filename, O_RDWR | O_CREAT | O_EXCL, 644);
        if (fd < 0) {   // If file exists
            // Read header.
            lseek(fd, 0, SEEK_SET);
            ::read(fd, &header, sizeof(header));
        }

        

        // Setup helper data.
        num_free_pages = 0;
        off_t now_off = last_free_off = header.free_off;
        while (now_off) {
            num_free_pages++;
            page_t next_pg;
            lseek(fd, now_off, SEEK_SET);
            ::read(fd, &next_pg, sizeof(next_pg));
            off_t next_off = *(uint64_t *)next_pg.raw;
            last_free_off = now_off;
            now_off = next_off;
        }

        
    }

    DiskMgr::~DiskMgr() {
        lseek(fd, 0, SEEK_SET);
        ::write(fd, &header, sizeof(header));
        close(fd);
    }

    pageid_t DiskMgr::alloc() {
        if (num_free_pages < 3) {
            expand(50);
        }
        //
        num_free_pages--;
    }

    void DiskMgr::free(pageid_t id) {

    }

    void DiskMgr::read(pageid_t id, page_t & dest) {
        off_t off = get_offset(id);
        lseek(fd, off, SEEK_SET);
        ::read(fd, &dest, PAGE_SZ);
    }

    void DiskMgr::write(pageid_t id, const page_t & src) {
        off_t off = get_offset(id);
        lseek(fd, off, SEEK_SET);
        ::write(fd, &src, PAGE_SZ);
        fsync(fd);
    }

    pageid_t DiskMgr::get_pageid(off_t off) {
        return (pageid_t)off / PAGE_SZ;
    }

    // If there is no data page, return -1.
    // Else return 0.
    int DiskMgr::get_data_page(page_t & dest) {
        if (!header.data_off) return -1;
        lseek(fd, (off_t)header.data_off, SEEK_SET);
        ::read(fd, &dest, PAGE_SZ);
    }

    off_t DiskMgr::get_offset(pageid_t id) {
        return (off_t)id * PAGE_SZ;
    }

    void DiskMgr::expand(int num_pages) {
        
    }

}