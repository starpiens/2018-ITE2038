#include "bpt.h"

namespace JiDB 
{

    BPT::BPT(const char * filename) : IndexMgr(filename) {
        disk_mgr = new DiskMgr(filename);
    }

    BPT::~BPT() {
        delete root;
    }

    value_t * BPT::_find(const key_t key) {
        if (!root) return nullptr;
        Node * ptr_node = root;
        while (!ptr_node->is_leaf) {
            Internal & internal = *static_cast<Internal *>(ptr_node);
            pageid_t nxt_page = find_child(internal, key);
            if (ptr_node != root) delete ptr_node;
            ptr_node = get_node(nxt_page);
        }

        Leaf & leaf = *static_cast<Leaf *>(ptr_node);
        int idx = find_in_leaf(leaf, key);
        value_t * ret = nullptr;
        if (idx >= 0) {
            ret = new value_t;
            memcpy(ret, &leaf.records[idx].value, sizeof(value_t));
        }
        delete ptr_node;
        return ret;
    }

    pageid_t BPT::find_child(const Internal & page, const key_t key) {
        int idx = find_lower_bound_in_internal(page, key);
        return (idx < 0) ? page.leftmost_page : page.key_ptr_pairs[idx].nxt_page;
    }

    /* Find index of key in leaf_page.
    * If found, return index of the key. Otherwise, return -1.
    */
    int BPT::find_in_leaf(const Leaf & page, const key_t key) {
        // [left, right)
        int left = 0, right = page.num_of_keys;
        while (left < right) {
            int mid = (left + right) >> 1;
            key_t mid_key = page.records[mid].key;
            if (key == mid_key) return mid;
            if (key < mid_key) right = mid - 1;
            else               left  = mid + 1;
        }
        return -1;
    }

    int BPT::find_lower_bound_in_internal(const Internal & page, const key_t key) {
        // [left, right)
        int left = 0, right = page.num_of_keys;
        while (left < right) {
            int mid = (left + right) >> 1;
            key_t mid_key = page.key_ptr_pairs[mid].key;
            if (key == mid_key) return mid;
            if (key < mid_key) right = mid;
            else               left  = mid;
        }
    }

    // id를 가지고 노드를 읽어 온다! 
    // 리턴값은 Leaf 또는 Internal 객체다.
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