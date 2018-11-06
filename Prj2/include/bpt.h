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
        static const int ORDER_INTERNAL = 248;
        static const int ORDER_LEAF     = 31;

        struct Node;
        struct Leaf;
        struct Internal;

        #pragma pack(push, 1)   // no align

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
            RAW_BPT_Page(const DiskMgr & disk_mgr, const Node & node);

            uint64_t parent;
            int      is_leaf;
            int      num_of_keys;
        
        private:
            char __reserved[104];
        };
    
        struct RAW_Leaf_Page : public RAW_BPT_Page {
            RAW_Leaf_Page(const DiskMgr & disk_mgr, const Leaf & leaf);

            uint64_t right_sibling;
            struct RAW_Record {
                int64_t key;
                value_t value;
            } records[ORDER_LEAF];
        };

        struct RAW_Internal_Page : public RAW_BPT_Page {
            RAW_Internal_Page(const DiskMgr & disk_mgr, const Internal & internal);

            int64_t leftmost_page;
            struct KeyOffPair {
                key_t    key;
                uint64_t nxt_page;
            } key_off_pairs[ORDER_INTERNAL];
        };

        struct Node {
            virtual ~Node() = 0;

            pageid_t parent;
            int      is_leaf;
            int      num_of_keys;
        
        protected:
            Node(const DiskMgr & disk_mgr, const page_t & page, pageid_t id);
            pageid_t id;
        };

        struct Leaf : public Node {
            Leaf(const DiskMgr & disk_mgr, const page_t & page, pageid_t id);
            ~Leaf() = default;

            void write(const DiskMgr & disk_mgr, pageid_t id);

            pageid_t right_sibling;
            Record   records[ORDER_LEAF];
        };

        struct Internal : public Node {
            Internal(const DiskMgr & disk_mgr, const page_t & page, pageid_t id);
            ~Internal() = default;

            void write(const DiskMgr & disk_mgr, pageid_t id);

            pageid_t leftmost_page;
            KeyPtr key_ptr_pairs[ORDER_INTERNAL];
        };

        #pragma pack(pop)

        Node * root;

        Node * get_node(pageid_t id) const;
        void free_node(Node & node);
        Leaf * alloc_leaf();
        Internal * alloc_internal();

        pageid_t find_child(const Internal & page, const key_t key);
        int find_in_leaf(const Leaf & page, const key_t key);
        int find_lower_bound_in_internal(const Internal & page, const key_t key);

        KeyPtr * insert_into_internal(Internal & page, const key_t key, const value_t value);
        KeyPtr * insert_into_leaf(Leaf & page, const key_t key, const value_t value);
    };

}