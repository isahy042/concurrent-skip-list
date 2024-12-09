#include "skip-list-sequential.h"
#include "skip-list-lock-free.h"



Checker::Checker(char mode, int num_elements, int min_val, bool v){
    verbose = v;
    switch(mode) {
        case '1':
            skiplist = new SequentialSkipList(num_elements);
            if (verbose) std::cout << "initialized skip list with max level " << dynamic_cast<SequentialSkipList*>(skiplist)->max_levels << '\n';
            break;
        case '4':
            skiplist = new LockFreeSkipList(num_elements);
            if (verbose) std::cout << "initialized skip list with max level " << dynamic_cast<LockFreeSkipList*>(skiplist)->max_levels << '\n';
            break;
        default:
            std::cout << "checker received mode unimplemented \n";
            skiplist = new SequentialSkipList(num_elements);
    }

    
}

void Checker::RunOperations(std::vector<Operation>* ops){
    bool b;
    for (const auto &op : *ops){
        switch(op.op) {

        case 'i':
        if (verbose) std::cout << "Operation: Insert " << op.value << "\n";
        skiplist->insert(op.value);
        break;

        case 'r':
        if (verbose) std::cout << "Operation: Remove " << op.value << "\n";
        skiplist->remove(op.value);
        break;

        case 'c':
        if (verbose) std::cout << "Operation: Search " << op.value << "\n";
        b = skiplist->contains(op.value);
        if (verbose) std::cout << "Search Result: " << b << "\n";
        break;

        default:
        break;

        }

        // if (verbose){
        //     PrintOutcome();
        //     std::cout << "\n";
        // }

        
    }
}

void Checker::PrintOutcome(){
    std::cout << "Simulation Outcome: \n" << SkipListToString();
}

std::string Checker::SkipListToString(){
    std::string s = "";
    std::shared_ptr<LockFreeNode> curr_node;
    std::shared_ptr<LockFreeNode> start_node = dynamic_cast<LockFreeSkipList*>(skiplist)->head;
    int level = dynamic_cast<LockFreeSkipList*>(skiplist)->max_levels;
    s += "TOTAL LEVELS: " + std::to_string(level) + "\n";
    int l = 1;
    while(start_node && l <= level){
        s += "LEVEL " +  std::to_string(l) + ": ";
        curr_node = start_node;
        while(curr_node){
            s += std::to_string(curr_node->key) + " ";
            curr_node = curr_node->succ.right;
        }
        s += "\n";
        start_node = start_node->up;
        l++;
    }
    
    return s;
}