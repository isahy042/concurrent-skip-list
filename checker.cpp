#include "skip-list-sequential.h"


Checker::Checker(char mode, int num_elements, int min_val, bool v){
    verbose = v;
    switch(mode) {
        case '1':
            skiplist = new SequentialSkipList(num_elements, min_val);
            if (verbose) std::cout << "initialized skip list with max level " << dynamic_cast<SequentialSkipList*>(skiplist)->max_levels << '\n';
            break;
        default:
            std::cout << "checker received mode unimplemented \n";
            skiplist = new SequentialSkipList(num_elements, min_val);
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
    Node* curr_node;
    Node* start_node = dynamic_cast<SequentialSkipList*>(skiplist)->head;
    int level = dynamic_cast<SequentialSkipList*>(skiplist)->max_levels;
    while(start_node){
        s += "LEVEL " +  std::to_string(level) + ": ";
        curr_node = start_node;
        while(curr_node){
            s += std::to_string(curr_node->value) + " ";
            curr_node = curr_node->next;
        }
        s += "\n";
        start_node = start_node->down;
        level --;
    }
    
    return s;
}