#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>
#include <vector>
#include <ios>
#include <knn_utility.hpp>
#include <utimer.hpp>


int main(int argc, char* argv[]) {

    if(argc != 2 && argc != 3) {
        std::cout << "Usage: ./sequential.o <number of points> [input_file]" << std::endl;
        return -1;
    }

    int k = std::stol(argv[1]);

    std::string input_path = "";

    if(argc == 3)
        input_path = argv[2];
    else
        input_path = "../data/inputs.txt";

    // initialize input stream from file
    std::ifstream inputs(input_path);
    if(inputs.fail()) {
        std::cout << "Failed to open the file, please check the input path." << std::endl;
        return -1;
    }

    // allocate memory for input space
    std::vector<point> space;

    {
        utimer tseq_in("Input time");
        // get lines and obtain points from parsing
        if(inputs.is_open()) {
            std::string tmp;
            while(std::getline(inputs, tmp)) {
                space.push_back(knn_utility::make_pair_from_string(tmp));
            }
        }
        inputs.close();
    }

    {
        // remember that if you change the output string the benchmark script breaks :|
        utimer tseq("Sequential time");

        // open output file
        std::ofstream output;
        output.open("../data/output_seq.txt", std::ios::out);

        // for each point in the space
        for(auto x : space) {
        // vector to store the minima of point x
            std::vector<pdistance> min_k;
            // for each point in the space
            for(auto y : space) {
                // skip distance between x and x itself
                if(x == y) continue;
                // sort insert the distance between x and y
                knn_utility::sort_insert(&min_k, std::make_pair(knn_utility::euclidean_distance(x, y), y), k);
            }
            // print result on file
            output << knn_utility::min_k_to_str(x, &min_k);
            // clear the structure
            min_k.clear();
        }
        // close output stream
        output.close();
    }
    
    return 0;
}

