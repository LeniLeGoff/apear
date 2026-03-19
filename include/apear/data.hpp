#pragma once

#include <vector>
#include <string>
#include <array>
#include <cmath>

namespace apear{

struct act_obs_t{
    act_obs_t(){}
    act_obs_t(const std::vector<double>& obs, const std::vector<double>& act) :
        observation(obs), action(act){}
    act_obs_t(double t, const std::vector<double>& obs, const std::vector<double>& act) :
         time(t), observation(obs), action(act){}
    double time;
    std::vector<double> observation;
    std::vector<double> action;
    double ret;
    std::string to_string() const;
    void from_string(const std::string& str);

};
using rollout_t = std::vector<act_obs_t>;


struct waypoint_t{
    double time;
    std::array<double,3> position = {0,0,0};
    std::array<double,3> euler_ori = {0,0,0};
    std::array<double,4> quat_ori = {0,0,0,0};

    waypoint_t(){}

    std::string to_string() const;

    void from_string(const std::string& wp);

    bool is_nan();
};

using trajectory_t = std::vector<waypoint_t>;


}//apear

