#include "bpt.h"

namespace JiDB 
{
    BPT::RAW_BPT_Page::RAW_BPT_Page(const DiskMgr & disk_mgr, const Node & node) {
        parent      = (uint64_t)disk_mgr.get_offset(node.parent);
        is_leaf     = node.is_leaf;
        num_of_keys = node.num_of_keys;
    }

    BPT::RAW_Leaf_Page::RAW_Leaf_Page(const DiskMgr & disk_mgr, const Leaf & leaf) :
            RAW_BPT_Page(disk_mgr, *static_cast<const Node *>(&leaf)) {
        right_sibling = (uint64_t)disk_mgr.get_offset(leaf.right_sibling);
        memcpy(records, leaf.records, sizeof(records));
    }

    BPT::RAW_Internal_Page::RAW_Internal_Page(const DiskMgr & disk_mgr, const Internal & internal) :
            RAW_BPT_Page(disk_mgr, *static_cast<const Node*>(&internal)) {
        leftmost_page = (off_t)disk_mgr.get_offset(internal.leftmost_page);
        for (int i = 0; i < 248; i++) {
            key_off_pairs[i].key      = internal.key_ptr_pairs[i].key;
            key_off_pairs[i].nxt_page = disk_mgr.get_offset(internal.key_ptr_pairs[i].nxt_page);
        }
    }

    BPT::Node::Node(const DiskMgr & disk_mgr, const page_t & page) {
        const RAW_BPT_Page & raw_page = *reinterpret_cast<const RAW_BPT_Page *>(&page);
        parent      = disk_mgr.get_pageid((off_t)raw_page.parent);
        is_leaf     = raw_page.is_leaf;
        num_of_keys = raw_page.num_of_keys;
    }

    BPT::Leaf::Leaf(const DiskMgr & disk_mgr, const page_t & page) : Node(disk_mgr, page) {
        const RAW_Leaf_Page & raw_leaf = *reinterpret_cast<const RAW_Leaf_Page *>(&page);
        right_sibling = disk_mgr.get_pageid((off_t)raw_leaf.right_sibling);
        memcpy(records, raw_leaf.records, sizeof(records));
    }

    BPT::Internal::Internal(const DiskMgr & disk_mgr, const page_t & page) : Node(disk_mgr, page) {
        const RAW_Internal_Page & raw_internal = *reinterpret_cast<const RAW_Internal_Page *>(&page);
        leftmost_page = disk_mgr.get_pageid((off_t)raw_internal.leftmost_page);
        for (int i = 0; i < 248; i++) {
            key_ptr_pairs[i].key      = raw_internal.key_off_pairs[i].key;
            key_ptr_pairs[i].nxt_page = disk_mgr.get_pageid(raw_internal.key_off_pairs[i].nxt_page);
        }
    }

    void BPT::Leaf::write(const DiskMgr & disk_mgr, pageid_t id) {
        RAW_Leaf_Page raw_leaf(disk_mgr, *this);
        disk_mgr.write(id, *reinterpret_cast<page_t *>(&raw_leaf));
    }

    void BPT::Internal::write(const DiskMgr & disk_mgr, pageid_t id) {
        RAW_Internal_Page raw_internal(disk_mgr, *this);
        disk_mgr.write(id, *reinterpret_cast<page_t *>(&raw_internal));
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
        uint left = 0, right = page.num_of_keys;
        while (left < right) {
            const uint mid = (left + right) >> 1;
            const key_t & mid_key = page.records[mid].key;
            if (key == mid_key) return mid;
            if (key < mid_key) right = mid - 1;
            else               left  = mid + 1;
        }
        return -1;
    }

    int BPT::find_lower_bound_in_internal(const Internal & page, const key_t key) {
        // [left, right)
        uint left = 0, right = page.num_of_keys;
        while (left < right) {
            const uint mid = (left + right + 1) >> 1;
            const key_t & mid_key = page.key_ptr_pairs[mid].key;
            if (key == mid_key) return mid;
            if (key < mid_key) right = mid;
            else               left  = mid + 1;
        }
        return left;
    }

    int BPT::_insert(const key_t key, const value_t value) {
        if (!root->is_leaf) {
            Internal & root_internal = *static_cast<Internal *>(root);
            KeyPtr * new_key = insert_into_internal(root_internal, key, value);
        }
        return 0;
    }

    /* Insert into subtree.
     * If split occurs, return a KeyPtr which need to be inserted in parent node.
     * Otherwise return nullptr.
     */
    BPT::KeyPtr * BPT::insert_into_internal(Internal & page, const key_t key, const value_t value) {
        pageid_t child_id = find_child(page, key);
        Node * child = get_node(child_id);
        KeyPtr * to_insert;
        if (child->is_leaf) {
            to_insert = insert_into_leaf(*static_cast<Leaf *>(child), key, value);

        }

        delete child;
        // return something;
    }

    BPT::KeyPtr * BPT::insert_into_leaf(Leaf & leaf, const key_t key, const value_t value) {

    }

    int BPT::_delete(const key_t key) {
        return 0;
    }

    // id를 가지고 노드를 읽어 온다! 
    // 리턴값은 Leaf 또는 Internal 객체다. is_leaf를 확인하여 적절히 캐스팅하여 사용해야 한다.
    BPT::Node * BPT::get_node(pageid_t id) const {
        // Read page from disk manager.
        page_t page;
        disk_mgr->read(id, page);
        RAW_BPT_Page & header = *reinterpret_cast<RAW_BPT_Page *>(&page);
        Node * node = header.is_leaf ? 
                      (static_cast<Node *>(new Leaf(*disk_mgr, page))) :
                      (static_cast<Node *>(new Internal(*disk_mgr, page)));
        return node;
    }

}