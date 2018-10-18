#include "bpt.h"

namespace JiDB 
{

    value_t BPT::_find(const key_t key) {
        
    }

    BPT::Node * BPT::get_node(pageid_t id) {
        // Read page from disk manager.
        page_t page;
        disk_mgr->read(id, page);
        
        // Is this leaf page?
        if ((*reinterpret_cast<RAW_BPT_Page *>(&page)).is_leaf) {

        } else {
            
        }
    }    

}