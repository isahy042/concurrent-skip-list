#pragma once
#include "skip-list.h"

struct FineNode {
    std::vector<std::unique_ptr<FineNode> > next_;
    int top_layer_;
    int key_;
    bool marked_;
    bool fully_linked_;
    std::mutex lock_;

    FineNode() : top_layer_(0), key_(INT_MIN), marked_(false), fully_linked_(false) {}
    FineNode(int top_layer, int key) : top_layer_(top_layer), key_(key), marked_(false), fully_linked_(false) {}
};

class FineSkipList : public SkipList
{
public:
    FineSkipList(int total_elements);
    FineSkipList() = delete;
    ~FineSkipList() = default;

    bool contains(int key);
    void insert(int key);
    void remove(int key);
private:
    int total_elements_;
    int max_layers_;
    std::unique_ptr<FineNode> l_sentinel_;
    std::unique_ptr<FineNode> r_sentinel_;
};