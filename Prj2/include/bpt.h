#pragma once

#include "table.h"
#include <inttypes.h>
#include <iostream>

namespace JiDB
{

    class BPT : public IndexMgr {
    public:
        BPT();
        ~BPT();

        value_t _find  (const key_t key);
        int     _insert(const key_t key, c_value_t value);
        int     _delete(const key_t key);
        
    };

}