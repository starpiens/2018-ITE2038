#include "table.h"

#include <unistd.h>
#include <fcntl.h>
#include <cstring>

namespace JiDB
{

    DiskMgr::DiskMgr(const char * filename) {
        fd = open(filename, O_RDWR | O_CREAT | O_EXCL, 644);

        if (fd >= 0) {  // Load header and setup helper data.
            read_header();
            setup_helper_data();

        } else {        // Make header.
            memset(&header, 0, sizeof(header));
            write_header();
        }
    }

    DiskMgr::~DiskMgr() {
        write_header();
        close(fd);
    }

    page_t & DiskMgr::alloc() {
        if(num_free_pages < 5) {
            expand(50);
        }

        page_t &   new_page = *new page_t;
        FreePage & free_new_page = *reinterpret_cast<FreePage *>(&new_page);
        read_off(header.free_off, new_page);
        header.free_off = free_new_page.next_off;
        write_header();
        num_free_pages--;
        
        return new_page;
    }

    // Make this page free page.
    void DiskMgr::free(page_t page) {
        FreePage & free_page = *reinterpret_cast<FreePage *>(&page);
        free_page.next_off = header.free_off;
        header.free_off = get_offset(page.id);
        write_header();
        num_free_pages++;
    }

    void DiskMgr::read(pageid_t id, page_t & dest) {
        off_t off = get_offset(id);
        lseek(fd, off, SEEK_SET);
        ::read(fd, &dest.raw, sizeof(dest.raw));
    }

    void DiskMgr::write(pageid_t id, const page_t & src) const {
        off_t off = get_offset(id);
        lseek(fd, off, SEEK_SET);
        ::write(fd, &src.raw, sizeof(src.raw));
        fsync(fd);
    }

    pageid_t DiskMgr::get_pageid(off_t off) const {
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
        if ((lseek(fd, off, SEEK_SET) < 0) ||
            (::write(fd, &src, PAGE_SZ) != PAGE_SZ) ||
            (fsync(fd) < 0)) {
            return -1;
        }
        return 0;
    }

    inline void DiskMgr::read_header() {
        page_t tmp;
        read_off(0, tmp);
        header = *reinterpret_cast<HeaderPage *>(&tmp);
    }

    inline void DiskMgr::write_header() {
        page_t tmp;
        *reinterpret_cast<HeaderPage *>(&tmp) = header;
        write_off(0, tmp);
    }

    inline void DiskMgr::setup_helper_data() {
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

    off_t DiskMgr::get_offset(pageid_t id) const {
        return (off_t)id * PAGE_SZ;
    }

    // num_pages만큼 페이지를 새로 만들고, 만들어진 페이지들을 free page list에 삽입한다.
    // 실제로 만들어진 페이지의 수를 리턴한다.
    inline int DiskMgr::expand(int num_pages) {
        int ret = lseek(fd, num_pages * PAGE_SZ, SEEK_END);
        if (ret < 0) return 0;
        int expanded_sz = ret - header.num_pages * PAGE_SZ;
        int expanded_pg =  expanded_sz / PAGE_SZ;
        header.num_pages += expanded_pg;
        num_free_pages++;
        return expanded_pg;
    }

}