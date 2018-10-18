#pragma once

#include "table.h"
#include <inttypes.h>
#include <iostream>

namespace JiDB
{

    class BPT : public IndexMgr {
    public:
        BPT(const char * filename);
        ~BPT();

        value_t * _find  (const key_t key);
        int       _insert(const key_t key, const value_t value);
        int       _delete(const key_t key);
        
    private: 
        struct Record {
            Record() {}
            Record(key_t key, value_t value) 
                : key(key), value(value) {}

            key_t   key;
            value_t value;
        };

        struct RAW_BPT_Page {
            RAW_BPT_Page() {}
            virtual ~RAW_BPT_Page() {}

            uint64_t parent;
            int      is_leaf;
            int      num_of_keys;
        
        private:
            char __reserved[104];
        };
    
        struct RAW_Leaf_Page : public RAW_BPT_Page {
            RAW_Leaf_Page() {}
            virtual ~RAW_Leaf_Page() {}

            uint64_t right_sibling;
            struct RAW_Record {
                int64_t key;
                value_t value;
            } records[31];
        };

        struct RAW_Internal_Page : public RAW_BPT_Page {
        public:
            RAW_Internal_Page() {}
            virtual ~RAW_Internal_Page() {}

            int64_t leftmost_page;
            struct KeyOffPair {
                key_t    key;
                uint64_t nxt_page;
            } key_off_pairs[248];
        };

        struct Node {
        public:
            Node() {}
            virtual ~Node() {}

            pageid_t parent;
            int      is_leaf;
            int      num_of_keys;
        };

        struct Leaf : public Node {
            Leaf() {}
            virtual ~Leaf() {}

            pageid_t right_sibling;
            Record   records[31];
        };

        struct Internal : public Node {
            Internal() {}
            virtual ~Internal() {}

            pageid_t leftmost_page;
            struct KeyPtr {
                key_t    key;
                pageid_t nxt_page;
            } key_ptr_pairs[248];
        };

        Node * root;

        Node * get_node(pageid_t id);

        pageid_t find_child(const Internal & page, const key_t key);
        int find_in_leaf(const Leaf & page, const key_t key);
        int find_lower_bound_in_internal(const Internal & page, const key_t key);
    };

}