#pragma once

#include <cmath>
#include <climits>

#include <iostream>
#include <vector>
#include <memory>

#define ASSERT_MSG(cond, msg) \
    if (!(cond)) { \
        std::cerr << "Assertion failed: " << msg << std::endl; \
        assert(cond); \
    }


inline bool coinflip(){
    return (rand() & 10) >= 5; // 50 50 chance
}

/* a single skip list operation */ 
struct Operation {
  char op; // insert, remove, contains
  int value;
};

/* a single skip list node */ 
struct Node {
  std::shared_ptr<Node> next;
  std::shared_ptr<Node> down;
  int value;
  Node(std::shared_ptr<Node> n, std::shared_ptr<Node> d, int v) : next(n), down(d), value(v) {}
};

/* Skip List Interface */
class SkipList
{
public:
    SkipList() = default;
    virtual ~SkipList() = default;

    /* whether the skip list contains a node with the target value*/
    virtual bool contains(int val) = 0;
    /* Add a node with the target value into the skip list, duplicates not allowed */
    virtual bool insert(int val) = 0;
    /* Remove the node with the target value from the skip list */
    virtual bool remove(int val) = 0;

    /* 
      Validates the skip list to ensure it maintains all invariants (each layer is sorted and each column is equivalent)
      This is not meant to be run in a multi-threaded context
    */
    virtual void validate() = 0;

    /* Returns the max number of levels in skip list */
    int get_max_levels() { return max_levels_; }
protected:
    int max_levels_;
};

class Checker{
  public:

  Checker(char mode, int num_ops, bool v);

  std::shared_ptr<SkipList> skiplist;
  bool verbose;

  void RunOperations(std::vector<Operation>& ops);
  void PrintOutcome();
  std::string SkipListToString();
};

const char *get_option_string(const char *option_name,
                              const char *default_value);
int get_option_int(const char *option_name, int default_value);
float get_option_float(const char *option_name, float default_value);
