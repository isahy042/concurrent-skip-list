#include "skip-list-coarse.h"
#include "skip-list-sequential.h"

CoarseSkipList::CoarseSkipList(int total_elements, int min_val){
    skiplist = std::make_unique<SequentialSkipList>(total_elements, min_val);
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