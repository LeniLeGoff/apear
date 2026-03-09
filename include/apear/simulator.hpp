#pragma once

#include <memory>
#include "apear/environment.hpp"

namespace apear {


enum sim_state_t{
    IDLE = 0,
    INITIALIZED,
    FINISHED,
    RUNNING,
    ERROR
};

template<typename ind_t>
class Simulator{
public:
    using IndPtr = std::shared_ptr<ind_t>;
    typedef std::unique_ptr<Simulator> Ptr;
    typedef std::unique_ptr<const Simulator> ConstPtr;

    Simulator(){}
    Simulator(settings::ParametersMapPtr param, bool headless = true) : _parameters(param), _headless(headless){}
    Simulator(const Simulator& sim) :
        _individual_ready(sim._individual_ready),
        _env_initialized(sim._env_initialized),
        _parameters(sim._parameters),
        _state(sim._state),
        _headless(sim._headless)
    {}
    /**
     * @brief init_environment
     * @param env
     * @return
     */
    virtual bool init_environment(const Environment::Ptr &env) = 0;

    /**
     * @brief init simulator with the individual
     * @return true if the simulation started successfully, false otherwise
     */
    virtual bool init(const IndPtr &ind) = 0;

    /**
     * @brief step the simulation
     * @return true if the simulation is running, false otherwise
     */
    virtual bool step() = 0;

    /**
     * @brief stop the simulation
     * @return true if the simulation has stopped, false otherwise
     */
    virtual bool stop() = 0;

    virtual void update_ind(IndPtr &ind, const Environment::Ptr& env) = 0;


    /**
     * @brief querry the current state of the simulator
     * @return the current state of the simulator
     */
    virtual sim_state_t state(){
        return _state;
    }

    /**
     * @brief querry the current simulation time
     * @return the current simulation time;
     */
    virtual double time() = 0;

    virtual void reconnect() = 0;


    bool is_individual_ready(){return _individual_ready;}
    void ready_for_new_sim(){_individual_ready = false;}
    void sim_started(){_individual_ready = true;}
    void env_initialized(){_env_initialized = true;}
    bool is_env_initialized(){return _env_initialized;}


protected:
    bool _individual_ready = false;
    bool _env_initialized = false;
    settings::ParametersMapPtr _parameters;
    sim_state_t _state = sim_state_t::IDLE;
    bool _headless = true;

};//Simulator

}//apear

