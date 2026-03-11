#pragma once

#include "apear/ea.hpp"


namespace apear {

/**
 * @brief The Asynchronous Morpho Evolution algorithm.
 */
template<class ind_t>
class AsyncMorphoEvolution : public apear::EA<ind_t>
{
    using IndPtr = std::shared_ptr<ind_t>;
    using Ptr = std::unique_ptr<AsyncMorphoEvolution>;
    using ConstPtr = std::unique_ptr<const AsyncMorphoEvolution>;

    AsyncMorphoEvolution() : apear::EA<ind_t>(){}
    AsyncMorphoEvolution(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) :
        apear::EA<ind_t>(rn,param)
    {}

    void init() override{
        //TODO
    }

    bool update() override{
        //TODO
        return false;
    }
    bool is_finish() override{
        //TODO
        return false;
    }

private:
    std::vector<IndPtr> _parent_pool;

    void _reproduction(){
        int pop_size = settings::getParameter<settings::Integer>(this->_parameters,"#populationSize").value;
        int tournament_size = settings::getParameter<settings::Integer>(this->__parameters,"#tournamentSize").value;

        while(this->_eval_queue.size() < pop_size){
            //Random selection of indexes without duplicate
            std::vector<int> random_indexes;
            random_indexes.push_back(this->_rand_num->randInt(0,_parent_pool.size()-1));
            do{
                int rand_idx = this->_rand_num->randInt(0,_parent_pool.size()-1);
                bool already_drawn = false;
                for(const int& idx: random_indexes)
                    if(idx == rand_idx){
                        already_drawn = true;
                        break;
                    }
                if(!already_drawn)
                    random_indexes.push_back(rand_idx);
            }while(random_indexes.size() < tournament_size);
            //-

            //create a new morpholigical genome
            std::vector<genome_t> gene_subset;
            for(const int &idx: random_indexes)
                gene_subset.push_back(parent_pool[idx]);
            Genome::Ptr new_morph_gene = best_of_subset(gene_subset);
            new_morph_gene->set_id(highest_morph_id++);
            new_morph_gene->set_parameters(parameters);
            new_morph_gene->set_randNum(randomNum);

            //Add it to the population
            EmptyGenome::Ptr ctrl_genome = std::make_shared<EmptyGenome>();
            MEIMIndividual::Ptr ind = std::make_shared<MEIMIndividual>(new_morph_gene,ctrl_genome);
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);
            if(use_fixed_control)
                std::dynamic_pointer_cast<MEIMIndividual>(ind)->set_weight_bias(weight,bias);
            std::vector<double> init_pos;
            init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
            std::dynamic_pointer_cast<MEIMIndividual>(ind)->set_init_position(init_pos);
            population.push_back(ind);
            //-
        }
    }

};

}
