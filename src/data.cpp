#include "apear/data.hpp"
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
    return sstr.str();
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

