/**
 * Parallel VLSI Wire Routing via OpenMP
 * Name 1(andrew_id 1), Name 2(andrew_id 2)
 */

#ifndef __WIREOPT_H__
#define __WIREOPT_H__

#include <omp.h>
#include <cstdint>
#include <vector>

#define MAX_PTS_PER_WIRE 4
#define COST_REPORT_DEPTH 10

/* a single skip list operation */ 
struct Operation {
  char op; // insert, delete, contains
  int value;
};

/* a single skip list node */ 
struct Node {
  std::vector<Node *> ptrs;
  int value;
};

const char *get_option_string(const char *option_name,
                              const char *default_value);
int get_option_int(const char *option_name, int default_value);
float get_option_float(const char *option_name, float default_value);

#endif
