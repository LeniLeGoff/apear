#pragma once

#include <fstream>
#include <memory>
#include "apear/ea.hpp"

namespace apear {
struct logging{

static std::string log_folder;

static void create_log_folder(const std::string &exp_name);
static void create_folder(const std::string &name);
static void set_log_folder(const std::string&);

// Takes an arbitrary string input and saves it as a new file in the log_folder:
static void save_string_to_file(const std::string &fileName, const std::string &data);

};

template<class ind_t, class sim_t>
class Logging
{
public:
    typedef std::shared_ptr<Logging> Ptr;
    typedef std::shared_ptr<const Logging> ConstPtr;
    using IndPtr = std::shared_ptr<ind_t>;
    using SimPtr = std::shared_ptr<sim_t>;

    Logging(){}
    Logging(const std::string &file):_log_file(file){}
    Logging(const Logging& l) :
        _log_file(l._log_file){}
    virtual ~Logging(){}

    virtual void register_data(const IndPtr &ind,const sim_t &sim){};
    virtual void saveLog(const typename EA<ind_t>::Ptr &ind) = 0;
    virtual void loadLog(const std::string &file = std::string()) {};
    bool openOLogFile(std::ofstream& logFileStream){
        logFileStream.open(logging::log_folder + std::string("/")  + _log_file, std::ios::out | std::ios::ate | std::ios::app);

        if(!logFileStream)
        {
            std::cerr << "unable to open : " << logging::log_folder + std::string("/")  + _log_file << std::endl;
            return false;
        }

        return true;
    }
    bool openILogFile(std::ifstream& logFileStream){
        logFileStream.open(logging::log_folder + std::string("/")  + _log_file);

        if(!logFileStream)
        {
            std::cerr << "unable to open : " << logging::log_folder + std::string("/")  + _log_file << std::endl;
            return false;
        }

        return true;
    }

    bool openOLogFile(std::ofstream& logFileStream,const std::string &log_file){
        logFileStream.open(logging::log_folder + std::string("/")  + log_file, std::ios::out | std::ios::ate | std::ios::app);

        if(!logFileStream)
        {
            std::cerr << "unable to open : " << logging::log_folder + std::string("/")  + log_file << std::endl;
            return false;
        }

        return true;
    }
    bool openILogFile(std::ifstream& logFileStream,const std::string &log_file){
        logFileStream.open(logging::log_folder + std::string("/")  + log_file);

        if(!logFileStream)
        {
            std::cerr << "unable to open : " << logging::log_folder + std::string("/")  + log_file << std::endl;
            return false;
        }

        return true;
    }

    //SETTERS && GETTERS
    const std::string &get_logFile(){return _log_file;}
    void set_logFile(const std::string& file){_log_file = file;}

protected:
    std::string _log_file;
};








} //apear

