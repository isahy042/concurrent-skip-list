#include "skip-list.h"
#include <iomanip>
#include <unistd.h>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  const auto init_start = std::chrono::steady_clock::now();

  std::string input_filename;
  int num_threads = 1;
  char mode = '\0';
  bool verbose = false;

  int opt;
  while ((opt = getopt(argc, argv, "f:n:p:m:v::")) != -1) {
    switch (opt) {
      case 'f':
        input_filename = optarg;
        break;
      case 'n':
        num_threads = atoi(optarg);
        break;
      case 'm':
        mode = *optarg;
        break;
      case 'v':
        verbose = true;
        break;
      default:
        std::cerr << "Usage: " << argv[0] << " -f input_filename -m mode [-n num_threads] [-b batch_size] [-v]\n";
        exit(EXIT_FAILURE);
    }
  }

  // Check if required options are provided
  if (input_filename.empty() || num_threads <= 0 || (mode != '1' && mode != '2'
                                                    && mode != '3' && mode != '4')) {
      std::cerr << "Usage: " << argv[0] << " -f input_filename -m mode [-n num_threads] [-v] \n";
      exit(EXIT_FAILURE);
  }

  std::cout << "Number of threads: " << num_threads << '\n';
  std::cout << "Input file: " << input_filename << '\n';
  std::cout << "Skip list mode: " << mode << '\n';

  std::ifstream fin(input_filename);

  if (!fin) {
    std::cerr << "Unable to open file: " << input_filename << ".\n";
    exit(EXIT_FAILURE);
  }

  int num_ops, num_elements, min_val;
  /* Read the number of operations from file */
  fin >> num_ops >> num_elements >> min_val;

  std::vector<Operation> operations(num_ops);

  for (auto& op : operations) {
    fin >> op.op >> op.value;
  }

  const double init_time = std::chrono::duration_cast<std::chrono::duration<double> >(std::chrono::steady_clock::now() - init_start).count();
  std::cout << "Initialization time (sec): " << std::fixed << std::setprecision(10) << init_time << '\n';

  const auto compute_start = std::chrono::steady_clock::now();
  
  // run the operations with a checker
  Checker* checker = new Checker(mode, num_elements, min_val, verbose);
  checker->RunOperations(&operations);
  checker->PrintOutcome();

  const double compute_time = std::chrono::duration_cast<std::chrono::duration<double> >(std::chrono::steady_clock::now() - compute_start).count();
  std::cout << "Computation time (sec): " << compute_time << '\n';
}

