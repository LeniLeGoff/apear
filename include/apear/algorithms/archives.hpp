#pragma once
#include <iostream>
#include <functional>
#include <stdexcept>
#include <map>
#include <vector>
#include <oneapi/tbb.h>
#include "apear/misc/rand_num.hpp"

namespace apear{


struct selectors{
    template<typename solution_t>
    using selector_t = std::function<void(const std::vector<solution_t>&, const misc::RandNum::Ptr &,std::vector<solution_t>&)>;

    template<typename solution_t>
    static inline selector_t<solution_t> random = [](const std::vector<solution_t>& solutions,const misc::RandNum::Ptr & rand_num, std::vector<solution_t> &selection){
        for(size_t i = 0; i < selection.size(); i++){
            int index = rand_num->rand_int(0, static_cast<int>(solutions.size()) - 1);
            selection[i] = solutions.at(index);
        }
    };
};
/**
 * @brief Base class for Quality-Diversity archives
 * Implement a basic archive storing the solution in a vector with the most recent solution pushed back.
 */
template<typename T>
class Archive{
public:
    using comparator_t = std::function<bool(const T&, const T&)> ;

    Archive(){}
    Archive(comparator_t comparator, typename selectors::selector_t<T> selector ) :
        _comparator(comparator), _selector(selector){}

    virtual void reset(){
        _solutions.clear();
    }

    virtual bool add_solution(const T& solution){
        _solutions.push_back(solution);
        return true;
    }

    void push_back(const T& solution){
        this->_solutions.push_back(solution);
    }

    void select_solution(const misc::RandNum::Ptr &rand_num, T& selection){
        std::vector<T> select(1);
        _selector(_solutions,rand_num,selection);
        selection = select[0];
    }
    std::vector<T> select_solutions(int N, const misc::RandNum::Ptr &rand_num, std::vector<T>& selection){
        selection.resize(N);
        _selector(_solutions,rand_num,selection);
        return selection;
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

    virtual void update(){}
protected:
    comparator_t _comparator; //return true if the first argument as higher quality than the second
    std::vector<T> _solutions;
    typename selectors::selector_t<T> _selector;
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
    UnstructuredArchive(D threshold, distance_fct_t dist_fct, comparator_t comparator,
                        typename selectors::selector_t<T> selector, int nbr_ngb = 0)
        : Archive<T>(comparator,selector), _threshold(threshold), _distance_fct(dist_fct),_sparsity_number_neighbors(nbr_ngb){}

    bool add_solution(const T& solution) override{
        std::cout << "add_solution with archive size " << this->_solutions.size() << std::endl;
        if(this->_solutions.empty()){
            this->_solutions.push_back(solution);
            return true;
        }

        std::vector<D> distances(this->_solutions.size());
        // Comparing with archive
        tbb::parallel_for(tbb::blocked_range<size_t>(0,distances.size()),
                          [&](tbb::blocked_range<size_t> r){
            for(size_t i = r.begin(); i != r.end(); ++i)
            {
                distances[i] = _distance_fct(this->_solutions[i],solution);
            }
        });

        std::vector<size_t> indices(distances.size());
        std::iota(indices.begin(),indices.end(),0);
        std::sort(indices.begin(),indices.end(),[&](size_t i,size_t j){return distances[i] < distances[j];}); // Sorting distances
        if(distances[indices[0]] >= _threshold){ //if the closest solution in archive is farther than _threshold
            this->_solutions.push_back(solution); //then add the new solution
            if(_sparsity_number_neighbors > 0)
                _compute_sparsity_score(solution);
            return true;
        }
        //else compare their quality
        T& stored_sol = this->_solutions[indices[0]];
        if(this->_comparator(solution,stored_sol)){//replace current stored solution with the new one if it has better quality.
            stored_sol = solution;
            if(_sparsity_number_neighbors > 0)
                _compute_sparsity_score(solution);
            return true;
        }
        return false; //new solution not added
    }



    void update() override{
    }
    void set_threshold(const D& thres){_threshold = thres;}
    void set_sparsity_number_neighbors(int nbr_ngb){_sparsity_number_neighbors = nbr_ngb;}
    void compute_sparsity_scores(int nbr_ngb){
        std::vector<D> distances;
        for(const T &ind1: this->_solutions){
            for(const T &ind2: this->_solutions){
                if(ind1->id() == ind2->id())
                    continue;
                distances.push_back(_distance_fct(ind1,ind2));
            }
            std::sort(distances.begin(),distances.end());
            double sparsity_score = 0;
            size_t nbr_ngb_ = std::min(static_cast<size_t>(nbr_ngb),distances.size());
            for(size_t i = 0; i < nbr_ngb_; i++){
                sparsity_score += static_cast<double>(distances[i]);
            }
            ind1->set_sparsity_score(sparsity_score/static_cast<double>(nbr_ngb_));
            distances.clear();
        }
    }
    // const std::vector<double>& get_sparsity_scores(){return _sparsity_scores;}
private:
    D _threshold;
    distance_fct_t _distance_fct;
    // std::vector<double> _sparsity_scores;
    int _sparsity_number_neighbors = 0;

    void _compute_sparsity_score(const T& solution){
        std::vector<D> distances;
        for(const T &ind: this->_solutions){
            if(ind->get_morph_genome()->id() == solution->get_morph_genome()->id())
                continue;
            distances.push_back(_distance_fct(ind,solution));
        }
        std::vector<size_t> indices(distances.size());
        std::iota(indices.begin(),indices.end(),0);
        std::sort(indices.begin(),indices.end(),[&](size_t i,size_t j){return distances[i] < distances[j];}); // Sorting distances
        double sparsity_score = 0;
        size_t nbr_ngb_ = std::min(static_cast<size_t>(_sparsity_number_neighbors),distances.size());
        solution->nn_distances.clear();
        solution->nn_distances.resize(nbr_ngb_);
        for(size_t i = 0; i < nbr_ngb_; i++){
            solution->nn_distances[i] = distances[indices[i]];
            sparsity_score += static_cast<double>(distances[indices[i]]);
            this->_solutions[indices[i]]->update_sparsity_score(distances[indices[i]]);
        }
        solution->set_sparsity_score(sparsity_score/static_cast<double>(nbr_ngb_));
    }

};



/**
* @brief Grid archive to store solutions from MAP-Elites and similar algorithms.
* The type T of solution require a publicly available descriptor member of type std::vector<double>
*/
template<typename T>
class GridArchive: public Archive<T> {
public:
    using comparator_t = typename Archive<T>::comparator_t;
    using descriptor_t = std::function<std::vector<double>(const T&)>;

    GridArchive(): Archive<T>(){}
    GridArchive(const std::vector<int>& bins, const std::vector<std::tuple<double,double>>& boundaries,
                comparator_t comparator, typename selectors::selector_t<T> selector)
        : Archive<T>(comparator,selector), _bins(bins), _boundaries(boundaries) {
        _dimensions = bins.size();
    }
    void reset() override {
        this->_solutions.clear();
        _indices.clear();
    }
    bool add_solution(const T& solution) override{
        solution->set_descriptors(_descriptor_fct(solution));
        std::vector<double> descriptor = solution->get_descriptors();
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
    descriptor_t _descriptor_fct;

};

/**
* @brief Infinite Grid archive is a grid archive without boundaries. It is use to store solutions from MAP-Elites and similar algorithms.
* The type T of solution require a publicly available descriptor member of type std::vector<double>
*/
template<typename T>
class InfiniteGridArchive: public Archive<T> {
public:
    using comparator_t = typename Archive<T>::comparator_t;
    using descriptor_t = std::function<std::vector<double>(const T&)>;

    InfiniteGridArchive(): Archive<T>(){}
    InfiniteGridArchive(const std::vector<double>& intervals,comparator_t comparator, descriptor_t descriptor_fct,
                       typename selectors::selector_t<T> selector)
        : Archive<T>(comparator,selector), _intervals(intervals), _descriptor_fct(descriptor_fct) {
        _dimensions = intervals.size();
    }
    void reset() override {
        this->_solutions.clear();
        _indices.clear();
    }
    bool add_solution(const T& solution) override{
        solution->set_descriptors(_descriptor_fct(solution));
        std::vector<double> descriptor = solution->get_descriptors();
        if (descriptor.size() != static_cast<size_t>(_dimensions)) {
            return false; // Descriptor dimension mismatch
        }
        std::vector<int> index(_dimensions);
        for (int i = 0; i < _dimensions; ++i)
            index[i] = static_cast<int>(std::floor(descriptor[i] / _intervals[i]));

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
    std::vector<double> _intervals; // intervals used to discretize each dimension
    std::map<std::vector<int>,int> _indices; // Matrix of indices organised as a multi-dimensional grid
    descriptor_t _descriptor_fct;
};




}//apear
