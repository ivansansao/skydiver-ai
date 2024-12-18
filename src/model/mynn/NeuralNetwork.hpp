#ifndef NEURALNETWORK_HPP
#define NEURALNETWORK_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "Layer.hpp"

class NeuralNetwork {
   public:
    uint16_t inputs;
    std::vector<Layer> layers;
    std::vector<std::vector<std::vector<double>>> weights;
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
    void draw(sf::RenderWindow *w, uint16_t left, uint16_t top);

   private:
    double getRand();
};

#endif  // NEURALNETWORK_HPP
