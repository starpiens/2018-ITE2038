#include "table.h"
#include "bpt.h"

namespace JiDB
{

    Table::Table(const char * filename, BASE_TYPE type = BPTree) {
        switch(type) {
        case BPTree:
            index_mgr = new BPT();

        case RTree:
        case GiST:
        default:
        }
    }

    int Table::_insert(void * key, const void * value) {
        return index_mgr->_insert(key, value);
    }

}