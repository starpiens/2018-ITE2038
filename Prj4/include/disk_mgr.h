#pragma once

#include "api.h"
#include "common.h"
#include <unistd.h>

namespace DiskMgr {

    page_t & alloc(void);
    void     free (page_t & page);

    page_t & read (tableid_t table_id, pageid_t page_id);
    void     write(page_t & page);

    pageid_t get_pageid(tableid_t table_id, off_t offset);
    off_t    get_offset(tableid_t table_id, pageid_t page_id);

}