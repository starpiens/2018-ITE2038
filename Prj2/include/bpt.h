#pragma once

#include <stdint.h>

namespace bpt 
{
    typedef int64_t      key_t;
    typedef char *       value_t;
    typedef const char * c_value_t;

    int open_db(const char * path);
    int insert(key_t key, c_value_t value);
    value_t find(key_t key);
    int _delete(key_t key);

}  // namespace bpt