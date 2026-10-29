#pragma once
#include <functional>
#include <apear/misc/rand_num.hpp>

namespace apear{

namespace selectors{

template<typename solution_t>
using selector_t = std::function<const solution_t &(const std::vector<solution_t>&, const misc::RandNum::Ptr &)>;

template<typename solution_t>
inline selector_t<solution_t> random = [](const std::vector<solution_t>& solutions,const misc::RandNum::Ptr & rand_num) -> const solution_t &{
    int index = rand_num->rand_int(0, static_cast<int>(solutions.size()) - 1);
    return solutions.at(index);
};
}


}//apear
