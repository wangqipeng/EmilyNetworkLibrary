#ifndef CACHE_H_
#define CACHE_h_
#include <list>
#include <boost/noncopyable.hpp>
#include <stdio.h>
#include <unordered_map>
#include <iostream>
#include "Mutex.h"
using namespace std;
namespace ACache{
template <typename Key, typename Value>
class Cache : boost::noncopyable
{
public:
    Cache(const std::unordered_map<Key,Value>& hm,
                       const std::list<Key>& list):hashmap_(hm),
		                                           lruListM(list),
									               MAX_SIZE(3000),//FIXME
									               mutex_(){}

    int Set(const Key& k, const Value& v);
   
    Value Get(const Key& k);

    int Erase(const Key& k);
   
    int Add(const Key& k,const Value& v);
  
    int Replace(const Key& k, const Value& v);

private:
    int  RemoveLRU();
    void RefreshList(const Key& k);

private:
    std::unordered_map<Key,Value> hash_map_;
    std::list<Key> lru_list_;   
    const int kMaxSize;
    Mutex mutex_;
};

template <typename Key,typename  Value>
int Cache<Key,Value>::set(const Key& k, const Value&v)
{
    MutexGuard guard(mutex_);   
    //if the K exist in the cache
    typename std::unordered_map<Key,Value>::iterator iter = hashmap_.find(k);
    if(iter != hashmap_.end())
    {
        hash_map_[k] = v;
        //update the state of the LRU list
        RefreshList(k);
        return 0;            
    }
    
    int size = hash_map_.size();   
    if(size == kMaxSize)
    {   //remove the LRU element
        int ret = RemoveLRU();
        if(ret == -1)
        {
            FILE_LOG(logERROR)<<"ERR at removeLRU() node";   
            return -1;
        }               
    }
   
    hash_map_[k] = v; 
    lru_list_.push_back(k); 
    return 0;     
}

template <typename Key,typename  Value>
int Cache<Key,Value>::RemoveLRU()
{   
    Key k= lruList_.front();
    lru_list_.pop_front();
    hash_map_.erase(k);    
}

template <typename Key,typename  Value>
Value Cache<Key,Value>:: get(const Key& k)
{
    MutexGuard guard(mutex_);   
    typename std::unordered_map<Key,Value>::iterator iter = hash_map_.find(k);
    if(iter == hash_map_.end())
    {
        Value tmp;
        hash_map_[k] = tmp;
        lru_list_.push_back(k);
        return tmp;//return the default value
    }
  
    RefreshList(k);
    return iter->second;   
}

template <typename Key,typename Value>
void Cache<Key,Value>::refreshList(const Key& k)
{
    lru_list_.remove(k);
    lru_list_.push_back(k);    
}

template <typename Key,typename  Value>
int Cache<Key,Value>::erase(const Key& k)
{
    MutexGuard guard(mutex_);   
    hash_map_.erase(k);
    lru_list_.remove(k);       
} 

template <typename Key,typename  Value>
int Cache<Key,Value>::add(const Key& k, const Value& v)
{
    MutexGuard guard(mutex_);   
    typename std::unordered_map<Key,Value>::iterator iter = hash_map_.find(k);
    if(iter != hash_map_.end())
    {
        fprintf(stderr,"ERR in add the Key-Value to the Cache, the Key has existed\n");
        return -1; 
    }

    hash_map_[k] = v;
    lru_list_.push_back(k);
    return 0;
}

template <typename Key,typename  Value>
int Cache<Key,Value>::replace(const Key& k, const Value& v)
{
    MutexGuard guard(mutex_);   
    typename std::unordered_map<Key,Value>::iterator iter = hash_map_.find(k);
    if(iter == hash_map_.end())
    {
        fprintf(stderr,"ERR in replace the Key-Value in the Cache,the Key doesn't exist\n");
        return -1;   
    }   

    hash_map_[k] = v;
    RefreshList(k);
    return 0;
}
}//ACache
#endif
