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

void CoarseSkipList::insert(int val){
    std::lock_guard<std::mutex> guard(mtx);
    skiplist->insert(val);
}
void CoarseSkipList::remove(int val){
    std::lock_guard<std::mutex> guard(mtx);
    skiplist->remove(val);
}