#ifndef NEURON_HPP
#define NEURON_HPP

#include <functional>
#include <numeric>
#include <vector>

class Neuron {
   public:
    static int neuronIdCounter;
    int neuronId;
    std::vector<double> weightsSums;
    double output;
    double bias = 0.0;
    std::function<double(double)> activationFunction;

    Neuron(std::function<double(double)> activationFunction);
    void active();
    void clear();
};

#endif  // NEURON_HPP
