#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include <memory>

#include <boost/serialization/array.hpp>

#include "apear/misc/rand_num.hpp"
#include "apear/settings.hpp"


namespace apear {


template<class sim_t>
class Environment{
public:

    using Ptr = std::shared_ptr<Environment>;
    using ConstPtr = std::shared_ptr<const Environment>;
    using Sim = sim_t;

    Environment(){
    }
    Environment(const Environment& env) :
        _name(env._name),
        _parameters(env._parameters),
        _rand_num(env._rand_num)
    {}

    ~Environment(){}

    /**
     * @brief Initialize the default environment scene and simulation time step size
     */
    virtual void init(Sim &sim) = 0;

    /**
     * @brief Calculate the fitness value of the robot
     * @param morph The pointer of the robot (morphology)
     */
    virtual std::vector<double> fitness_function(Sim &sim) = 0;

    virtual bool update(double time,Sim &sim) = 0;

    virtual void print_info(){}

    //GETTERS & SETTERS
    void set_parameters(const settings::ParametersMapPtr &param){_parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return _parameters;}
    const std::string &get_name(){return _name;}
    void set_randNum(misc::RandNum::Ptr &rn){_rand_num = rn;}

protected:
    std::string _name;
    ///setting of the environment
    settings::ParametersMapPtr _parameters;
    misc::RandNum::Ptr _rand_num;

};


}//apear

