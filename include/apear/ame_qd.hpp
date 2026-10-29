#pragma once

#include "apear/ea.hpp"


namespace apear {

/**
 * @brief The Asynchronous Morpho Evolution algorithm.
 */
template<class ind_t, class morph_gen_t, class archive_t>
class AsyncMorphoEvolutionQD : public apear::EA<ind_t>
{
public:
    using IndPtr = std::shared_ptr<ind_t>;
    using GenPtr = std::shared_ptr<morph_gen_t>;
    using Ptr = std::unique_ptr<AsyncMorphoEvolutionQD>;
    using ConstPtr = std::unique_ptr<const AsyncMorphoEvolutionQD>;

    AsyncMorphoEvolutionQD() : apear::EA<ind_t>(){}
    AsyncMorphoEvolutionQD(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) :
        apear::EA<ind_t>(rn,param)
    {}

    void init() override{
        int pop_size = settings::getParameter<settings::Integer>(this->_parameters,"#populationSize").value;
        this->_eval_queue.resize(pop_size);
        for(IndPtr& ind: this->_eval_queue){
            ind = std::make_shared<ind_t>(this->_rand_num,this->_parameters);
            ind->init();
            ind->random();
            ind->set_id(this->_highest_id++);
        }
    }

    template<typename... Targs>
    void init_archive(Targs... params){
        _parent_pool = archive_t(params...);
    }

    bool update() override{
        for(IndPtr& ind: this->_evaluated){
            if(ind->get_objectives()[0] == 0){
                ind = std::make_shared<ind_t>(this->_rand_num,this->_parameters);
                ind->init();
                ind->random();
                ind->set_id(this->_highest_id++);
                this->_eval_queue.push_back(ind);
            }else{
                _nbr_eval++;
                _parent_pool.add_solution(ind);
            }
        }
        this->evaluated().clear();

        _reproduction();
        return true;
    }
    bool is_finish() override{
        if(_nbr_eval >= settings::getParameter<settings::Integer>(this->_parameters,"#maxNbrEval").value)
            return true;
        return false;
    }

    const archive_t &get_parent_pool() const{return _parent_pool;}

private:
    archive_t _parent_pool;
    int _nbr_eval = 0;

    void _reproduction(){
        if(_parent_pool.size() == 0)
            return;
        int pop_size = settings::getParameter<settings::Integer>(this->_parameters,"#populationSize").value;

        while(this->_eval_queue.size() < pop_size){
            IndPtr parent = _parent_pool.select_solution(this->_rand_num);
            GenPtr new_gene = std::dynamic_pointer_cast<morph_gen_t>(parent->get_morph_genome()->clone());
            new_gene->set_id(this->_highest_id++);
            new_gene->mutate();
            new_gene->set_parents_ids({parent->get_morph_genome()->id(),-1});


            //Add it to the population
            EmptyGenome::Ptr ctrl_genome = std::make_shared<EmptyGenome>();
            IndPtr ind = std::make_shared<ind_t>(new_gene,ctrl_genome);
            ind->set_parameters(this->_parameters);
            ind->set_rand_num(this->_rand_num);
            this->_eval_queue.push_back(ind);
            //-
        }
    }
};

}
