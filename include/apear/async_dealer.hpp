#pragma once

#include <vector>
#include <memory>
#include <tbb/tbb.h>

#include "apear/misc/rand_num.hpp"
#include "apear/settings.hpp"
#include "apear/ea.hpp"
#include "apear/logging.hpp"

#include "apear/simulator.hpp"
#include "apear/environment.hpp"

namespace apear{

using hr_clock = std::chrono::high_resolution_clock;

enum EvalOrder{
    FIFO = 0,
    FILO,
    RANDOM,
    FIFO_RANDOM
};

template<class ind_t, class sim_t>
class AsyncDealer
{
public:
    using EAPtr = std::unique_ptr<EA<ind_t>>;
    using IndPtr = std::shared_ptr<ind_t>;
    using EnvPtr = std::shared_ptr<Environment<sim_t>>;
    typedef std::unique_ptr<AsyncDealer> Ptr;
    typedef std::unique_ptr<const AsyncDealer> ConstPtr;

    AsyncDealer() = delete;
    AsyncDealer(const settings::ParametersMapPtr &param, const misc::RandNum::Ptr &rn) : _parameters(param), _rand_num(rn){}

    bool init(int nbr_sim = 1, bool headless = true){
        for(int i = 0; i < nbr_sim; i++){
            _simulators.push_back(sim_t(_parameters, headless));
        }
        _ind_vec.resize(nbr_sim);
        return true;
    }
    bool update_simulators(){
        bool verbose = settings::getParameter<settings::Boolean>(_parameters,"#verbose").value;
        // float sim_duration = settings::getParameter<settings::Float>(_parameters,"#maxEvalTime").value;
        if(!_ea->eval_queue().empty())
            individuals_distribution();

        //Update simulators
        tbb::parallel_for(tbb::blocked_range<size_t>(0,_simulators.size()),
                          [&](tbb::blocked_range<size_t> r){
                              for(size_t sim_idx = r.begin(); sim_idx != r.end(); ++sim_idx)
                              {
        // for(size_t sim_idx = 0; sim_idx < _simulators.size(); sim_idx++){
            if(_ind_vec[sim_idx] == nullptr)
                continue;
            sim_t &sim = _simulators[sim_idx];
            if(sim.state() == sim_state_t::IDLE){
                if(verbose)
                    std::cout << "simulation " << sim_idx <<  " initializing" << std::endl;

                _environment->init(sim);
                sim.init(_ind_vec[sim_idx]);
            }else if(sim.state() == sim_state_t::INITIALIZED){
                if(!sim.update_robot(_ind_vec[sim_idx]) || //if update robot return false it means no controller.
                    !_environment->update(sim.time(),sim)){// or if env update return false it corresponds to an early stopping condition.
                    sim.stop();
                    continue;
                }
                register_data(_ind_vec[sim_idx],sim);
                if(sim.step()){
                    // if(verbose)
                    // std::cout << "simulation " << sim_idx <<  " running" << std::endl;
                }else{
                    std::cerr << "simulation " << sim_idx << " encountered an error while running" << std::endl;
                }

            }else if(sim.state() == sim_state_t::FINISHED){
                if(verbose)
                    std::cout << "simulation " << sim_idx <<  " finished" << std::endl;
                _ind_vec[sim_idx]->set_objectives(_environment->fitness_function(sim));
                sim.set_state(sim_state_t::IDLE);
                if(verbose)
                    std::cout << "ind " << _ind_vec[sim_idx]->get_morph_genome()->id()
                              << " as fitness " << _ind_vec[sim_idx]->get_objectives()[0] << std::endl;
                assert(_ind_vec[sim_idx] != nullptr);
                _ea->evaluated().push_back(std::move(_ind_vec[sim_idx]));
            }
        }//for each simulators
        });//tbb::parallel
        _ea->update();
        save_logs();
        if(_ea->is_finish()){
            if(verbose)
            {
                std::cout << "---------------------" << std::endl;
                std::cout << "Evolution is Finished" << std::endl;
                std::cout << "---------------------" << std::endl;
            }
            return false;
        }
        return true;
    }
    void individuals_distribution(){
        for(int i = 0; i < _ind_vec.size(); i++){
            if(_ea->eval_queue().empty() || _ind_vec[i] != nullptr)
                break;
            int eval_order = settings::getParameter<settings::Integer>(_parameters,"#evaluationOrder").value;
            if(eval_order == EvalOrder::FILO){
                //First in Last out
                _ind_vec[i] = std::move(_ea->eval_queue().back());
                _ea->eval_queue().erase(_ea->eval_queue().begin()+_ea->eval_queue().size()-1);
            }
            else if(eval_order == EvalOrder::FIFO){
                //First in First out
                _ind_vec[i] = std::move(_ea->eval_queue().front());
                _ea->eval_queue().erase(_ea->eval_queue().begin());
            }
        }
    }

    const settings::ParametersMapPtr &get_parameters(){return _parameters;}
    void set_parameters(const settings::ParametersMapPtr &param){_parameters = param;}
    const misc::RandNum::Ptr &get_rand_num(){return _rand_num;}
    void set_rand_num(const misc::RandNum::Ptr &rn){_rand_num = rn;}

    void set_environment(const EnvPtr &env){_environment = env;}

    void set_ea(EAPtr &ea){ea.swap(_ea);}

    void save_logs(){
        for(const auto &log : _logging_fcts){      
            log->saveLog();
        }
    }

    void register_data(const IndPtr &ind,const sim_t &sim){
        for(const auto &log : _logging_fcts){
            log->register_data(ind,sim);
        }
    }
    void add_logging(const typename Logging<ind_t,sim_t>::Ptr &log){
        _logging_fcts.push_back(log);
    }

private:
    ///pointer to setting of EA
    settings::ParametersMapPtr _parameters;
    /// pointer to EA
    EAPtr _ea;
    /// pointer to random number generator of EA
    misc::RandNum::Ptr _rand_num;
    /// Pointer to the Environment class
    EnvPtr _environment;

    std::vector<typename Logging<ind_t,sim_t>::Ptr> _logging_fcts;
    int _population_size;

    std::vector<sim_t> _simulators;
    int _nbr_of_instances = 1;
    std::vector<IndPtr> _ind_vec;


    hr_clock::time_point _reference_time;
    hr_clock::time_point _start_eval_time;
    hr_clock::time_point _end_eval_time;

    hr_clock::time_point _start_overhead_time;
    hr_clock::time_point _end_overhead_time;

    bool _is_all_simulators_finished();
    const int _max_connection_trials = 3;
};

}//apear



