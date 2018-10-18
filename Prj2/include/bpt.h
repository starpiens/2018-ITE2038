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

        value_t _find  (const key_t key);
        int     _insert(const key_t key, c_value_t value);
        int     _delete(const key_t key);
        
    private: 
        struct Record {
            key_t   key;
            value_t value;
        };

        struct RAW_BPT_Page {
        public:
            uint64_t parent;
            int      is_leaf;
            int      num_of_keys;
        
        private:
            char __reserved[104];
        };
    
        struct RAW_Leaf_Page : public RAW_BPT_Page {
        public:
            uint64_t right_sibling;
            struct RAW_Record {
                int64_t key;
                char value[120];
            } records[31];
        };

        struct RAW_Internal_Page : public RAW_BPT_Page {
        public:
            int64_t leftmost_page;
            struct KeyOffPair {
                key_t    key;
                uint64_t nxt_page;
            } key_off_pairs[248];
        };

        struct Node {
        public:
            pageid_t parent;
            int      is_leaf;
            int      num_of_keys;
        };

        struct Leaf : public Node {
            pageid_t right_sibling;
            Record   records[31];
        };

        struct Internal : public Node {
            pageid_t leftmost_page;
            struct KeyPtr {
                key_t    key;
                pageid_t nxt_page;
            } key_ptr_pairs[248];
        };

        Node * root;

        Node * get_node(pageid_t id);
    };

}