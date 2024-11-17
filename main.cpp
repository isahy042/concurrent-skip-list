#include "skip-list.h"

void print_stats() {
  // int max_occupancy = 0;
  // long long total_cost = 0;

  // for (const auto& row : occupancy) {
  //   for (const int count : row) {
  //     max_occupancy = std::max(max_occupancy, count);
  //     total_cost += count * count;
  //   }
  // }

  // std::cout << "Max occupancy: " << max_occupancy << '\n';
  // std::cout << "Total cost: " << total_cost << '\n';
}

void write_output() {
  // if (std::size(input_filename) >= 4 && input_filename.substr(std::size(input_filename) - 4) == ".txt") {
  //   input_filename.resize(std::size(input_filename) - 4);
  // }

  // const std::string occupancy_filename = input_filename + "_occupancy_" + std::to_string(num_threads) + ".txt";
  // const std::string wires_filename = input_filename + "_wires_" + std::to_string(num_threads) + ".txt";

  // std::ofstream out_occupancy(occupancy_filename, std::fstream::out);
  // if (!out_occupancy) {
  //   std::cerr << "Unable to open file: " << occupancy_filename << '\n';
  //   exit(EXIT_FAILURE);
  // }

  // out_occupancy << dim_x << ' ' << dim_y << '\n';
  // for (const auto& row : occupancy) {
  //   for (const int count : row) {
  //     out_occupancy << count << ' ';
  //   }
  //   out_occupancy << '\n';
  // }

  // out_occupancy.close();

  // std::ofstream out_wires(wires_filename, std::fstream:: out);
  // if (!out_wires) {
  //   std::cerr << "Unable to open file: " << wires_filename << '\n';
  //   exit(EXIT_FAILURE);
  // }

  // out_wires << dim_x << ' ' << dim_y << '\n' << num_wires << '\n';

  // for (const auto& [start_x, start_y, end_x, end_y, bend1_x, bend1_y] : wires) {
  //   out_wires << start_x << ' ' << start_y << ' ' << bend1_x << ' ' << bend1_y << ' ';

  //   if (start_y == bend1_y) {
  //   // first bend was horizontal

  //     if (end_x != bend1_x) {
  //       // two bends

  //       out_wires << bend1_x << ' ' << end_y << ' ';
  //     }
  //   } else if (start_x == bend1_x) {
  //     // first bend was vertical

  //     if(end_y != bend1_y) {
  //       // two bends

  //       out_wires << end_x << ' ' << bend1_y << ' ';
  //     }
  //   }
  //   out_wires << end_x << ' ' << end_y << '\n';
  // }

  // out_wires.close();
}

int main(int argc, char *argv[]) {
  const auto init_start = std::chrono::steady_clock::now();

  std::string input_filename;
  int num_threads = 1;
  char mode = '\0';
  int batch_size = 1;
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
      case 'b':
        batch_size = atoi(optarg);
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
  if (empty(input_filename) || num_threads <= 0 || (mode != '1' && mode != '2'
                                                    && mode != '3' && mode != '4') || batch_size <= 0) {
      std::cerr << "Usage: " << argv[0] << " -f input_filename -m mode [-n num_threads] [-b batch_size] [-v] \n";
      exit(EXIT_FAILURE);
  }

  std::cout << "Number of threads: " << num_threads << '\n';
  std::cout << "Input file: " << input_filename << '\n';
  std::cout << "Skip list mode: " << mode << '\n';
  std::cout << "Batch size: " << batch_size << '\n';

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

  const double init_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - init_start).count();
  std::cout << "Initialization time (sec): " << std::fixed << std::setprecision(10) << init_time << '\n';

  const auto compute_start = std::chrono::steady_clock::now();
  
  // run the operations with a checker
  Checker* checker = new Checker(mode, num_elements, min_val, verbose);
  checker->RunOperations(&operations);
  checker->PrintOutcome();

  const double compute_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - compute_start).count();
  std::cout << "Computation time (sec): " << compute_time << '\n';

  /* Write wires and occupancy matrix to files */

  //print_stats(occupancy);
  //write_output(wires, num_wires, occupancy, dim_x, dim_y, num_threads, input_filename);
}

