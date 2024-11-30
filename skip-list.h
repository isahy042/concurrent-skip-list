#include <cstdint>
#include <cstdio>
#include <cmath>
#include <cassert>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>

#include <unistd.h>
#include <pthread.h>

#include <omp.h>

static bool coinflip(){
    return (rand() & 10) >= 5; // 50 50 chance
}

/* a single skip list operation */ 
struct Operation {
  char op; // insert, remove, contains
  int value;
};

/* a single skip list node */ 
struct Node {
  Node* next;
  Node* down;
  int value = 0;
  Node(Node* n, Node* d, int v) : next(n), down(d), value(v) {}
};

/* Skip List Interface */
class SkipList
{
  public:
    SkipList(){}
    virtual ~SkipList(){}

    // Node* head; // start of skip list
    // int max_levels;

    /* whether the skip list contains a node with the target value*/
    virtual bool contains(int val) = 0;
    /* Add a node with the target value into the skip list, duplicates not allowed */
    virtual void insert(int val) = 0;
    /* Remove the node with the target value from the skip list */
    virtual void remove(int val) = 0;
};

/* Linear Skip List */
class LinearSkipList : public SkipList
{
  public:
    Node* head;
    int max_levels;

    LinearSkipList(int total_elements, int min_val);
    ~LinearSkipList();

    bool contains(int val);
    void insert(int val);
    void remove(int val);
};

/* Linear Skip List */
class CoarseSkipList : public SkipList
{
  public:
    LinearSkipList* skiplist;
    pthread_mutex_t* mtx;

    CoarseSkipList(int total_elements, int min_val);
    ~CoarseSkipList();

    bool contains(int val);
    void insert(int val);
    void remove(int val);
};

class Checker{
  public:

  Checker(char mode, int num_ops, int min_val, bool v);

  SkipList* skiplist;
  bool verbose;

  void RunOperations(std::vector<Operation>* ops);
  void PrintOutcome();
  std::string SkipListToString();
};


const char *get_option_string(const char *option_name,
                              const char *default_value);
int get_option_int(const char *option_name, int default_value);
float get_option_float(const char *option_name, float default_value);
