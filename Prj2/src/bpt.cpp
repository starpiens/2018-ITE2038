#include "bpt.h"

namespace JiDB 
{

    BPT::BPT(const char * filename) : IndexMgr(filename) {
        
    }

    BPT::~BPT() {
        delete root;
    }

    value_t BPT::_find(const key_t key) {
        
    }

    BPT::Node * BPT::get_node(pageid_t id) {
        // Read page from disk manager.
        page_t page;
        disk_mgr->read(id, page);

        // Make header
        RAW_BPT_Page * raw_header = reinterpret_cast<RAW_BPT_Page *>(&page);
        Node header;
        header.parent      = disk_mgr->get_pageid((off_t)raw_header->parent);
        header.is_leaf     = raw_header->is_leaf;
        header.num_of_keys = raw_header -> num_of_keys;
        
        // Is this leaf page?
        if (header.is_leaf) {
            RAW_Leaf_Page * raw_leaf = reinterpret_cast<RAW_Leaf_Page *>(&page);
            Leaf * leaf_node = new Leaf;

            *static_cast<Node *>(leaf_node) = header;
            leaf_node->right_sibling = disk_mgr->get_pageid((off_t)raw_leaf->right_sibling);
            memcpy(leaf_node->records, raw_leaf->records, sizeof(leaf_node->records));

            return static_cast<Node *>(leaf_node);
            
        } else {
            RAW_Internal_Page * raw_internal = reinterpret_cast<RAW_Internal_Page *>(&page);
            Internal * internal_node = new Internal;
            
            *static_cast<Node *>(internal_node) = header;
            internal_node->leftmost_page = disk_mgr->get_pageid((off_t)raw_internal->leftmost_page);
            for (int i = 0; i < 248; i++) {
                internal_node->key_ptr_pairs[i].key      
                    = raw_internal->key_off_pairs[i].key;
                internal_node->key_ptr_pairs[i].nxt_page 
                    = disk_mgr->get_pageid((off_t)raw_internal->key_off_pairs[i].nxt_page);
            }

            return static_cast<Node *>(internal_node);
        }
    }

    

}