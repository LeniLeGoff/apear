#include "apear/data.hpp"
#include <sstream>

using namespace apear;

std::string act_obs_t::to_string() const{
    std::stringstream sstr;
    sstr << observation[0];
    for(size_t i = 1; i < observation.size(); i++)
        sstr << "," << observation[i];
    sstr << ";" << next_action[0];
    for(size_t i = 1; i < next_action.size(); i++)
        sstr << "," << next_action[i];
    return sstr.str();
}

std::string waypoint_t::to_string() const{
    std::stringstream sstr;
    sstr << position[0] << "," << position[1] << "," << position[2] << ";"
         << orientation[0] << "," << orientation[1] << "," << orientation[2];
    return sstr.str();
}

void waypoint_t::from_string(const std::string &wp){
    std::stringstream sstr(wp);
    char delim[5];
    sstr >> position[0] >> delim[0] >> position[1] >> delim[1] >> position[2] >> delim[2]
        >> orientation[0] >> delim[3] >> orientation[1] >> delim[4] >> orientation[2];
}

bool waypoint_t::is_nan(){
    return std::isnan(position[0]) || std::isnan(position[1]) || std::isnan(position[2]) ||
           std::isnan(orientation[0]) || std::isnan(orientation[1]) || std::isnan(orientation[2]);
}
