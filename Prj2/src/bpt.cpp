#include "bpt.h"
#include "file.h"

namespace bpt
{
    namespace  // private
    {
        struct Header {
            off_t parent_off;
            bool  is_leaf;
            int   n_keys;
            off_t off;  // In leaf page, right sibiling.
                        // In internal page, one more page (< key(0)).
        };

        struct Leaf : public Header {
            Leaf(disk_mgr::pageid_t pid) {
                this->id = pid;
                disk_mgr::page_t d_page;
                disk_mgr::read(pid, d_page);
            }

            ~Leaf() {   // 임시
                disk_mgr::page_t d_page;
                disk_mgr::write(id, d_page);
            }

            disk_mgr::pageid_t id;
            struct Record {
                key_t   key;
                value_t value;
            } records[31];
        };

        struct Internal : public Header {
            struct KeyOff {
                key_t key;
                off_t off;
            } key_offs[248];
        };
    }  // private

    typedef struct Header * Node;

    static Node root = nullptr;

    // Initialization
    int open_db(char * path) {
        disk_mgr::pageid_t root_pid = disk_mgr::open_db(path);
        disk_mgr::page_t d_page;
        disk_mgr::read(root_pid, d_page);
        if (*(int *)(d_page.raw + 8)) {    // is root leaf?
            root = new Leaf(root_pid);
        } 
    }

    /******************************        ******************************/
    /*****************************   FIND   *****************************/
    /******************************        ******************************/
    value_t find(key_t key) {
        
        
    }

} // bpt