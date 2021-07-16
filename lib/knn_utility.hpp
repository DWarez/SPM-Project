#pragma once

#ifndef KNN_UTILITY
#define KNN_UTILITY

#include <iomanip>
#include <sstream>
#include <utility>
#include <cstring>

// Definition of a point as a pair of doubles (coordinates)
typedef std::pair<double, double> point;
// Definition of pdistance as a pair of double (distance of x from y) and a point (point x)
typedef std::pair<double, point> pdistance;


namespace knn_utility {

    // Function to pretty print a point
    std::string str_point(point p) {
        std::stringstream stream1;
        stream1 << std::fixed << std::setprecision(2) << std::get<0>(p);
        std::stringstream stream2;
        stream2 << std::fixed << std::setprecision(2) << std::get<1>(p);
        return "(" + stream1.str() + ", " + stream2.str() + ")";
    }


    std::string min_k_to_str(point p, std::vector<pdistance>* x) {
        std::string r = "\n";
        r += knn_utility::str_point(p) + ": ";
        for(auto e : (*x)) {
            r += "<" + knn_utility::str_point(std::get<1>(e)) + "," + std::to_string(std::get<0>(e)) + "> ";
        }
        return r;
    }

    // Function to create points from strings, in particular from the strings generated from generate_data.py
    point make_pair_from_string(std::string s) {
        std::string first = "";
        std::string second = "";
        bool b = true;
        // finding the space, that delimits the second value
        for(size_t i = 0; i < s.length(); ++i) {
            if(s[i] == ',') {
                b = false;
                continue;
            }
            if(b)
                first += (s[i]);
            else
                second += (s[i]);
        }
        
        // making the pair
        return std::make_pair(std::stof(first), std::stof(second));
    }


    // function to compute the euclian distance in 2D: r = sqrt((x_0 - y_0)^2 + (x_1 - y_1)^2)
    double euclidean_distance(point x, point y) {
        return sqrt(pow((std::get<0>(x) - std::get<0>(y)), 2) + pow((std::get<1>(x) - std::get<1>(y)), 2));
    }


    // function to insert pdistances in a sorted way
    void sort_insert(std::vector<pdistance>* x, pdistance value, int k) {

        // If empy simply add the value
        if(x->empty()) {
            x->push_back(value);
            return;
        }

        // It's useless to linearly scan the vector of minima if the value
        // is greater than the maximum among the minima already found
        if(x->size() == k && std::get<0>(value) > std::get<0>(x->back())) {
            return;
        }

        // get raw data from the vector structure and the iterator
        auto data = x->data();
        auto it = x->begin();

        // scan to find the correct position of the value
        std::size_t i = 0;
        for(i = 0; i < x->size(); ++i) {
            if(std::get<0>(value) < std::get<0>(data[i])) {
                break;
            }
        }

        // insert the value, shifting all the elements to its right to the right
        x->insert(it+i, value);

        // if the vector size exceedes the number of minima we want to find, pop the
        // last element which is also the larger minimum found up to now
        if(x->size() > k) {
            x->pop_back();
        }
        return;
    }
}

#endif