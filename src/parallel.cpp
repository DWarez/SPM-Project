#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>
#include <vector>
#include <ios>
#include <thread>
#include <mutex>
#include <knn_utility.hpp>
#include <utimer.hpp>

// Mutex used to syncronize the output file
std::mutex m_ostream;

// Body of the threads
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
        std::cout << "Usage: ./parallel.o <number of points> <number of workers>" << std::endl;
        return -1;
    }

    // grab input parameters
    int k = std::stol(argv[1]);
    int nw = std::stol(argv[2]);

    // vector of the input space
    std::vector<point> space;
    // initialize input stream from file
    std::fstream inputs;
    inputs.open("../data/inputs.txt", std::ios::in);

    // get lines and obtain points from parsing
    if(inputs.is_open()) {
        std::cout << "Reading input file" << std::endl;
        std::string tmp;
        while(std::getline(inputs, tmp)) {
            space.push_back(knn_utility::make_pair_from_string(tmp));
        }
    }
    inputs.close();
    
    // open output stream
    std::ofstream output;
    output.open("../data/output_par.txt", std::ios::out);

    // vector for storing thread ids
    std::vector<std::thread*> tids;
    // establish how many points will be managed by the thread
    int rate = std::ceil(space.size()/nw);
    int reminder = space.size()%nw;

    {
        // remember that if you change the output string the benchmark script breaks :|
        utimer tpar("Parallel time with " + std::to_string(nw) + " workers");
        // starting threads
        size_t i = 0;
        for(i = 0; i < space.size(); i += rate) {
            if(i + rate > space.size()) {
                tids.push_back(new std::thread(compute_min_k, &space, i, space.size(), k, &output));
            } else
                tids.push_back(new std::thread(compute_min_k, &space, i, i + rate, k, &output));
        }

        // joining threads
        for(auto e : tids) e->join();
    }
    

    output.close();
    return 0;
}