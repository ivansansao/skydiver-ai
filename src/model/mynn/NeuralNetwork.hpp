#ifndef NEURALNETWORK_HPP
#define NEURALNETWORK_HPP

#include "Layer.hpp"
#include <vector>
#include <string>

class NeuralNetwork
{
public:
    int inputs;
    std::vector<Layer> layers;
    std::vector<std::vector<std::vector<double>>> weights;
    int mutated;
    std::string mutatedNeurons;

    NeuralNetwork(int inputs);
    void addLayer(int size, std::function<double(double)> activationFunction);
    void compile();
    std::vector<double> think(const std::vector<double> &input);
    void mutate(int many);
    void clearNeurons();
    void setWeights(const std::string &text);
    std::string version() const;
    std::string getWeights() const;
    void printWeights() const;

private:
    double getRand();
};

#endif // NEURALNETWORK_HPP
