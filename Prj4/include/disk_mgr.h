#pragma once

#include "api.h"
#include "common.h"
#include <unistd.h>

namespace DiskMgr {

    // Read page.
    int read_page(page_t * dest, tableid_t table_id);
    int read_page(page_t * dest, tableid_t table_id, pageid_t page_id);

    // 

    void     free_page(page_t & page);

    void     write(page_t & page);

    pageid_t get_pageid(tableid_t table_id, off_t offset);
    off_t    get_offset(tableid_t table_id, pageid_t page_id);

}