#pragma once

#include "apear/data.hpp"
#include <apear/genome.hpp>
#include <apear/control.hpp>
#include <apear/morphology.hpp>
#include <apear/learner.hpp>

#include <eigen3/Eigen/Core>

namespace apear {

class Individual
{
public:
    typedef std::shared_ptr<Individual> Ptr;
    typedef std::shared_ptr<const Individual> ConstPtr;

    Individual(){}
    Individual(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) :
        _parameters(param),
        _rand_num(rn)
    {}
    Individual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen);
    Individual(const Individual& ind) :
        _outputs(ind._outputs),
        _objectives(ind._objectives),
        _morph_genome(ind._morph_genome),
        _ctrl_genome(ind._ctrl_genome),
        _morphology(ind._morphology),
        _control(ind._control),
        _learner(ind._learner),
        _parameters(ind._parameters),
        _rand_num(ind._rand_num)
    {}

    virtual Individual::Ptr clone() = 0;

    virtual void init()
    {
        if(_morph_genome != nullptr)
            _morph_genome->init();
        if(_ctrl_genome != nullptr)
            _ctrl_genome->init();
    }
    virtual void random(){
        if(_morph_genome != nullptr)
            _morph_genome->random();
        if(_ctrl_genome != nullptr)
            _ctrl_genome->random();
    }
    virtual void decode()
    {
        if(_morph_genome != nullptr)
            _create_morphology();
        if(_ctrl_genome != nullptr)
            _create_controller();
    }

    virtual void mutate()
    {
        if(_morph_genome != nullptr)
            _morph_genome->mutate();
        if(_ctrl_genome != nullptr)
            _ctrl_genome->mutate();
    }
    virtual void crossover(const Individual::Ptr& partner, Individual *child);
    virtual void symmetrical_crossover(const Individual::Ptr& partner, Individual *child1, Individual *child2);


    virtual Eigen::VectorXd descriptor(){return Eigen::VectorXd::Zero(1);}

    void set_rand_num(const misc::RandNum::Ptr &rn){_rand_num = rn;}

    bool is_init(){return (_control != nullptr && _morphology != nullptr);}

    //Getters & Setters
    const std::vector<double> &get_outputs(){return _outputs;}
    const Morphology::Ptr &get_morphology(){return _morphology;}
    const Control::Ptr &get_control(){return _control;}
    const Genome::Ptr &get_morph_genome(){return _morph_genome;}
    const Genome::Ptr &get_ctrl_genome(){return _ctrl_genome;}
    void set_objectives(const std::vector<double> &objs){_objectives = objs;}
    const std::vector<double> &get_objectives(){return _objectives;}
    void set_parameters(const settings::ParametersMapPtr &param){_parameters = param;}
    const settings::ParametersMapPtr &get_parameters() const {return _parameters;}

    const int id() const {return _morph_genome->id();}
    void set_id(int id){_morph_genome->set_id(id);}

    virtual std::string to_string() const;
    virtual void from_string(const std::string &str);



    const Learner::Ptr & get_learner(){return _learner;}

protected:
    std::vector<double> _outputs;
    std::vector<double> _objectives;
    Genome::Ptr _morph_genome;
    Genome::Ptr _ctrl_genome;
    Morphology::Ptr _morphology;
    Control::Ptr _control;
    Learner::Ptr _learner;

    settings::ParametersMapPtr _parameters;
    misc::RandNum::Ptr _rand_num;


    //    std::function<Genome::Factory> createGenome;

    virtual void _create_controller() = 0;
    virtual void _create_morphology() = 0;
};



}//apear


