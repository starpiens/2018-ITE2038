#pragma once

#include "api.h"
#include <inttypes.h>

const int PAGE_SZ = 0x1000;

using pageid_t = uint64_t;
using page_t   = struct {
    tableid_t table_id;
    pageid_t  page_id;
    char      data[PAGE_SZ];
};