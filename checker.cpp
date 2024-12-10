#include "skip-list-sequential.h"
#include "skip-list-lock-free.h"
#include "skip-list-coarse.h"
#include "skip-list-fine.h"



Checker::Checker(char mode, int num_elements, bool v){
    verbose = v;
    switch(mode) {
        case '1':
            skiplist = std::make_shared<SequentialSkipList>(num_elements);
            if (verbose) std::cout << "initialized sequential skip list with max levels " << skiplist->get_max_levels() << '\n';
            break;
        case '2':
            skiplist = std::make_shared<CoarseSkipList>(num_elements);
            if (verbose) std::cout << "initialized coarse-grain skip list with max levels " << skiplist->get_max_levels() << '\n';
            break;
        case '3':
            skiplist = std::make_shared<FineSkipList>(num_elements);
            if (verbose) std::cout << "initialized fine-grain skip list with max levels " << skiplist->get_max_levels() << '\n';
            break;
        case '4':
           skiplist = std::make_shared<LockFreeSkipList>(num_elements);
            if (verbose) std::cout << "initialized lock free skip list with max levels " << skiplist->get_max_levels() << '\n';
            break;
        
        default:
            std::cout << "checker received mode unimplemented \n";
            skiplist = std::make_shared<SequentialSkipList>(num_elements);
    }

    
}

void Checker::RunOperations(std::vector<Operation>& ops){
    bool b;
    for (const auto &op : ops){
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
    std::shared_ptr<LockFreeNode> start_node = std::dynamic_pointer_cast<LockFreeSkipList>(skiplist)->head;
    int level = std::dynamic_pointer_cast<LockFreeSkipList>(skiplist)->get_max_levels();
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