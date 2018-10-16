#include "table.h"
#include "bpt.h"
#include <fcntl.h>
#include <unistd.h>

namespace JiDB
{

    Table::Table(const char * filename, BASE_TYPE type = BPTree) {
        switch(type) {
        case BPTree:
            // index_mgr = new BPT();

        case RTree:
        case GiST:
        default:
        }
    }

    Table::~Table() { 
        delete index_mgr;
        delete disk_mgr;
    }

    void * Table::_find(const void * key) {
        return index_mgr->_find(key);
    }

    int Table::_insert(const void * key, const void * value) {
        return index_mgr->_insert(key, value);
    }

    int Table::_delete(const void * key) {
        return index_mgr->_delete(key);
    }

}