#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <map>
#include <functional>
#include <string>
#include <vector>
#include <list>
class HashTable
{
    size_t last_code;
    size_t table_size;
    std::vector<std::list<std::pair<std::string,size_t>>> table;//hash:code
    std::function<size_t(std::string)> hash;
public:
    size_t add(std::string str, bool override_code=false, size_t code=0){
        if(override_code)
            last_code=code;
        size_t index=hash(str)%table_size;
        table[index].push_back(std::pair<std::string,size_t>(str,++last_code));
        return last_code;
    }
    size_t get_code(std::string str){
        size_t index=hash(str)%table_size;
        for(auto it: table[index]){
            if(it.first==str){
                return it.second;
            }
        }
        return 0;
    }
    std::string get_by_code(size_t code){
        for(size_t i=0;i<table_size;i++){
            for(auto it: table[i]){
                if(it.second==code){
                    return it.first;
                }
            }
        }
        return "";
    }
    HashTable(size_t _table_size, std::function<size_t(std::string)> _hash=std::hash<std::string>()){
        table_size=_table_size;
        hash=_hash;
        table.resize(table_size);
        last_code=0;
        size_t index=hash("\0")%table_size;
        table[index].push_back(std::pair<std::string,size_t>("\0",0));
    }
};

#endif // HASHTABLE_H
