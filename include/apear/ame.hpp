#pragma once

#include "apear/ea.hpp"


namespace apear {

/**
 * @brief The Asynchronous Morpho Evolution algorithm.
 */
template<class ind_t, class morph_gen_t>
class AsyncMorphoEvolution : public apear::EA<ind_t>
{
public:
    using IndPtr = std::shared_ptr<ind_t>;
    using GenPtr = std::shared_ptr<morph_gen_t>;
    using Ptr = std::unique_ptr<AsyncMorphoEvolution>;
    using ConstPtr = std::unique_ptr<const AsyncMorphoEvolution>;

    AsyncMorphoEvolution() : apear::EA<ind_t>(){}
    AsyncMorphoEvolution(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) :
        apear::EA<ind_t>(rn,param)
    {}

    void init() override{
        int pop_size = settings::getParameter<settings::Integer>(this->_parameters,"#populationSize").value;
        this->_eval_queue.resize(pop_size);
        for(IndPtr& ind: this->_eval_queue){
            ind = std::make_shared<ind_t>(this->_rand_num,this->_parameters);
            ind->init();
            ind->set_id(this->_highest_id++);
        }
    }

    bool update() override{
        int pop_size = settings::getParameter<settings::Integer>(this->_parameters,"#populationSize").value;
        for(IndPtr& ind: this->_evaluated){
            _nbr_eval++;
            _parent_pool.push_back(std::move(ind));
            if(_parent_pool.size() > pop_size)
                _remove_worst_parent();
        }
        this->evaluated().clear();

        if(_parent_pool.size() >= pop_size)
            _reproduction();
        return true;
    }
    bool is_finish() override{
        if(_nbr_eval >= settings::getParameter<settings::Integer>(this->_parameters,"#maxNbrEval").value)
            return true;
        return false;
    }

private:
    std::vector<IndPtr> _parent_pool;
    int _nbr_eval = 0;

    void _reproduction(){
        int pop_size = settings::getParameter<settings::Integer>(this->_parameters,"#populationSize").value;
        int tournament_size = settings::getParameter<settings::Integer>(this->_parameters,"#tournamentSize").value;

        while(this->_eval_queue.size() < pop_size){
            //Random selection of indexes without duplicate
            std::vector<int> random_indexes;
            random_indexes.push_back(this->_rand_num->rand_int(0,_parent_pool.size()-1));
            do{
                int rand_idx = this->_rand_num->rand_int(0,_parent_pool.size()-1);
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
            std::vector<IndPtr> gene_subset;
            for(const int &idx: random_indexes)
                gene_subset.push_back(_parent_pool[idx]);
            GenPtr new_morph_gene = _best_of_subset(gene_subset);


            //Add it to the population
            EmptyGenome::Ptr ctrl_genome = std::make_shared<EmptyGenome>();
            IndPtr ind = std::make_shared<ind_t>(new_morph_gene,ctrl_genome);
            ind->set_parameters(this->_parameters);
            ind->set_rand_num(this->_rand_num);
            this->_eval_queue.push_back(ind);
            //-
        }
    }

    GenPtr _best_of_subset(const std::vector<IndPtr> &parents){

        double best_fitness = parents[0]->get_objectives()[0];
        int best_idx = 0;
        for(size_t i = 1; i < parents.size(); i++){
            if(best_fitness < parents[i]->get_objectives()[0]){
                best_fitness = parents[i]->get_objectives()[0];
                best_idx = i;
            }
        }
        GenPtr new_gene = std::dynamic_pointer_cast<morph_gen_t>(parents[best_idx]->get_morph_genome()->clone());
        new_gene->set_id(this->_highest_id++);
        new_gene->mutate();
        new_gene->set_parents_ids({parents[best_idx]->get_morph_genome()->id(),-1});
        return new_gene;
    }

    void _remove_worst_parent(){
        int pop_size = settings::getParameter<settings::Integer>(this->_parameters,"#populationSize").value;
        if(_parent_pool.size() <= pop_size)
            return;

        //- find lowest fitness value in the parent pool
        double lowest_obj = _parent_pool[0]->get_objectives()[0];
        size_t worst_parent_idx = 0;
        for(size_t i = 1; i < _parent_pool.size(); i++){
            if(lowest_obj > _parent_pool[i]->get_objectives()[0]){
                lowest_obj = _parent_pool[i]->get_objectives()[0];
                worst_parent_idx = i;
            }
        }
        //-

        //- then erase the worst one
        _parent_pool.erase(_parent_pool.begin() + worst_parent_idx);
        //-
    }
};

}
