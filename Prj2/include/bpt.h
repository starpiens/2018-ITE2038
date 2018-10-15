#pragma once

#include <stdint.h>

namespace bpt 
{

    typedef int64_t      key_t;
    typedef char *       value_t;
    typedef const char * c_value_t;

    struct Header {
        off_t parent_off;
        bool  is_leaf;
        int   n_keys;
        off_t off;  // In leaf page, right sibiling.
                    // In internal page, one more page (< key(0)).
    };

    struct Leaf : public Header {
        struct Record {
            key_t   key;
            value_t value;
        } records[31];
    };

    struct Internal : public Header {
        struct KeyOff {
            key_t key;
            off_t off;
        } key_offs[248];
    };

    int open_db(const char * path);
    int insert(key_t key, c_value_t value);
    value_t find(key_t key);
    int _delete(key_t key);

}  // namespace bpt