#pragma once
#include <functional>
#include <stdexcept>
#include <map>
#include <vector>
#include <oneapi/tbb.h>
#include "apear/misc/rand_num.hpp"

namespace apear{

/**
 * @brief Base class for Quality-Diversity archives
 * Implement a basic archive storing the solution in a vector with the most recent solution pushed back.
 */
template<typename T>
class Archive{
public:
    using comparator_t = std::function<bool(const T&, const T&)> ;

    Archive(){}
    Archive(comparator_t comparator) :
        _comparator(comparator){}

    virtual void reset(){
        _solutions.clear();
    }

    virtual bool add_solution(const T& solution){
        _solutions.push_back(solution);
    }
    const T& random_solution(const misc::RandNum::Ptr &rand_num) const{
        if (_solutions.empty()) {
            throw std::runtime_error("No solutions in archive");
        }
        int index = rand_num->rand_int(0, static_cast<int>(_solutions.size()) - 1);
        return _solutions[index];
    };
    const std::vector<T>& get_solutions() const{
        return _solutions;
    };
    size_t size() const{
        return _solutions.size();
    }


protected:
    comparator_t _comparator; //return true if the first argument as higher quality than the second
    std::vector<T> _solutions;
};

/**
 * @brief Archive using only a distance function on the solution for addition.
 * It uses a distance threshold to decide if the quality of a solution in the archive needs to be compared with the new solution quality.
 * One additional template parameter D: the numerical type used for the distance.
 */
template<typename T, typename D>
class UnstructuredArchive: public Archive<T>{
public:
    using comparator_t = typename Archive<T>::comparator_t;
    using distance_fct_t = std::function<D(const T&, const T&)>;

    UnstructuredArchive(): Archive<T>(){}
    UnstructuredArchive(D threshold, distance_fct_t dist_fct, comparator_t comparator)
        : Archive<T>(comparator), _threshold(threshold), _distance_fct(dist_fct){}

    bool add_solution(const T& solution) override{
        std::vector<D> distances(this->_solutions.size());
        // Comparing with archive
        tbb::parallel_for(tbb::blocked_range<size_t>(0,this->_solutions.size()),
                          [&](tbb::blocked_range<size_t> r){
                              for(size_t i = r.begin(); i != r.end(); i++){
                                  distances[i] = _distance_fct(this->_solutions[i],solution);
                              }
                          });
        std::vector<size_t> indices(distances.size());
        std::iota(indices.begin(),indices.end(),0);
        std::sort(indices.begin(),indices.end(),[&](size_t i,size_t j){return distances[i] < distances[j];}); // Sorting distances
        if(distances[indices[0]] > _threshold){ //if the closest solution in archive is farther than _threshold
            this->_solutions.push_back(solution); //then add the new solution
            return true;
        }
        //else compare their quality
        T& stored_sol = this->_solutions[indices[0]];
        if(this->_comparator(solution,stored_sol)){//replace current stored solution with the new one if it has better quality.
            stored_sol = solution;
            return true;
        }
        return false; //new solution not added
    }
    void set_threshold(const D& thres){_threshold = thres;}

private:
    D _threshold;
    distance_fct_t _distance_fct;
};

/**
* @brief Grid archive to store solutions from MAP-Elites and similar algorithms.
* The type T of solution require a publicly available descriptor member of type std::vector<double>
*/
template<typename T>
class GridArchive: public Archive<T> {
public:
    using comparator_t = typename Archive<T>::comparator_t;

    GridArchive(): Archive<T>(){}
    GridArchive(const std::vector<int>& bins, const std::vector<std::tuple<double,double>>& boundaries,comparator_t comparator)
        : Archive<T>(comparator), _bins(bins), _boundaries(boundaries) {
        _dimensions = bins.size();
    }
    void reset() override {
        this->_solutions.clear();
        _indices.clear();
    }
    bool add_solution(const T& solution) override{
        std::vector<double> descriptor = solution.descriptor;
        if (descriptor.size() != _dimensions) {
            return false; // Descriptor dimension mismatch
        }
        std::vector<int> index(_dimensions);
        for (int i = 0; i < _dimensions; ++i) {
            double min_bound = std::get<0>(_boundaries[i]);
            double max_bound = std::get<1>(_boundaries[i]);
            if (descriptor[i] < min_bound || descriptor[i] > max_bound) {
                return false; // Descriptor out of bounds
            }
            double normalized = (descriptor[i] - min_bound) / (max_bound - min_bound);
            index[i] = std::min(static_cast<int>(normalized * _bins[i]), _bins[i] - 1);
        }

        if (_indices.find(index) == _indices.end()) {
            this->_solutions.push_back(solution);
            _indices[index] = static_cast<int>(this->_solutions.size()-1);
            return true;
        }else if(this->_comparator(solution, this->_solutions[_indices[index]])) {
            this->_solutions[_indices[index]] = solution;
            return true;

        }
        return false; // Slot already occupied
    }

    const std::map<std::vector<int>,int>& get_indices() const {
        return _indices;
    }

private:
    int _dimensions; // Number of dimensions
    std::vector<int> _bins; // Number of bins per dimension
    std::vector<std::tuple<double,double>> _boundaries; // Min and max for each dimension
    std::map<std::vector<int>,int> _indices; // Matrix of indices organised as a multi-dimensional grid
};

/**
* @brief Infinite Grid archive is a grid archive without boundaries. It is use to store solutions from MAP-Elites and similar algorithms.
* The type T of solution require a publicly available descriptor member of type std::vector<double>
*/
template<typename T>
class InfinteGridArchive: public Archive<T> {
public:
    using comparator_t = typename Archive<T>::comparator_t;

    InfinteGridArchive(): Archive<T>(){}
    InfinteGridArchive(const std::vector<double>& intervals,comparator_t comparator)
        : Archive<T>(comparator), _intervals(intervals){
        _dimensions = intervals.size();
    }
    void reset() override {
        this->_solutions.clear();
        _indices.clear();
    }
    bool add_solution(const T& solution) override{
        std::vector<double> descriptor = solution.descriptor;
        if (descriptor.size() != _dimensions) {
            return false; // Descriptor dimension mismatch
        }
        std::vector<int> index(_dimensions);
        for (int i = 0; i < _dimensions; ++i) {
            // double min_bound = std::get<0>(_boundaries[i]);
            // double max_bound = std::get<1>(_boundaries[i]);
            // if (descriptor[i] < min_bound || descriptor[i] > max_bound) {
            //     return false; // Descriptor out of bounds
            // }
            // double normalized = (descriptor[i] - min_bound) / (max_bound - min_bound);
            // index[i] = std::min(static_cast<int>(normalized * _bins[i]), _bins[i] - 1);
        }

        if (_indices.find(index) == _indices.end()) {
            this->_solutions.push_back(solution);
            _indices[index] = static_cast<int>(this->_solutions.size()-1);
            return true;
        }else if(_comparator(solution, this->_solutions[_indices[index]])) {
            this->_solutions[_indices[index]] = solution;
            return true;

        }
        return false; // Slot already occupied
    }

    const std::map<std::vector<int>,int>& get_indices() const {
        return _indices;
    }

private:
    int _dimensions; // Number of dimensions
    std::vector<double> _intervals; // intervals used to discretize each dimension
    std::map<std::vector<int>,int> _indices; // Matrix of indices organised as a multi-dimensional grid
};



}//apear
