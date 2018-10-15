#include "file.h"
#include <fcntl.h>

namespace disk_mgr
{
    namespace  // private
    {
        class File {
        public:
            File(const char * path) {
                fd = ::open(path, O_RDWR | O_CREAT, 644);  
                //
            }

            ~File() {}

            // Convert page id to matching file offset.
            inline off_t pid_to_off(pageid_t pid) {
                return (off_t)pid * PAGE_SZ;   // No conversion in this version.
            }

            // allocate page id.
            inline pageid_t alloc_pageid(off_t off) {

            }

            // Free page id.
            inline void free_pageid(pageid_t pid) {
                ;   // Do nothing.
            }

            void expand(int num_page) {

            }

            int fd;          // File descriptor; -1 if not opened.
            off_t free_off;  // Points the head of free page list.
                             // 0, if there is no free page left.
            off_t data_off;  // Points the first data page.
            int n_pages;     // Number of pages.

            // TODO: Maybe on next version.
            // struct OffTable {
            //     pageid_t pid;
            //     off_t    off;
            // } off_table[128];

        };
    }  // private

    static File * db_file;

    // Open a file at path.
    int open_db(const char * path) {
        db_file = new File(path);
    }

    // Close a file with file id.
    void close_db() {
        delete db_file;
    }

    // Allocate an on-disk page from the free page list
    pageid_t alloc() {
        if (!db_file->free_off)
            db_file->expand(10);
        
    }

    // Free an on-disk page to the free page list
    void free(pageid_t pid) {

    }

    // Read an on-disk page into the in-memory page structure(dest)
    void read(pageid_t pid, page_t & dest) {
        off_t off = db_file->pid_to_off(pid);
        lseek(db_file->fd, off, SEEK_SET);
        ::read(db_file->fd, &dest, PAGE_SZ);
    }

    // Write an in-memory page(src) to the on-disk page
    void write(pageid_t pid, const page_t & src) {
        off_t off = db_file->pid_to_off(pid);
        lseek(db_file->fd, off, SEEK_SET);
        ::write(db_file->fd, &src, PAGE_SZ);
        fsync(db_file->fd);
    }

}  // namespace DiskMgr