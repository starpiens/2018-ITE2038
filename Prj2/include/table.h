#pragma once

class Table {
public:
    enum T_BASE_TYPE { BPT };
    Table(const char * filename, T_BASE_TYPE base_type = BPT) {}
    ~Table() {}

    virtual int insert(void * key, const void * value) = 0;
    
    
private:

};