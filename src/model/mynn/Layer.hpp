#ifndef LAYER_HPP
#define LAYER_HPP

#include "Neuron.hpp"
#include <vector>

class Layer
{
public:
    std::vector<Neuron> neurons;

    Layer(int size, std::function<double(double)> activationFunction);
};

#endif // LAYER_HPP
