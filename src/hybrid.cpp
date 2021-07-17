#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ios>
#include <thread>
#include <mutex>
#include <knn_utility.hpp>
#include <utimer.hpp>

// each par_sort_insert has its own min_k -> computes it -> results are stored in hashmap(<point, tid>, vector) -> merge sort
#define SCALING_FACTOR 2
// Mutex used to syncronize the output file
std::mutex m_ostream;
std::mutex m_mink;

typedef std::pair<point, std::thread::id> result_point;

std::unordered_map<result_point, std::vector<pdistance>> results;
std::unordered_map<point, std::vector<std::thread::id>> tids_of_point;


void merge_sort(std::vector<pdistance>* dest, std::vector<pdistance>* x) {
    auto data_dest = dest->data();
    auto data_x = x->data();

    int counter = 0;
    int i_x = 0; 
    int i_dest = 0;

    while(counter != dest->size()) {
        if(std::get<0>(data_dest[i_dest]) > std::get<0>(data_x[i_x])) {
            data_dest[i_dest] = data_x[i_x];
            i_dest++;
            i_x++;
        }
        else
            i_dest++;
    }
}


void merge_sort_all(std::vector<pdistance>* dest, std::vector<point>* space, const int point, const int k) {
    std::vector<std::thread::id> tids = results.at(point);
    std::vector<result_point> results_of_point;
    std::vector<std::vector<pdistance>> to_be_merged;

    for(auto e: tids)
        results_of_point.push_back(std::make_pair((*space)[i], e));
    
    for(auto e: results_of_point) 
        to_be_merged.push_back(results.at(e));

    for(size_t i = 0; i < k; ++i)
        dest->push_back(1.0e10f);
    
    for(auto e: to_be_merged)
        merge_sort(dest, &e);
}


void par_sort_insert(std::vector<point>* space, int outer_index, int start, int finish, int k) {
    std::vector<pdistance> min_k;
    for(size_t i = start; i < finish; ++i) {
        if((std::get<0>((*space)[i]) == std::get<0>((*space)[outer_index])) && (std::get<1>((*space)[i]) == std::get<1>((*space)[outer_index]))) continue;
        knn_utility::sort_insert(&min_k, &(std::make_pair(knn_utility::euclidean_distance(&((*space)[outer_index]), &((*space)[i])), (*space)[i])), &k);
    }
    results.insert({std::make_pair((*space)[outer_index], std::this_thread::get_id()), min_k});
}


// Body of the threads
void compute_min_k(std::vector<point>* space, int start, int finish, int k, int nw, std::ofstream* output) {
    std::vector<pdistance> min_k;
    std::vector<std::thread*> insert_tids;
    std::vector<std::thread::id> thread_ids;
    int insert_nw = std::floor((nw - std::floor(nw/SCALING_FACTOR))/std::floor(nw/SCALING_FACTOR));
    int rate = std::ceil(space->size()/insert_nw);

    size_t i = 0;
    int index = 0;
    for(size_t i = start; i < finish; ++i) {
        for(size_t j = 1; j <= insert_nw; ++j)
            insert_tids.push_back(new std::thread(par_sort_insert, space, &min_k, i, (j-1)*rate, j*rate, k));
        for(index; index < insert_tids.size(); ++index)
            thread_ids.push_back(insert_tids.at(index)->get_id())
            insert_tids.at(index)->join();
        
        tids_of_point.insert({(*space)[i], thread_ids});
        thread_ids.clear();

        merge_sort_all(&min_k, (*space)[i], k);
        m_ostream.lock();
        (*output) << knn_utility::min_k_to_str(&((*space)[i]), &min_k);
        m_ostream.unlock();
        min_k.clear();
    }
}



int main(int argc, char* argv[]) {
    if(argc != 3 && argc != 4) {
        std::cout << "Usage: ./parallel.o <number of points> <number of workers> [input_file]" << std::endl;
        return -1;
    }

    // grab input parameters
    int k = std::stol(argv[1]);
    int nw = std::stol(argv[2]);

    std::string input_path = "";

    if(argc == 4)
        input_path = argv[3];
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
    output.open("../data/output_hybrid.txt", std::ios::out);

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
                tids.push_back(new std::thread(compute_min_k, &space, i, size, k, nw, &output));
            } else
                tids.push_back(new std::thread(compute_min_k, &space, i, i + rate, k, nw, &output));
        }
        // joining threads
        for(auto e : tids) e->join();
        output.close();
    }
    
    return 0;
}