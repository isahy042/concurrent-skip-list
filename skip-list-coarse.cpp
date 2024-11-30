#include "skip-list.h"

CoarseSkipList::CoarseSkipList(int total_elements, int min_val){
    skiplist = new LinearSkipList(total_elements, min_val);
    pthread_mutex_init(mtx, nullptr);
}

CoarseSkipList::~CoarseSkipList(){
}


bool CoarseSkipList::contains(int val){
    bool contains = false;
    pthread_mutex_lock(mtx); // Lock the mutex
    contains = skiplist->contains(val);
    pthread_mutex_unlock(mtx); // Unlock the mutex
    return contains;
}

void CoarseSkipList::insert(int val){
    pthread_mutex_lock(mtx); 
    skiplist->insert(val);
    pthread_mutex_unlock(mtx); 
}
void CoarseSkipList::remove(int val){
    pthread_mutex_lock(mtx); 
    skiplist->remove(val);
    pthread_mutex_unlock(mtx); 
}