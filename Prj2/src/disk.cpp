#include "table.h"

#include <unistd.h>
#include <fcntl.h>

namespace JiDB
{

    DiskMgr::DiskMgr(const char * filename) {
        fd = open(filename, O_RDWR | O_CREAT, 644);
        lseek(fd, 0, SEEK_SET);
        ::read(fd, &header, sizeof(header));
    }

    DiskMgr::~DiskMgr() {
        close(fd);
    }

    pageid_t DiskMgr::alloc() {

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

    void DiskMgr::get_data(page_t & dest) {

    }

    off_t DiskMgr::get_offset(pageid_t id) {
        return (off_t)id * PAGE_SZ;
    }

    void DiskMgr::expand(int num_pages) {
        
    }

}