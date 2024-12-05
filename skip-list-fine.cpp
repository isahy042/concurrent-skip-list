#include "skip-list-fine.h"
#include <memory>

FineSkipList::FineSkipList(int total_elements) {
    int total_elements_ = total_elements;
    int max_layers_ = std::max(1, static_cast<int>(std::log2(total_elements)));

    l_sentinel_ = std::make_unique<FineNode>(max_layers_, INT_MIN);
    l_sentinel_->fully_linked_ = true;
    auto r_sentinal = std::make_unique<FineNode>(max_layers_, INT_MAX);
    r_sentinal->fully_linked_ = true;
    
    for (int i = 0; i < max_layers_; i++) {
        l_sentinel_->next_.push_back(std::move(r_sentinal));
    }
}