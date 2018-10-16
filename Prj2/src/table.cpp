#include "table.h"
#include "bpt.h"
#include <fcntl.h>
#include <unistd.h>

namespace JiDB
{

    Table::Table(const char * filename, BASE_TYPE type = BPTree) {
        // Setup index manager.
        switch(type) {
        case BPTree: index_mgr = new BPT();
        case RTree:  index_mgr = nullptr;
        case GiST:   index_mgr = nullptr;
        default:     index_mgr = nullptr;
        }

        // Setup disk manager.
        disk_mgr = new DiskMgr(filename);
    }

    Table::~Table() { 
        delete index_mgr;
        delete disk_mgr;
    }

    value_t Table::_find(const key_t key) {
        return index_mgr->_find(key);
    }

    int Table::_insert(const key_t key, c_value_t value) {
        return index_mgr->_insert(key, value);
    }

    int Table::_delete(const key_t key) {
        return index_mgr->_delete(key);
    }

}