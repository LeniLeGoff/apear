#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "apear/misc/rand_num.hpp"
#include "apear/settings.hpp"
#include "apear/ea.hpp"
#include "apear/logging.hpp"

#include "apear/simulator.hpp"
#include "apear/environment.hpp"

namespace apear{

enum EvalOrder{
    FIFO = 0,
    FILO,
    RANDOM,
    FIFO_RANDOM
};

class AsyncDealer
{
public:
    typedef std::unique_ptr<AsyncDealer> Ptr;
    typedef std::unique_ptr<const AsyncDealer> ConstPtr;

    AsyncDealer(){}

    virtual bool init() = 0;
    void execute();
    bool update_simulators();
    void individuals_distribution();

    const settings::ParametersMapPtr &get_parameters(){return _parameters;}
    void set_parameters(const settings::ParametersMapPtr &param){_parameters = param;}
    const misc::RandNum::Ptr &get_rand_num(){return _rand_num;}
    void set_rand_num(const misc::RandNum::Ptr &rn){_rand_num = rn;}

    void save_logs(bool end_of_gen = false);

private:
    ///pointer to settting of EA
    settings::ParametersMapPtr _parameters;
    /// pointer to EA
    EA::Ptr _ea;
    /// pointer to random number generator of EA
    misc::RandNum::Ptr _rand_num;
    /// Pointer to the Environment class
    Environment::Ptr _environment;
    ///Individual currently evaluated
    Individual::Ptr _current_ind;



    std::vector<Logging::Ptr> _logging_fcts;
    int _population_size;

    std::vector<Simulator> _simulators;
    int _nbr_of_instances = 1;
    std::vector<Individual::Ptr> _ind_vec;
    std::vector<int> _idx_vec;
    std::vector<int> _eval_queue;


    hr_clock::time_point _reference_time;
    hr_clock::time_point _start_eval_time;
    hr_clock::time_point _end_eval_time;

    hr_clock::time_point _start_overhead_time;
    hr_clock::time_point _end_overhead_time;

    bool _is_all_simulators_finished();
    const int _max_connection_trials = 3;
};

}//apear



