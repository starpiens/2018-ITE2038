#include "table.h"

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/mman.h>

namespace JiDB
{

    DiskMgr::DiskMgr(const char * filename) {
        fd = open(filename, O_RDWR | O_CREAT | O_EXCL, 644);

        // Mapping header.
        // 이 위치에 쓰는 내용은 DB파일의 헤더 부분에 자동으로 기록되지만,
        // 즉시 동기화되지는 않는다. 동기화를 보장하려면 msync를 사용한다.
        header = reinterpret_cast<HeaderPage *>(
            mmap(NULL, 
                 sizeof(HeaderPage), 
                 PROT_READ | PROT_WRITE,
                 MAP_FILE | MAP_PRIVATE,
                 fd,
                 0)
        );

        if (fd < 0) {
            memset(header, 0, sizeof(HeaderPage));
            msync(header, sizeof(HeaderPage), MS_SYNC);
        }
    }

    DiskMgr::~DiskMgr() {
        close(fd);
    }

    // Allocate new page from free page list.
    page_t * DiskMgr::alloc() {
        // If no page left, expand database file.
        if (!header->free_off) {
            expand(50);
        }

        page_t *   new_page = new page_t;
        FreePage & free_new_page = *reinterpret_cast<FreePage *>(new_page);
        read(get_pageid(header->free_off), *new_page);
        header->free_off = free_new_page.next_off;
        memset(&new_page->raw, 0, PAGE_SZ);
        
        return new_page;
    }

    // Make this page free page.
    void DiskMgr::free(page_t page) {
        FreePage & free_page = *reinterpret_cast<FreePage *>(&page);
        free_page.next_off = header->free_off;
        header->free_off = get_offset(page.id);
    }

    // Read page which is at given offset. 
    // If success, return 0. Otherwise, return -1.
    int DiskMgr::read(pageid_t id, page_t & dest) {
        off_t off = get_offset(id);
        if ((lseek(fd, off, SEEK_SET) < 0) ||
            (::read(fd, &dest.raw, PAGE_SZ) != PAGE_SZ)) {
            return -1;
        }
        dest.id = id;
        return 0;
    }

    // Write page at given offset.
    // If success, return 0. Otherwise, return -1.
    int DiskMgr::write(const page_t & src) const {
        off_t off = get_offset(src.id);
        if ((lseek(fd, off, SEEK_SET) < 0) ||
            (::write(fd, &src.raw, PAGE_SZ) != PAGE_SZ) ||
            (fsync(fd) < 0)) {
            return -1;
        }
        return 0;
    }

    // offset -> pageid.
    pageid_t DiskMgr::get_pageid(off_t off) const {
        return (pageid_t)off / PAGE_SZ;
    }

    // pageid -> offset.
    off_t DiskMgr::get_offset(pageid_t id) const {
        return (off_t)id * PAGE_SZ;
    }

    // If there is no data page, return -1.
    // Else return 0.
    int DiskMgr::get_data_page(page_t & dest) {
        if ((!header->data_off) ||
            (lseek(fd, (off_t)header->data_off, SEEK_SET) < 0) ||
            (::read(fd, &dest.raw, PAGE_SZ) != PAGE_SZ)) {
            return -1;
        }
        dest.id = get_pageid(header->data_off);
        return 0;
    }

    // num_pages만큼 페이지를 새로 만들고, 만들어진 페이지들을 free page list에 삽입한다.
    // 실제로 만들어진 페이지의 수를 리턴한다.
    inline int DiskMgr::expand(int num_pages) {
        int ret = lseek(fd, num_pages * PAGE_SZ, SEEK_END);
        if (ret < 0) return 0;
        int expanded_sz = ret - header->num_pages * PAGE_SZ;
        int expanded_pg =  expanded_sz / PAGE_SZ;
        header->num_pages += expanded_pg;
        return expanded_pg;
    }

}