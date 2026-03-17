#pragma once

#include <vector>
#include <memory>
#include "apear/settings.hpp"
#include "apear/misc/rand_num.hpp"

// #include <boost/serialization/map.hpp>
// #include <boost/serialization/shared_ptr.hpp>

namespace apear {
class  Genome
{
public:
    typedef std::shared_ptr<Genome> Ptr;
    typedef std::shared_ptr<const Genome> ConstPtr;

    Genome(int id = 0) : _id(id){
        _parents_ids= std::vector<int>(2,-1);
    }
    Genome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param, int id = 0);
    Genome(const Genome& gen) :
        _parameters(gen._parameters),
        _rand_num(gen._rand_num),
        _initialized(gen._initialized),
        _type(gen._type),
        _id(gen._id),
        _parents_ids(gen._parents_ids)
    {}

    /// This method deep copies the genome
    virtual Genome::Ptr clone() const = 0;

    /// Initialize the morph and its control
    virtual void init() = 0;

    virtual void mutate() = 0;

    virtual void crossover(const Genome::Ptr& partner,Genome::Ptr child1){
        child1 = clone();
    }

    virtual void symmetrical_crossover(const Genome::Ptr& partner,Genome::Ptr child1,Genome::Ptr child2){
        child1 = clone();
        child2 = partner->clone();
    }

    virtual void random(){}

    virtual std::string to_string() const = 0;
    virtual void from_string(const std::string&) = 0;
    virtual void from_file(const std::string&);

    //Getters & Setters
    void set_parameters(const settings::ParametersMapPtr &param){_parameters = param;}
    const settings::ParametersMapPtr &get_parameters() const {return _parameters;}
    virtual void set_randNum(const misc::RandNum::Ptr& rn){_rand_num = rn;}
    const std::string& get_type(){return _type;}
    const int id() const {return _id;}
    void set_id(int id){_id = id;}
    const std::vector<int>& get_parents_ids() const {return _parents_ids;}
    void set_parents_ids(const std::vector<int>& ids){_parents_ids = ids;}

    // template <class archive>
    // void serialize(archive &arch, const unsigned int v)
    // {
    //     arch & _id;
    //     arch & _type;
    //     arch & _parents_ids;
    //     //        arch & initialized;
    // }

protected:
    /// simulation setting shared by genome and EA
    settings::ParametersMapPtr _parameters;
    /// random number generator
    misc::RandNum::Ptr _rand_num;

    bool _initialized = false;
    std::string _type;
    int _id;
    std::vector<int> _parents_ids;


};

/**
 * @brief The EmptyGenome class is meant to be used when no genome is needed.
 */
class EmptyGenome : public Genome
{
public:
    EmptyGenome() {_type = "empty_genome";}
    Genome::Ptr clone() const override{return nullptr;}
    void init() override{}
    void mutate() override{}
    std::string to_string() const override{return "";}
    void from_string(const std::string &) override{}

    // template <class archive>
    // void serialize(archive &arch, const unsigned int v)
    // {
    //     arch & boost::serialization::base_object<Genome>(*this);
    // }
};


}//apear

