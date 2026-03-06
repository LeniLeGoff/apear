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

template<class ind_t>
class Logging
{
public:
    typedef std::shared_ptr<Logging> Ptr;
    typedef std::shared_ptr<const Logging> ConstPtr;

    Logging() : end_of_gen(true), end_of_run(false){}
    Logging(bool eog, bool eor = false) : end_of_gen(eog), end_of_run(eor){}
    Logging(const std::string &file, bool eog = true) : end_of_gen(eog){logFile = file;}
    Logging(const Logging& l) :
        logFile(l.logFile),
        end_of_gen(l.end_of_gen),
        end_of_run(l.end_of_run){}
    virtual ~Logging(){}

    virtual void saveLog(typename EA<ind_t>::Ptr& ea) = 0;
    virtual void loadLog(const std::string &file = std::string()) = 0;
    bool openOLogFile(std::ofstream& logFileStream){
        logFileStream.open(logging::log_folder + std::string("/")  + logFile, std::ios::out | std::ios::ate | std::ios::app);

        if(!logFileStream)
        {
            std::cerr << "unable to open : " << logging::log_folder + std::string("/")  + logFile << std::endl;
            return false;
        }

        return true;
    }
    bool openILogFile(std::ifstream& logFileStream){
        logFileStream.open(logging::log_folder + std::string("/")  + logFile);

        if(!logFileStream)
        {
            std::cerr << "unable to open : " << logging::log_folder + std::string("/")  + logFile << std::endl;
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
    const std::string &get_logFile(){return logFile;}
    void set_logFile(const std::string& file){logFile = file;}
    bool isEndOfGen(){return end_of_gen;}
    bool isEndOfRun(){return end_of_run;}
    void set_end_of_gen(bool eog){end_of_gen = eog;}

protected:
    std::string logFile;
    bool end_of_gen = true;
    bool end_of_run = false;
};








} //apear

