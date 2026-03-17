#pragma once

#include <memory>
#include "apear/settings.hpp"

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
        _parameters(sim._parameters),
        _state(sim._state),
        _headless(sim._headless)
    {}

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

    virtual bool update_robot(const IndPtr &ind){return true;}

    /**
     * @brief stop the simulation
     * @return true if the simulation has stopped, false otherwise
     */
    virtual bool stop() = 0;


    /**
     * @brief querry the current state of the simulator
     * @return the current state of the simulator
     */
    virtual sim_state_t state(){
        return _state;
    }

    void set_state(sim_state_t state){_state = state;}

    /**
     * @brief querry the current simulation time
     * @return the current simulation time;
     */
    virtual double time() const = 0;

    virtual void reconnect() = 0;



protected:
    settings::ParametersMapPtr _parameters;
    sim_state_t _state = sim_state_t::IDLE;
    bool _headless = true;

};//Simulator

}//apear

