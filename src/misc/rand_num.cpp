#include "apear/misc/rand_num.hpp"
#include <iostream>
#include <cmath>

using namespace apear::misc;

RandNum::RandNum(int seed)
{
    gen.seed(seed);
    std::cout << "seed set to " << seed << std::endl;
    m_seed = seed;
}


RandNum::~RandNum(){}

double RandNum::rand_double(double lower, double upper) {
    std::uniform_real_distribution<> dist(lower,upper);
    return dist(gen);
}

float RandNum::rand_float(float lower, float upper) {
    std::uniform_real_distribution<> dist(lower,upper);
    return dist(gen);
}

int RandNum::rand_int(int lower, int upper) {
    std::uniform_int_distribution<> dist(lower,upper);
    return dist(gen);
}

std::vector<double> RandNum::rand_vectd(double lower, double upper, int size){
    std::vector<double> res;
    res.reserve(size);
for(int i = 0; i < size; i++)
        res.push_back(rand_double(lower,upper));
    return res;
}

double RandNum::normal_dist(double mu, double sigma){
    std::normal_distribution<> nd(mu,sigma);
    return nd(gen);
}


void RandNum::set_seed(int seed) {
    gen.seed(seed);
	m_seed = seed;
	std::cout << "Seed set to " << seed << std::endl;
}

int RandNum::get_seed() const
{
        return m_seed;
}


