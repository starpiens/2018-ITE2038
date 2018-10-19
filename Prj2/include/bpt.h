#pragma once

#include "table.h"
#include <inttypes.h>
#include <iostream>

namespace JiDB
{

    class BPT : public IndexMgr {
    public:
        BPT(const char * filename) : IndexMgr(filename) {}
        ~BPT() noexcept { delete root; }

        value_t * _find  (const key_t key);
        int       _insert(const key_t key, const value_t value);
        int       _delete(const key_t key);
        
    private: 
        struct Node;

        struct Record {
            Record() = default;
            Record(key_t key, value_t value) 
                : key(key), value(value) {}

            key_t   key;
            value_t value;
        };

        struct KeyPtr {
            KeyPtr() = default;
            KeyPtr(key_t key, pageid_t nxt_page)
                : key(key), nxt_page(nxt_page) {}

            key_t    key;
            pageid_t nxt_page;
        };

        struct RAW_BPT_Page {
            RAW_BPT_Page(const Node & node);

            uint64_t parent;
            int      is_leaf;
            int      num_of_keys;
        
        private:
            char __reserved[104];
        };
    
        struct RAW_Leaf_Page : public RAW_BPT_Page {
            uint64_t right_sibling;
            struct RAW_Record {
                int64_t key;
                value_t value;
            } records[31];
        };

        struct RAW_Internal_Page : public RAW_BPT_Page {
            int64_t leftmost_page;
            struct KeyOffPair {
                key_t    key;
                uint64_t nxt_page;
            } key_off_pairs[248];
        };

        struct Node {
            virtual ~Node() = 0;

            pageid_t parent;
            int      is_leaf;
            int      num_of_keys;
        
        protected:
            Node(const DiskMgr & disk_mgr, const page_t & page);
        };

        struct Leaf : public Node {
            Leaf(const DiskMgr & disk_mgr, const page_t & page);
            ~Leaf() = default;

            pageid_t right_sibling;
            Record   records[31];
        };

        struct Internal : public Node {
            Internal(const DiskMgr & disk_mgr, const page_t & page);
            ~Internal() = default;

            pageid_t leftmost_page;
            KeyPtr key_ptr_pairs[248];
        };

        Node * root;

        Node * get_node(pageid_t id) const;
        void free_node(Node & node);

        pageid_t find_child(const Internal & page, const key_t key);
        int find_in_leaf(const Leaf & page, const key_t key);
        int find_lower_bound_in_internal(const Internal & page, const key_t key);

        KeyPtr * insert_into_internal(Internal & page, const key_t key, const value_t value);
        KeyPtr * insert_into_leaf(Leaf & page, const key_t key, const value_t value);
    };

}