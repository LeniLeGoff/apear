#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include <memory>

#include <boost/serialization/array.hpp>

#include "apear/misc/rand_num.hpp"
#include "apear/settings.hpp"
#include "apear/individual.hpp"


namespace apear {

struct waypoint{
    std::array<double,3> position = {0,0,0};
    std::array<double,3> orientation = {0,0,0};

    waypoint(){}

    // waypoint(const waypoint &wp){
    //     position[0] = wp.position[0];
    //     position[1] = wp.position[1];
    //     position[2] = wp.position[2];

    //     orientation[0] = wp.orientation[0];
    //     orientation[1] = wp.orientation[1];
    //     orientation[2] = wp.orientation[2];
    // }

    std::string to_string() const{
        std::stringstream sstr;
        sstr << position[0] << "," << position[1] << "," << position[2] << ";"
             << orientation[0] << "," << orientation[1] << "," << orientation[2];
        return sstr.str();
    }

    void from_string(const std::string& wp){
        std::stringstream sstr(wp);
        char delim[5];
        sstr >> position[0] >> delim[0] >> position[1] >> delim[1] >> position[2] >> delim[2]
            >> orientation[0] >> delim[3] >> orientation[1] >> delim[4] >> orientation[2];
    }

    bool is_nan(){
        return std::isnan(position[0]) || std::isnan(position[1]) || std::isnan(position[2]) ||
               std::isnan(orientation[0]) || std::isnan(orientation[1]) || std::isnan(orientation[2]);
    }

    // template<class archive>
    // void serialize(archive &arch, const unsigned int v)
    // {
    //     arch & position;
    //     arch & orientation;
    // }

};

class Environment{
public:

    typedef std::shared_ptr<Environment> Ptr;
    typedef std::shared_ptr<const Environment> ConstPtr;
    typedef Environment::Ptr (Factory)(const settings::ParametersMapPtr&);


    Environment(){
        _trajectory.resize(0);
        _final_position.resize(3);
    }
    Environment(const Environment& env) :
        _name(env._name),
        _parameters(env._parameters),
        _rand_num(env._rand_num),
        _trajectory(env._trajectory),
        _final_position(env._final_position){}

    ~Environment(){}

    /**
     * @brief Initialize the default environment scene and simulation time step size
     */
    virtual void init() = 0;

    /**
     * @brief Calculate the fitness value of the robot
     * @param morph The pointer of the robot (morphology)
     */
    virtual std::vector<double> fitness_function(const Individual::Ptr &ind) = 0;

    virtual void update_info(double time) = 0;

    virtual void print_info(){}

    virtual void clear_data(){
        _trajectory.clear();
    }

    //GETTERS & SETTERS
    void set_parameters(const settings::ParametersMapPtr &param){_parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return _parameters;}
    const std::string &get_name(){return _name;}
    void set_randNum(misc::RandNum::Ptr &rn){_rand_num = rn;}
    const std::vector<waypoint> &get_trajectory(){return _trajectory;}
    const std::vector<double> &get_final_position(){return _final_position;}

protected:
    std::string _name;
    ///setting of the environment
    settings::ParametersMapPtr _parameters;
    misc::RandNum::Ptr _rand_num;
    std::vector<waypoint> _trajectory;
    std::vector<double> _final_position;
};

class DummyEnv : public Environment
{
public:
    void init() override {}
    std::vector<double> fitness_function(const Individual::Ptr &ind) override {return {0};}
    void update_info(double time) override {}
};

}//apear

