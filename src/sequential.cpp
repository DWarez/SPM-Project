#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <utility>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <cstring>
#include <ios>
#include "knn_utility.hpp"


int main(int argc, char* argv[]) {

    if(argc != 2) {
        std::cout << "Usage: ./parallel.cpp k." << std::endl;
        return -1;
    }

    int k = atoi(argv[1]);
    // allocate memory for input space
    std::vector<point> space;
    // initialize input stream from file
    std::fstream inputs;
    inputs.open("inputs.txt", std::ios::in);

    // get lines and obtain points from parsing
    if(inputs.is_open()) {
        std::string tmp;
        while(std::getline(inputs, tmp)) {
            space.push_back(knn_utility::make_pair_from_string(tmp));
        }
    }
    inputs.close();
    
    std::ofstream output;
    output.open("output_seq.txt", std::ios::out);

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
        output << knn_utility::min_k_to_str(x, min_k);
        // clear the structure
        min_k.clear();
    }

    output.close();
    return 0;
}

