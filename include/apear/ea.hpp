#pragma once

#include <chrono>
#include <memory>

#include "apear/misc/rand_num.hpp"
#include "apear/settings.hpp"
#include "apear/individual.hpp"



namespace apear{

/**
 * @brief Base class for all the EA algorithms in the framework. non copyable
 */
template<class ind_t>
class EA
{
public:
    using IndPtr = std::shared_ptr<ind_t>;
    typedef std::unique_ptr<EA> Ptr;
    typedef std::unique_ptr<const EA> ConstPtr;

    /**
     * @brief default constructor
     */
    EA(){}
    EA(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) :
        _parameters(param),
        _rand_num(rn)
    {}

    /**
     * @brief Initialisation of the algorithm. Called only one time on starting up
     */
    virtual void init() = 0;

    /**
     * @brief Update method is called after each evaluation of an individual.
     * @return If the algorithm can continue to the next ind
     */
    virtual bool update() = 0;


    /**
     * @brief ending condition of the algorithm
     * @return true if ending condition is met
     */
    virtual bool is_finish() = 0;


    //GETTERS & SETTERS
    std::vector<IndPtr> &eval_queue() {return _eval_queue;}
    std::vector<IndPtr> &evaluated() {return _evaluated;}
    const settings::ParametersMapPtr &get_parameters() const {return _parameters;}
    const misc::RandNum::Ptr get_randomNum() const {return _rand_num;}
    void set_randomNum(const misc::RandNum::Ptr& rn){_rand_num = rn;}
    void set_parameters(const settings::ParametersMapPtr& param){_parameters = param;}

protected:
    std::vector<IndPtr> _eval_queue;
    std::vector<IndPtr> _evaluated;
    ///set the environment type, evolution type...
    settings::ParametersMapPtr _parameters;
    ///random number generator for EA
    misc::RandNum::Ptr _rand_num;
    int _highest_id = 0; //highest id of individual generated so far.
};


/**
 * @brief Random elitist search: population based random elitist exploratio.
 *  Reproduction: taking the best individual of the population, mutate it to produce a new offspring.
 *  Normalised maximisation.
 */
template<class ind_t>
class RandomElitistSearch : public EA<ind_t>{
public:
    using IndPtr = std::shared_ptr<ind_t>;
    typedef std::unique_ptr<RandomElitistSearch> Ptr;
    typedef std::unique_ptr<const RandomElitistSearch> ConstPtr;

    RandomElitistSearch() : EA<ind_t>(){}
    RandomElitistSearch(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) :
        EA<ind_t>(rn,param)
    {}

    void init() override{
        int pop_size = settings::getParameter<settings::Integer>(this->_parameters,"#populationSize").value;
        for(int i = 0; i < pop_size; i++){

            IndPtr ind = std::make_shared<ind_t>(this->_rand_num,this->_parameters);
            ind->init();
            this->_eval_queue.push_back(ind);
        }
    }
    bool update() override{
        if(this->_evaluated.empty())
            return false; //no update yet

        _number_evaluations += this->evaluated().size();

        int pop_size = settings::getParameter<settings::Integer>(this->_parameters,"#populationSize").value;
        //update the best individual
        double best_obj;
        if(_best_ind == nullptr)
            best_obj = -1;
        else best_obj = _best_ind->get_objectives()[0];
        for(IndPtr ind: this->_evaluated){
            if(ind->get_objectives()[0] > best_obj){
                best_obj = ind->get_objectives()[0];
                _best_ind = ind;
            }
        }
        //fill up the evaluation queue with offspring of the best individual
        while(this->eval_queue().size() < pop_size){
            IndPtr new_ind = std::make_shared<ind_t>(*_best_ind);
            new_ind->mutate();
            this->_eval_queue.push_back(new_ind);

        }
        this->_evaluated.clear();
        return true;
    }
    bool is_finish() override{
        if(_number_evaluations >= settings::getParameter<settings::Integer>(this->_parameters,"#maxNbrEval").value)
            return true;
        return false;
    }
private:
    IndPtr _best_ind = nullptr;
    int _number_evaluations = 0;
};


}//apear

