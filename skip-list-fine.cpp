#include "skip-list-fine.h"
#include <memory>
#include <mutex>

FineSkipList::FineSkipList(int total_elements) {
    int total_elements_ = total_elements;
    int max_layers_ = std::max(1, static_cast<int>(std::log2(total_elements)));

    l_sentinel_ = std::make_shared<FineNode>(max_layers_, INT_MIN);
    l_sentinel_->fully_linked_ = true;
    auto r_sentinal = std::make_shared<FineNode>(max_layers_, INT_MAX);
    r_sentinal->fully_linked_ = true;
    
    for (int i = 0; i < max_layers_; i++) {
        l_sentinel_->next_.push_back(r_sentinal);
    }
}

int FineSkipList::find_node(int key, std::vector<std::shared_ptr<FineNode>>& preds, std::vector<std::shared_ptr<FineNode>>& succs) {
    int l_found = -1;
    auto pred = l_sentinel_;

    for (int layer = max_layers_ - 1; layer >= 0; layer--) {
        auto curr = pred->next_[layer];
        while (key > curr->key_) {
            pred = curr;
            curr = pred->next_[layer];
        }

        if (l_found == -1 && key == curr->key_) {
            l_found = layer;
        }

        preds[layer] = pred;
        succs[layer] = curr;
    }

    return l_found;
}

bool FineSkipList::contains(int key) {
    std::vector<std::shared_ptr<FineNode>> preds(max_layers_);
    std::vector<std::shared_ptr<FineNode>> succs(max_layers_);
    
    int l_found = find_node(key, preds, succs);

    return l_found != -1 && succs[l_found]->fully_linked_ && !succs[l_found]->marked_;
}

bool FineSkipList::insert(int key) {
    int top_layer = random_level();
    
    std::vector<std::shared_ptr<FineNode>> preds(max_layers_);
    std::vector<std::shared_ptr<FineNode>> succs(max_layers_);

    std::vector<std::unique_lock<std::mutex>> locks;

    while (true) {
        int l_found = find_node(key, preds, succs);
        if (l_found != -1) {
            auto node_found = succs[l_found];
            if (!node_found->marked_) {
                while (!node_found->fully_linked_) {}
                return false;
            }
            continue;
        }

        int highest_locked = -1;
        
        std::shared_ptr<FineNode> pred, succ, prev_pred = nullptr;
        bool valid = true;

        for(int layer = 0; valid && layer <= top_layer; layer++) {
            pred = preds[layer];
            succ = succs[layer];

            if (pred != prev_pred) {
                locks.push_back(std::unique_lock<std::mutex>(pred->lock_));
                highest_locked = layer;
                prev_pred = pred;
            }
            valid = !pred->marked_ && !succ->marked_ && pred->next_[layer] == succ;
        }

        if (!valid) continue;

        auto new_node = std::make_shared<FineNode>(top_layer, key);
        for (int layer = 0; layer <= top_layer; layer++) {
            new_node->next_[layer] = succs[layer];
            preds[layer]->next_[layer] = new_node;
        }

        new_node->fully_linked_ = true;
        return true;
    }
}

bool FineSkipList::remove(int key) {
    std::shared_ptr<FineNode> node_to_delete = nullptr;
    bool is_marked = false;
    int top_layer = -1;

    std::vector<std::shared_ptr<FineNode>> preds(max_layers_);
    std::vector<std::shared_ptr<FineNode>> succs(max_layers_);

    std::vector<std::unique_lock<std::mutex>> locks;

    while (true) {
        int l_found = find_node(key, preds, succs);
        if (is_marked || (l_found != -1 && ok_to_delete(succs[l_found], l_found))) {
            if (!is_marked) {
                node_to_delete = succs[l_found];
                top_layer = node_to_delete->top_layer_;
                locks.push_back(std::unique_lock<std::mutex>(node_to_delete->lock_));

                if (node_to_delete->marked_) {
                    return false;
                }

                node_to_delete->marked_ = true;
                is_marked = true;
            }

            int highest_locked = -1;

            std::shared_ptr<FineNode> pred, succ, prev_pred = nullptr;
            bool valid = true;
            for (int layer = 0; layer <= top_layer; layer++) {
                pred = preds[layer];
                succ = succs[layer];

                if (pred != prev_pred) {
                    locks.push_back(std::unique_lock<std::mutex>(pred->lock_));
                    highest_locked = layer;
                    prev_pred = pred;
                }
                valid = !pred->marked_ && pred->next_[layer] == succ;
            }

            if (!valid) continue;

            for (int layer = top_layer; layer >= 0; layer--) {
                preds[layer]->next_[layer] = node_to_delete->next_[layer];
            }
            return true;
        } else {
            return false;
        }
    }
}