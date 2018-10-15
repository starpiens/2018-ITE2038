#pragma once



class DB {
public:
    DB();
    int open_db(const char * path);
    virtual int _insert(int64_t key, const char * value) = 0;
    virtual char * _find(int64_t key) = 0;
    virtual int _delete(int64_t key) = 0;
    
protected:
    int fd;
};