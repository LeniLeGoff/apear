#pragma once

#include <apear/genome.hpp>
#include <apear/control.hpp>
#include <apear/morphology.hpp>
#include <apear/learner.hpp>

#include <eigen3/Eigen/Core>

// #include <boost/serialization/shared_ptr.hpp>
// #include <boost/serialization/map.hpp>
// #include <boost/serialization/vector.hpp>
// #include <boost/archive/text_iarchive.hpp>
// #include <boost/archive/text_oarchive.hpp>

namespace apear {

typedef struct act_obs_sample{
    act_obs_sample(){}
    act_obs_sample(const std::vector<double>& obs, const std::vector<double>& act) :
        observation(obs), next_action(act){}
    std::vector<double> observation;
    std::vector<double> next_action;
    double ret;
    std::string to_string() const;
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & observation;
        arch & next_action;
    }
}act_obs_sample;
typedef std::vector<act_obs_sample> rollout_t;

class Individual
{
public:
    typedef std::shared_ptr<Individual> Ptr;
    typedef std::shared_ptr<const Individual> ConstPtr;

    Individual(){}
    Individual(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) :
        _parameters(param),
        _rand_num(rn),
        _is_eval(false)
    {}
    Individual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen);
    Individual(const Individual& ind) :
        _outputs(ind._outputs),
        _objectives(ind._objectives),
        _morphology(ind._morphology),
        _control(ind._control),
        _learner(ind._learner),
        _parameters(ind._parameters),
        _rand_num(ind._rand_num),
        _individual_id(ind._individual_id),
        _generation(ind._generation)
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

    virtual void update(double delta_time);
    virtual void mutate()
    {
        _morph_genome->mutate();
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
    int get_individual_id(){return _individual_id;}
    void set_individual_id(int i){_individual_id = i;}
    void set_generation(int g){_generation = g;}
    int get_generation(){return _generation;}
    void set_parameters(const settings::ParametersMapPtr &param){_parameters = param;}
    const settings::ParametersMapPtr &get_parameters() const {return _parameters;}
    bool isEvaluated(){return _is_eval;}
    void set_isEvaluated(bool b){_is_eval = b;}
    void set_client_id(int cid){_client_id = cid;}
    int get_client_id(){return _client_id;}

    const rollout_t &get_rollout() const {return _rollout;}
    void set_rollout(const rollout_t& ro){_rollout = ro;}

    virtual std::string to_string() const;
    virtual void from_string(const std::string &str);

    // template<class archive>
    // void serialize(archive &arch, const unsigned int v)
    // {
    //     arch & _objectives;
    //     arch & _ctrl_genome;
    //     arch & _morph_genome;
    //     arch & _individual_id;
    //     arch & _generation;
    // }
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

    bool _is_eval;

    int _individual_id;
    int _generation;

    int _client_id;
    double _sum_ctrl_freq = 0;
    rollout_t _rollout;

    //    std::function<Genome::Factory> createGenome;

    virtual void _create_controller() = 0;
    virtual void _create_morphology() = 0;
};



}//apear


