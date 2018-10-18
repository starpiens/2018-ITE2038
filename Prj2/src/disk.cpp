#include "table.h"

#include <unistd.h>
#include <fcntl.h>
#include <cstring>

namespace JiDB
{

    DiskMgr::DiskMgr(const char * filename) {
        fd = open(filename, O_RDWR | O_CREAT | O_EXCL, 644);
        if (fd < 0) {   // If file exists
            read_header();
        } else {
            
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
        off_t allocated_off = 0;
        // TODO
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
        if ((!header.data_off) ||
            (lseek(fd, (off_t)header.data_off, SEEK_SET) < 0) ||
            (::read(fd, &dest, PAGE_SZ) != PAGE_SZ)) {
            return -1;
        }
        return 0;
    }

    // off에 있는 페이지를 읽는다. 성공 여부를 반환한다.
    int DiskMgr::read_off(off_t off, page_t & dest) {
        if ((lseek(fd, off, SEEK_SET) < 0) ||
            (::read(fd, &dest, PAGE_SZ) != PAGE_SZ)) {
            return -1;
        }
        return 0;
    }

    // off에 있는 페이지에 쓴다. 성공 여부를 반환한다.
    int DiskMgr::write_off(off_t off, const page_t & src) {
        if (lseek(fd, off, SEEK_SET) < 0) {
            return -1;
        } else if (::write(fd, &src, PAGE_SZ) != PAGE_SZ) {
            return -1;
        } else if (fsync(fd) < 0) {
            return -1;
        } else {
            return 0;
        }
    }

    void DiskMgr::read_header() {
        page_t tmp;
        read_off(0, tmp);
        header = *reinterpret_cast<Header *>(&tmp);
    }

    void DiskMgr::write_header() {
        page_t tmp;
        *reinterpret_cast<Header *>(&tmp) = header;
        write_off(0, tmp);
    }

    off_t DiskMgr::get_offset(pageid_t id) {
        return (off_t)id * PAGE_SZ;
    }

    // num_pages만큼 페이지를 새로 만들고, 만들어진 페이지들을 free page list에 삽입한다.
    // 실제로 만들어진 페이지의 수를 리턴한다.
    int DiskMgr::expand(int num_pages) {
        // TODO
    }

}