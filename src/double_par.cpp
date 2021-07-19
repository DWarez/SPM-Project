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

// Mutex used to synchronize the output file
std::mutex m_ostream;

// Body of threads started in compute_min_k. Computes the local min_k of a given point
void par_sort_insert(std::vector<point>* space, std::vector<pdistance>* min_k, int outer_index, int start, int finish, int k, std::mutex* m_mink) {
    for(size_t i = start; i < finish; ++i) {
        if((std::get<0>((*space)[i]) == std::get<0>((*space)[outer_index])) && (std::get<1>((*space)[i]) == std::get<1>((*space)[outer_index]))) continue;
        double distance = knn_utility::euclidean_distance(&((*space)[outer_index]), &((*space)[i]));
        if(min_k->size() >= k && distance >= std::get<0>(min_k->back())) continue;
        (*m_mink).lock();
        knn_utility::sort_insert(min_k, &(std::make_pair(distance, (*space)[i])), &k);
        (*m_mink).unlock();
    }
}


// Body of threads started in main
void compute_min_k(std::vector<point>* space, int start, int finish, int k, int nw, const int SCALING_FACTOR, std::ofstream* output) {
    // Mutex used to synchronize the min_k structure
    std::mutex m_mink;
    // Min_k data structure shared among threads started from this
    std::vector<pdistance> min_k;
    // tids of threads started from here
    std::vector<std::thread*> insert_tids;
    // parameters needed for computation
    int insert_nw = std::floor((nw - std::floor(nw/SCALING_FACTOR))/std::floor(nw/SCALING_FACTOR));
    int rate = std::ceil(space->size()/insert_nw);

    size_t i = 0;
    int index = 0;
    // for each point in the range assigned to the thread
    for(size_t i = start; i < finish; ++i) {
        // start a thread executing par_sort_insert
        for(size_t j = 1; j <= insert_nw; ++j)
            insert_tids.push_back(new std::thread(par_sort_insert, space, &min_k, i, (j-1)*rate, j*rate, k, &m_mink));
        // join them
        for(index; index < insert_tids.size(); ++index)
            insert_tids.at(index)->join();
        
        // write on output file and clear the data structure
        m_ostream.lock();
        (*output) << knn_utility::min_k_to_str(&((*space)[i]), &min_k);
        m_ostream.unlock();
        min_k.clear();
    }
}



int main(int argc, char* argv[]) {
    if(argc != 4 && argc != 5) {
        std::cout << "Usage: ./parallel.o <number of points> <number of workers> <scaling factor> [input_file]" << std::endl;
        return -1;
    }

    // grab input parameters
    int k = std::stol(argv[1]);
    int nw = std::stol(argv[2]);
    int SCALING_FACTOR = std::stol(argv[3]);

    std::string input_path = "";

    if(argc == 5)
        input_path = argv[4];
    else
        input_path = "../data/inputs.txt";

    // initialize input stream from file
    std::ifstream inputs (input_path);
    if(inputs.fail()) {
        std::cout << "Failed to open the file, please check the input path." << std::endl;
        return -1;
    }
    
    // open output stream
    std::ofstream output;
    output.open("../data/output_double_par.txt", std::ios::out);

    // vector of the input space
    std::vector<point> space;

    // vector for storing thread ids
    std::vector<std::thread*> tids;

    {
        utimer tpar_in("Input time");
        // get lines and obtain points from parsing
        if(inputs.is_open()) {
            std::string tmp;
            while(std::getline(inputs, tmp)) {
                space.push_back(knn_utility::make_pair_from_string(tmp));
            }
        }
        inputs.close();
    }
    int outer_nw = std::floor(nw/SCALING_FACTOR);
    {
        // remember that if you change the output string the benchmark script breaks :|
        utimer tpar("Parallel time with " + std::to_string(nw) + " workers");

        int size = space.size();
        // establish how many points will be managed by the thread
        int rate = std::ceil(size/outer_nw);
        // starting threads
        size_t i = 0;
        for(i = 0; i < size; i += rate) {
            if(i + rate > size) {
                tids.push_back(new std::thread(compute_min_k, &space, i, size, k, nw, SCALING_FACTOR, &output));
            } else
                tids.push_back(new std::thread(compute_min_k, &space, i, i + rate, k, nw, SCALING_FACTOR, &output));
        }
        // joining threads
        for(auto e : tids) e->join();
        output.close();
    }
    
    return 0;
}