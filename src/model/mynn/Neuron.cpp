#include "Neuron.hpp"

#include "iostream"

int Neuron::neuronIdCounter = 1;

Neuron::Neuron(std::function<double(double)> activationFunction) : neuronId(neuronIdCounter++),
                                                                   output(0.0),
                                                                   activationFunction(activationFunction) {}

void Neuron::active() {
    double rsSum = std::accumulate(weightsSums.begin(), weightsSums.end(), 0.0);
    output = activationFunction(rsSum + bias);
}

void Neuron::clear() {
    weightsSums.clear();
    output = 0.0;
}
