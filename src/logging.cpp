#include "apear/logging.hpp"
#include <filesystem>


using namespace apear;

std::string logging::log_folder = "";

void logging::create_log_folder(const std::string &exp_name){
    std::random_device rd;

    typedef std::chrono::duration<double,std::milli> milli_sec;
    std::chrono::time_point<std::chrono::high_resolution_clock,milli_sec> milli =
            std::chrono::time_point_cast<milli_sec>(std::chrono::high_resolution_clock::now());
    double time_milli = milli.time_since_epoch().count();
    time_milli = time_milli/(10000.f);
    time_milli = time_milli - static_cast<int>(time_milli);
    time_milli = std::trunc(time_milli*10000.f);
    std::time_t present_time = std::time(nullptr);
    std::tm* date = std::localtime(&present_time);
    std::stringstream stream;
    stream << date->tm_mday
        << "_" << date->tm_mon
        << "_" << date->tm_hour
        << "-" << date->tm_min
        << "-" << date->tm_sec
        << "-" << time_milli
        << "-" << rd();

    set_log_folder(exp_name + "_" + stream.str());
    create_folder(exp_name + "_" + stream.str());
}
void logging::set_log_folder(const std::string& name){
    log_folder = name;
}
void logging::create_folder(const std::string &name){
    if(!std::filesystem::exists(name))
        std::filesystem::create_directory(name);
}

void logging::save_string_to_file(const std::string &fileName, const std::string &data){
    std::ofstream out(log_folder + "/" + fileName,std::ios::out | std::ios::ate | std::ios::app);
    if(!out)
    {
        std::cerr << "unable to open : " << logging::log_folder + std::string("/")  + fileName << std::endl;
        return;
    }
    out << data;
    out.close();
}





