#include "sequential/skip-list-sequential.h"
#include "lock-free/skip-list-lock-free.h"
#include "coarse/skip-list-coarse.h"
#include "fine/skip-list-fine.h"



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
        b = skiplist->insert(op.value);
        if (verbose) std::cout << "Insert Result: " << b << "\n";
        break;

        case 'r':
        if (verbose) std::cout << "Operation: Remove " << op.value << "\n";
        b = skiplist->remove(op.value);
        if (verbose) std::cout << "Remove Result: " << b << "\n";
        break;

        case 'c':
        if (verbose) std::cout << "Operation: Search " << op.value << "\n";
        b = skiplist->contains(op.value);
        if (verbose) std::cout << "Search Result: " << b << "\n";
        break;

        default:
        break;

        }
        std::cout << "\n";
        // Validate the list

        // if (verbose){
        //     PrintOutcome();
        //     std::cout << "\n";
        // }

        
    }
    skiplist->validate();
}

void Checker::PrintOutcome(){
    //std::cout << "Simulation Outcome: \n" << SkipListToString();
}

// std::string Checker::SkipListToString(){
//     std::string s = "";
//     LockFreeNode* curr_node;
//     LockFreeNode* start_node = std::dynamic_pointer_cast<LockFreeSkipList>(skiplist)->head;
//     int level = std::dynamic_pointer_cast<LockFreeSkipList>(skiplist)->get_max_levels();
//     s += "TOTAL LEVELS: " + std::to_string(level) + "\n";
//     int l = 1;
//     while(start_node && l <= level){
//         s += "LEVEL " +  std::to_string(l) + ": ";
//         curr_node = start_node;
//         while(curr_node){
//             s += std::to_string(curr_node->key) + " ";
//             curr_node = curr_node->get_right();
//         }
//         s += "\n";
//         start_node = start_node->up;
//         l++;
//     }
    
//     return s;
// }