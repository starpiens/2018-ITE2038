#pragma once

#include <inttypes.h>

const int PAGE_SZ = 0x1000;

using pageid_t = uint64_t;
using page_t   = struct {
    pageid_t id;
    char     data[PAGE_SZ];
};