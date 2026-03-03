#pragma once

#include <memory>
#include <random>

namespace apear {

namespace misc {

class RandNum
{
public:
    typedef std::shared_ptr<RandNum> Ptr;
    typedef std::shared_ptr<const RandNum> ConstPtr;

    RandNum(int seed); // instantiate the class and specify the initial seed.
    RandNum(const RandNum &rn): m_seed(rn.m_seed), gen(rn.gen){}
    ~RandNum();

    /**
     * @brief random number between a lower bound and a upper bound included
     * @param lower bound (double)
     * @param upper bound (double)
     * @return a double
     */
    double rand_double(double lower, double upper);

    /**
     * @brief random number between a lower bound and a upper bound included
     * @param lower bound (float)
     * @param upper bound (float)
     * @return a float
     */
    float rand_float(float lower, float upper);

    /**
     * @brief random number between a lower bound and a upper bound included
     * @param lower bound (int)
     * @param upper bound (int)
     * @return an integer
     */
    int rand_int(int lower, int upper);

    /**
     * @brief Generate a random vector of double
     * @param lower bound
     * @param upper bound (included
     * @param size of the vector
     * @return
     */
    std::vector<double> rand_vectd(double lower, double upper, int size);

    /**
     * @brief Generate a random number from a normal distribution
     * @param mean of the distribution
     * @param variance of the distribution
     * @return
     */
    double normal_dist(double mu, double sigma);

    void set_seed(int seed); // sets the seed of the random number generator
    int m_seed = 0;
    int get_seed() const;
    std::mt19937 gen;
};

}//misc

}//apear

