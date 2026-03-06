#include "apear/individual.hpp"

using namespace apear;

Individual::Individual(const Genome::Ptr &morph_gen,const Genome::Ptr &ctrl_gen) :
    _morph_genome(morph_gen),_ctrl_genome(ctrl_gen)
{
//    init();
}


void Individual::update(double delta_time){
    bool use_joint_feedback = settings::getParameter<settings::Boolean>(_parameters,"#useJointFeedback").value;
    bool use_wheel_feedback = settings::getParameter<settings::Boolean>(_parameters,"#useWheelFeedback").value;
    // double input_noise_lvl = settings::getParameter<settings::Double>(parameters,"#inputNoiseLevel").value;
    // double output_noise_lvl = settings::getParameter<settings::Double>(parameters,"#outputNoiseLevel").value;
    double ctrl_freq = settings::getParameter<settings::Double>(_parameters,"#ctrlUpdateFrequency").value;
    double max_velo = settings::getParameter<settings::Double>(_parameters,"#maxVelocity").value;

    if( fabs(_sum_ctrl_freq - ctrl_freq) < 0.0001){
        act_obs_sample aos;
        _sum_ctrl_freq = 0;
        // - Retrieve sensors, joints and wheels values
        std::vector<double> inputs = _morphology->update();

        if(use_joint_feedback){
            std::vector<double> joints = _morphology->get_joints_positions();
            for(double &j: joints)
                j = 2.*j/M_PI;
            inputs.insert(inputs.end(),joints.begin(),joints.end());
        }
        if(use_wheel_feedback){
            std::vector<double> wheels = _morphology->get_wheels_velocities();
            for(double &w: wheels)
                w = w/max_velo;
            inputs.insert(inputs.end(),wheels.begin(),wheels.end());
        }
        for(double& i: inputs)
            i = misc::round_at_precision(i,2);

        aos.observation = inputs;


        std::vector<double> outputs = _control->update(inputs);
        for(double& o: outputs)
            o = misc::round_at_precision(o,2);
        aos.next_action = outputs;

        _morphology->command(outputs);
        _rollout.push_back(aos);
    }
    _sum_ctrl_freq += settings::getParameter<settings::Float>(_parameters,"#timeStep").value;
}

void Individual::crossover(const Individual::Ptr &partner, Individual *child){
    _morph_genome->crossover(partner->get_morph_genome(),child->get_morph_genome());
}

void Individual::symmetrical_crossover(const Individual::Ptr &partner, Individual *child1, Individual *child2){
    _morph_genome->symmetrical_crossover(partner->get_morph_genome(),child1->get_morph_genome(),child2->get_morph_genome());
}

std::string Individual::to_string() const
{
    return "";
    // std::stringstream sstream;
    // boost::archive::text_oarchive oarch(sstream);
    // oarch << *this;
    // return sstream.str();
}

void Individual::from_string(const std::string &str){
    // std::cout << "Individual" <<std::endl;
    // std::stringstream sstream;
    // sstream << str;
    // boost::archive::text_iarchive iarch(sstream);
    // iarch >> *this;

    // //set the parameters and randNum of the genome because their are not included in the serialisation
    // _ctrl_genome->set_parameters(_parameters);
    // _ctrl_genome->set_randNum(_rand_num);
    // _morph_genome->set_parameters(_parameters);
    // _morph_genome->set_randNum(_rand_num);
}

std::string act_obs_sample::to_string() const{
    std::stringstream sstr;
    sstr << observation[0];
    for(size_t i = 1; i < observation.size(); i++)
        sstr << "," << observation[i];
    sstr << ";" << next_action[0];
    for(size_t i = 1; i < next_action.size(); i++)
        sstr << "," << next_action[i];
    return sstr.str();
}
