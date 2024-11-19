#ifndef NEURALNETWORK_HPP
#define NEURALNETWORK_HPP

#include <string>
#include <vector>

#include "Layer.hpp"

class NeuralNetwork {
   public:
    int inputs;
    std::vector<Layer> layers;
    std::vector<std::vector<std::vector<double>>> weights;
    std::vector<std::vector<double>> biases;
    int mutated = 0;
    std::string mutatedNeurons;

    NeuralNetwork(int inputs);
    void addLayer(int size, std::function<double(double)> activationFunction);
    void compile();
    std::vector<double> think(const std::vector<double> &input);
    void mutate(int many, bool tryAll = false);
    void clearNeurons();
    std::string version() const;
    void setWeights(const std::string &text);
    std::string getWeights() const;
    void printWeights() const;
    void printWeightsNoWrap() const;

    void setBias(const std::string &text);
    std::string getBias() const;

   private:
    double getRand();
};

#endif  // NEURALNETWORK_HPP
