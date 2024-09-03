#ifndef NEURON_HPP
#define NEURON_HPP

#include <vector>
#include <functional>
#include <numeric>

class Neuron
{
public:
    static int neuronIdCounter;
    int neuronId;
    std::vector<double> weightsSums;
    double output;
    std::function<double(double)> activationFunction;

    Neuron(std::function<double(double)> activationFunction);
    void active();
    void clear();
};

#endif // NEURON_HPP
