#include "apear/data.hpp"
#include "apear/misc/utilities.hpp"
#include <sstream>

using namespace apear;

std::string act_obs_t::to_string() const{
    std::stringstream sstr;
    sstr << time << ";";
    sstr << observation[0];
    for(size_t i = 1; i < observation.size(); i++)
        sstr << "," << observation[i];
    sstr << ";" << action[0];
    for(size_t i = 1; i < action.size(); i++)
        sstr << "," << action[i];
    //TODO add return value to the string
    return sstr.str();
}

void act_obs_t::from_string(const std::string& str){
    std::vector<std::string> split_str;
    apear::misc::split_line(str,";",split_str);
    time = std::stod(split_str[0]);
    std::vector<std::string> obs_str, act_str;
    apear::misc::split_line(split_str[1],",",obs_str);
    apear::misc::split_line(split_str[2],",",act_str);
    for(const std::string& s: obs_str)
        observation.push_back(std::stod(s));
    for(const std::string& s: act_str)
        action.push_back(std::stod(s));
    //TODO add return value from the string
}

std::string waypoint_t::to_string() const{
    std::stringstream sstr;
    sstr << time << ";" << position[0] << "," << position[1] << "," << position[2] << ";"
         << euler_ori[0] << "," << euler_ori[1] << "," << euler_ori[2] << ";"
         << quat_ori[0]<< "," << quat_ori[1] << "," << quat_ori[2] << "," << quat_ori[3];
    return sstr.str();
}

void waypoint_t::from_string(const std::string &wp){
    std::stringstream sstr(wp);
    char delim[9];
    sstr >> time >> delim[0] >> position[0] >> delim[1] >> position[1] >> delim[2] >> position[2] >> delim[3]
        >> euler_ori[0] >> delim[4] >> euler_ori[1] >> delim[5] >> euler_ori[2]
        >> quat_ori[0] >> delim[6] >> quat_ori[1] >> delim[7] >> quat_ori[2] >> delim[8] >> quat_ori[3];
}

bool waypoint_t::is_nan(){
    return std::isnan(position[0]) || std::isnan(position[1]) || std::isnan(position[2]) ||
           std::isnan(euler_ori[0]) || std::isnan(euler_ori[1]) || std::isnan(euler_ori[2]) ||
            std::isnan(quat_ori[0]) || std::isnan(quat_ori[1]) || std::isnan(quat_ori[2]) || std::isnan(quat_ori[3]);
}

