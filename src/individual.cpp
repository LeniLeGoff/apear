#include "apear/individual.hpp"

using namespace apear;

Individual::Individual(const Genome::Ptr &morph_gen,const Genome::Ptr &ctrl_gen) :
    _morph_genome(morph_gen),_ctrl_genome(ctrl_gen)
{
//    init();
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

