#pragma once

#include "api.h"
#include "disk_mgr.h"
#include <inttypes.h>

//==============================================================
// CONSTANTS
const int PAGE_DATA_SZ = 0x1000;

//==============================================================
// TYPES
using pageid_t = uint64_t;
struct page_t {
    page_t(const tableid_t table_id, const pageid_t page_id) :
        table_id(table_id), page_id(page_id) {}
    const tableid_t table_id;
    const pageid_t  page_id;
    bool            is_dirty = 0;
    char            data[PAGE_DATA_SZ];     // For syncing with disk.
};