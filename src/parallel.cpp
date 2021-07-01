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
#include <thread>
#include <mutex>
#include "knn_utility.hpp"

std::mutex m_ostream;

void compute_min_k(std::vector<point>* space, int start, int finish, int k, std::ofstream* output) {
    std::vector<pdistance> min_k;
    for(size_t i = start; i < finish; ++i) {
        for(auto y : (*space)) {
            // skip distance between x and x itself
            if((*space)[i] == y) continue;
            // sort insert the distance between x and y
            knn_utility::sort_insert(&min_k, std::make_pair(knn_utility::euclidean_distance((*space)[i], y), y), k);
        }
        m_ostream.lock();
        (*output) << knn_utility::min_k_to_str((*space)[i], min_k);
        m_ostream.unlock();
        min_k.clear();
    }
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        std::cout << "Usage: ./parallel.cpp k nw." << std::endl;
        return -1;
    }

    int k = atoi(argv[1]);
    int nw = atoi(argv[2]);

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
    output.open("output_par.txt", std::ios::trunc);

    std::vector<std::thread*> tids;
    int rate = space.size()/nw;

    for(size_t i = 0; i < space.size(); i += rate) {
        std::cout << "Creating thread" << std::endl;
        tids.push_back(new std::thread(compute_min_k, &space, i, i + rate, k, &output));
    }

    for(auto e : tids) {
        std::cout << "Joining thread" << std::endl;
        e->join();
    }
    output.close();
    return 0;
}