#pragma once

#include <memory>
#include <vector>
#include <stdexcept>

namespace apear {


enum sim_state_t{
    IDLE = 0,
    STARTED,
    STOPPED,
    RUNNING,
    ERROR
};

class Simulator{
public:
    typedef std::unique_ptr<Simulator> Ptr;
    typedef std::unique_ptr<const Simulator> ConstPtr;

    Simulator(){}
    Simulator(const Simulator& sim) :
        _individual_ready(sim._individual_ready),
        _env_initialized(sim._env_initialized)
    {}


    /**
     * @brief start the simulation in stepping mode
     * @param wether to activate stepping mode or not
     * @return true if the simulation started successfully, false otherwise
     */
    virtual bool start(bool stepping = false) = 0;

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

    /**
     * @brief run the simulation for a set duration in seconds
     * @param duration of the simulation in seconds
     * @return true if the simulation stopped normally, false otherwise
     */
    virtual bool run(double duration) = 0;

    /**
     * @brief run the simulation in stepping mode for a set duration in seconds
     * @param duration of the simulation in seconds
     * @return true if the simulation stopped normally, false otherwise
     */
    virtual bool run_stepping(double duration) = 0;

    /**
     * @brief querry the current state of the simulator
     * @return the current state of the simulator
     */
    virtual sim_state_t state() = 0;

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


    static std::string state_to_string(int state);

private:
    bool _individual_ready = false;
    bool _env_initialized = false;

    void _print_error(const std::string &fct_name,const std::runtime_error& error,  const std::vector<std::string> fct_args = {});

};//Simulator

}//apear

