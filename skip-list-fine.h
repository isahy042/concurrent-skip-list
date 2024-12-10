#pragma once
#include "skip-list.h"
#include <memory>
#include <random>
#include <mutex>

struct FineNode {
    std::vector<std::shared_ptr<FineNode> > next_;
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
    bool insert(int key);
    bool remove(int key);
private:
    std::shared_ptr<FineNode> l_sentinel_;

    int find_node(int key, std::vector<std::shared_ptr<FineNode>>& preds, std::vector<std::shared_ptr<FineNode>>& succs);

    int random_level() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);

        int level = 0;
        while (level <= max_levels_ - 1) {
            if (dis(gen) == 0) {
                break;
            }
            level++;
        }

        return level;
    }

    bool ok_to_delete(std::shared_ptr<FineNode> candidate, int l_found) {
        return candidate->fully_linked_ && candidate->top_layer_ == l_found && !candidate->marked_;
    }
};