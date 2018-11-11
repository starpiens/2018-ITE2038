#include "table.h"
#include "bpt.h"

#include <algorithm>

using namespace JiDB;

namespace JiDB {

    /******************************************/
    /************ HELPER FUNCTIONS ************/
    /******************************************/

    // id를 가지고 노드를 읽어 온다! 
    // 리턴값은 Leaf 또는 Internal 객체다. is_leaf를 확인하여 적절히 캐스팅하여 사용해야 한다.
    BPT::Node * BPT::get_node(pageid_t id) const {
        page_t page;
        if (disk_mgr->read(id, page) < 0) {
            return nullptr;
        }
        RAW_BPT_Page & header = *reinterpret_cast<RAW_BPT_Page *>(page.raw);
        return header.is_leaf ?
            static_cast<Node *>(&page_to_leaf(page)) :
            static_cast<Node *>(&page_to_internal(page));
    }

    // page_t -> Node
    inline void BPT::page_to_node(const page_t & page, Node & node) const {
        node.id = page.id;
        const RAW_BPT_Page & raw_page = *reinterpret_cast<const RAW_BPT_Page *>(page.raw);
        node.parent      = disk_mgr->get_pageid(raw_page.parent);
        node.is_leaf     = raw_page.is_leaf;
        node.num_of_keys = raw_page.num_of_keys;
    }

    // page_t -> Leaf
    BPT::Leaf & BPT::page_to_leaf(const page_t & page) const {
        Leaf & leaf = *new Leaf;
        page_to_node(page, *static_cast<Node *>(&leaf));
        const RAW_Leaf_Page & raw_page = *reinterpret_cast<const RAW_Leaf_Page *>(page.raw);
        leaf.right_sibling = disk_mgr->get_pageid(raw_page.right_sibling);
        memcpy(leaf.records, raw_page.records, sizeof(raw_page.records));
        return leaf;
    }

    // page_t -> Internal
    BPT::Internal & BPT::page_to_internal(const page_t & page) const {
        Internal & internal = *new Internal;
        page_to_node(page, *static_cast<Node *>(&internal));
        const RAW_Internal_Page & raw_page = *reinterpret_cast<const RAW_Internal_Page *>(page.raw);
        internal.leftmost_page = disk_mgr->get_pageid(raw_page.leftmost_page);
        for (int i = 0; i < raw_page.num_of_keys; i++) {
            internal.key_ptr_pairs[i].key      = raw_page.key_off_pairs[i].key;
            internal.key_ptr_pairs[i].nxt_page = disk_mgr->get_pageid(raw_page.key_off_pairs[i].nxt_page);
        }
        return internal;
    }

    // Write node to disk.
    void BPT::write_node(const Node & node) const {
        page_t & page = node.is_leaf ?
            leaf_to_page(*static_cast<const Leaf *>(&node)) :
            internal_to_page(*static_cast<const Internal *>(&node));
        disk_mgr->write(page);
    }

    // Node -> page_t
    inline void BPT::node_to_page(const Node & node, page_t & page) const {
        page.id = node.id;
        RAW_BPT_Page & raw_page = *reinterpret_cast<RAW_BPT_Page *>(page.raw);
        raw_page.parent      = (uint64_t)disk_mgr->get_offset(node.parent);
        raw_page.is_leaf     = node.is_leaf;
        raw_page.num_of_keys = node.num_of_keys;
    }

    // Leaf -> page_t
    page_t & BPT::leaf_to_page(const Leaf & leaf) const {
        page_t & page = *new page_t;
        node_to_page(*static_cast<const Node *>(&leaf), page);
        RAW_Leaf_Page & raw_page = *reinterpret_cast<RAW_Leaf_Page *>(page.raw);
        raw_page.right_sibling = disk_mgr->get_offset(leaf.right_sibling);
        memcpy(raw_page.records, leaf.records, sizeof(leaf.records));
        return page;
    }

    page_t & BPT::internal_to_page(const Internal & internal) const {
        page_t & page = *new page_t;
        node_to_page(*static_cast<const Node *>(&internal), page);
        RAW_Internal_Page & raw_page = *reinterpret_cast<RAW_Internal_Page *>(page.raw);
        raw_page.leftmost_page = disk_mgr->get_offset(internal.leftmost_page);
        for (int i = 0; i < internal.num_of_keys; i++) {
            raw_page.key_off_pairs[i].key      = internal.key_ptr_pairs[i].key;
            raw_page.key_off_pairs[i].nxt_page = disk_mgr->get_offset(raw_page.key_off_pairs[i].nxt_page);
        }
    }

    // Internal -> page_t
    page_t & BPT::internal_to_page(const Internal & node) const {
        page_t & page = *new page_t;
        page.id = node.id;
        RAW_Internal_Page & internal_raw = *reinterpret_cast<RAW_Internal_Page *>(page.raw);
        internal_raw.parent        = (uint64_t)disk_mgr->get_offset(node.parent);
        internal_raw.is_leaf       = node.is_leaf;
        internal_raw.num_of_keys   = node.num_of_keys;
        internal_raw.leftmost_page = (uint64_t)disk_mgr->get_offset(node.leftmost_page);
        for (int i = 0; i < ORDER_INTERNAL; i++) {
            internal_raw.key_off_pairs[i].key      = node.key_ptr_pairs[i].key;
            internal_raw.key_off_pairs[i].nxt_page = disk_mgr->get_offset(
                node.key_ptr_pairs[i].nxt_page
            );
        }
        return page;
    }

    /******************************************/
    /************ B+ TREE FUNCTIONS ***********/
    /******************************************/
    
    // Find value by key.
    value_t * BPT::_find(const key_t & key) {
        if (!root) return nullptr;
        Node * ptr_node = root;

        // Find leaf node.
        while (!ptr_node->is_leaf) {
            Internal & internal = *static_cast<Internal *>(ptr_node);
            pageid_t nxt_page = find_child(internal, key);
            if (ptr_node != root) delete ptr_node;
            ptr_node = get_node(nxt_page);
        }

        Leaf & leaf = *static_cast<Leaf *>(ptr_node);
        int idx = find_lower_bound_in_leaf(leaf, key);
        value_t * ret = nullptr;
        if (idx != leaf.num_of_keys && leaf.records[idx].key == key) {
            ret = new value_t;
            memcpy(ret, &leaf.records[idx].value, sizeof(value_t));
        }
        delete ptr_node;

        return ret;
    }

    // Find child which may contain key in internal node.
    pageid_t BPT::find_child(const Internal & page, const key_t & key) {
        int idx = find_lower_bound_in_internal(page, key);
        return (idx < 0) ? page.leftmost_page : page.key_ptr_pairs[idx].nxt_page;
    }

    /* Find a lower bound of key in the leaf page.
     * Returns an index of the first record in the leaf page which does not compare less than key.
     * If all keys in the leaf page compare less than key, the function returns last index (page.num_of_keys).
     */
    inline int BPT::find_lower_bound_in_leaf(const Leaf & page, const key_t & key) {
        const Record * ptr = std::lower_bound(page.records, page.records + page.num_of_keys, key,
            [](const Record & lhs, const key_t & rhs) -> bool { return lhs.key < rhs; });
        return ptr - page.records;
    }

    /* Find a lower bound of key in the internal page.
     * Returns an index of the first record in the internal page which does not compare less than key.
     * If all keys in the internal page compare less than key, the function returns last index (page.num_of_keys).
     */
    inline int BPT::find_lower_bound_in_internal(const Internal & page, const key_t & key) {
        const KeyPtr * ptr = std::lower_bound(page.key_ptr_pairs, page.key_ptr_pairs + page.num_of_keys, key,
            [](const KeyPtr & lhs, const key_t & rhs) -> bool { return lhs.key < rhs; });
        return ptr - page.key_ptr_pairs;
    }

    int BPT::_insert(const key_t & key, const value_t & value) {
        KeyPtr * key_ptr_from_root = root->is_leaf ?
            insert_into_leaf(*static_cast<Leaf *>(root), key, value) :
            insert_into_internal(*static_cast<Internal *>(root), key, value);

        // Split happened at root.
        if (key_ptr_from_root) {
            // TODO: Split root.
        }
        return 0;
    }

    /* Insert into subtree.
     * If split occurs, return a KeyPtr which need to be inserted in parent node.
     * Otherwise return nullptr.
     */
    BPT::KeyPtr * BPT::insert_into_internal(Internal & page, const key_t & key, const value_t & value) {
        pageid_t child_id = find_child(page, key);
        Node * child = get_node(child_id);
        KeyPtr * key_ptr_from_child = child->is_leaf ?
            insert_into_leaf(*static_cast<Leaf *>(child), key, value) :
            insert_into_internal(*static_cast<Internal *>(child), key, value);
        
        // Split occured in child.
        if (key_ptr_from_child) {

            if (page.num_of_keys == ORDER_INTERNAL) {
                int ret = find_lower_bound_in_internal(page, key);
                
            } else {

            }
        } else ; // no split, do nothing.

        delete child;
        return nullptr;
    }

    // TODO: 이 함수를 Node의 멤버함수로.
    BPT::KeyPtr * BPT::insert_into_leaf(Leaf & page, const key_t & key, const value_t & value) {
        int key_idx = find_lower_bound_in_leaf(page, key);
        if (page.records[key_idx].key == key) {     // Key duplication
            return nullptr;
        }

        if (page.num_of_keys == ORDER_LEAF) {   // Leaf is full, split.
            Leaf & new_leaf = page_to_leaf(disk_mgr->alloc());
            int split_idx = ORDER_LEAF >> 1;
            int is_right = key_idx >= split_idx;
            

        } else {
            // Shift elements.
            std::for_each(page.records + key_idx, page.records + page.num_of_keys, 
                [](Record & record) -> void { memcpy(&record + 1, &record, sizeof(record)); });
            page.records[key_idx].key   = key;
            page.records[key_idx].value = value;
            return nullptr;
        }
    }

    int BPT::_delete(const key_t & key) {
        return 0;
    }

}