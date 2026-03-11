#pragma once

#include <vector>
#include <memory>
#include "apear/misc/rand_num.hpp"
#include "apear/settings.hpp"

namespace apear {

/// abstract class
class Control
{
public:

    typedef std::shared_ptr<Control> Ptr;
    typedef std::shared_ptr<const Control> ConstPtr;

    Control(){}
    Control(const misc::RandNum::Ptr& rand_num, const settings::ParametersMapPtr &param) :
        _rand_num(rand_num),
        _parameters(param)
    {}
    Control(const Control& ctrl) :
        _rand_num(ctrl._rand_num),
        _parameters(ctrl._parameters)
    {}

    virtual Control::Ptr clone() const = 0;

    /**
    @brief Take sensory values and returns controls values.
    @param sensorValues sensor values
    */
    virtual std::vector<double> update(const std::vector<double> &sensorValues) = 0;

    //GETTERS & SETTERS
    void set_parameters(const settings::ParametersMapPtr &param){_parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return _parameters;}
    void set_random_number(const misc::RandNum::Ptr &rnd){_rand_num = rnd;}

protected:
    misc::RandNum::Ptr _rand_num;
    settings::ParametersMapPtr _parameters;
};

}//apear

