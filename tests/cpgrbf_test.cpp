#include "apear/nn.hpp"
#include <cmath>
#include <random>

using namespace apear::nn;

/* Test of the CPGRBF network from the paper
 * Generic Neural Locomotion Control Framework for Legged Robots, M. Thor et al.
 * Should output several modulated oscilatory patterns.
 * The weights of the ouput layer are chosen randomly.
*/

int main(int argc, char** argv){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> uniform(-1,1);
    std::normal_distribution<> normal(0,0.1);

    std::vector<double> weights(40*1);
    std::vector<double> biases(1);
    for(int i = 0; i < 40*1; i++)
        weights[i] = uniform(gen);
    for(int i = 0; i < 1; i++)
        biases[i] = uniform(gen);


    CPGRBFNetwork cpgrbf(1,40,1,1);
    cpgrbf.init_cpg(1.01,0.03);
    cpgrbf.init_rbf();
    cpgrbf.set_out_layer_parameters(weights,biases);

    torch::Tensor x = torch::zeros(1);
    for(int i = 0; i < 1000; i++){
        x = cpgrbf.forward(x);
        std::cout << cpgrbf.get_cpg()->neuron_states().data_ptr<double>()[0] << ","
                  << cpgrbf.get_cpg()->neuron_states().data_ptr<double>()[1] << ",";

        if(i < 500)
            x = x + normal(gen);
        std::cout << x.data_ptr<double>()[0] << "," << std::endl;
                // << x.data_ptr<double>()[1] << ","
                // << x.data_ptr<double>()[2] << ","
                // << x.data_ptr<double>()[3] << "," << std::endl;
    }

    return 0;
}
