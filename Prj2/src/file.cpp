#include "file.h"
#include <fcntl.h>

namespace DiskMgr 
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

            // Convert page id to file offset.
            static inline off_t id_to_off(pageid_t pid) {
                return (off_t)pid;   // No conversion in this version.
            }

            void expand(int num_page) {

            }

            int fd;          // File descriptor; -1 if not opened.
            off_t free_off;  // Points the head of free page list.
                             // 0, if there is no free page left.
            off_t root_off;  // Points the root page.
            
            // Maybe on next version.
            // struct OffTable {
            //     pageid_t pid;
            //     off_t    off;
            // } off_table[128];

        } * files;
        int n_files;
        
    }  // private

    // Allocate an on-disk page from the free page list
    pageid_t alloc(fileid_t fid) {

    }

    // Free an on-disk page to the free page list
    void free(fileid_t fid, pageid_t pid) {

    }

    // Read an on-disk page into the in-memory page structure(dest)
    void read(fileid_t fid, pageid_t pid, page_t * dest) {
        if (!dest) return;
        off_t off = files[fid].id_to_off(pid);
        lseek(files[fid].fd, off, SEEK_SET);
        ::read(files[fid].fd, dest, PAGE_SZ);
    }

    // Write an in-memory page(src) to the on-disk page
    void write(fileid_t fid, pageid_t pid, const page_t * src) {
        if (!src) return;
        off_t off = files[fid].id_to_off(pid);
        lseek(files[fid].fd, off, SEEK_SET);
        ::write(files[fid].fd, src, PAGE_SZ);
    }

}  // namespace DiskMgr