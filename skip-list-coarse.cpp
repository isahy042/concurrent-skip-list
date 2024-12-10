#include "skip-list-coarse.h"
#include "skip-list-sequential.h"

CoarseSkipList::CoarseSkipList(int total_elements){
    skiplist = std::make_unique<SequentialSkipList>(total_elements);
}

bool CoarseSkipList::contains(int val){
    std::lock_guard<std::mutex> guard(mtx);
    bool contains = false;
    contains = skiplist->contains(val);
    return contains;
}

bool CoarseSkipList::insert(int val){
    std::lock_guard<std::mutex> guard(mtx);
    return skiplist->insert(val);
}
bool CoarseSkipList::remove(int val){
    std::lock_guard<std::mutex> guard(mtx);
    return skiplist->remove(val);
}