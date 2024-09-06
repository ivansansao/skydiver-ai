#include "NeuralNetwork.hpp"

#include <ctime>
#include <iostream>
#include <sstream>

// int Neuron::neuronIdCounter = 1;

NeuralNetwork::NeuralNetwork(int inputs) : inputs(inputs),
                                           mutated(0),
                                           mutatedNeurons("") {}

void NeuralNetwork::addLayer(int size, std::function<double(double)> activationFunction) {
    layers.emplace_back(size, activationFunction);
}

void NeuralNetwork::compile() {
    // Inputs x 1s layer
    std::vector<std::vector<double>> weightsLayer0;
    weightsLayer0.reserve(inputs);
    for (int i = 0; i < inputs; ++i) {
        std::vector<double> sub;
        sub.reserve(layers[0].neurons.size());
        for (uint j = 0; j < layers[0].neurons.size(); ++j) {
            sub.push_back(getRand());
        }
        weightsLayer0.push_back(sub);
    }
    weights.push_back(weightsLayer0);

    // Hidden layers
    for (uint lay = 1; lay < layers.size(); ++lay) {
        std::vector<std::vector<double>> weightsLayer;
        weightsLayer.reserve(layers[lay - 1].neurons.size());
        for (uint i = 0; i < layers[lay - 1].neurons.size(); ++i) {
            std::vector<double> sub;
            sub.reserve(layers[lay].neurons.size());
            for (uint j = 0; j < layers[lay].neurons.size(); ++j) {
                sub.push_back(getRand());
            }
            weightsLayer.push_back(sub);
        }
        weights.push_back(weightsLayer);
    }
}

std::vector<double> NeuralNetwork::think(const std::vector<double> &input) {
    clearNeurons();

    // First layer
    for (uint i = 0; i < weights[0].size(); ++i) {
        // Scan weights x input
        for (uint j = 0; j < weights[0][i].size(); ++j) {
            const double mult = input[i] * weights[0][i][j];
            layers[0].neurons[j].weightsSums.push_back(mult);
        }
    }

    // Activate the neurons
    for (uint i = 0; i < layers[0].neurons.size(); ++i) {
        layers[0].neurons[i].active();
    }

    // Hidden layers
    for (uint l = 1; l < layers.size(); ++l) {
        for (uint i = 0; i < weights[l].size(); ++i) {
            // Scan weights x input
            for (uint j = 0; j < weights[l][i].size(); ++j) {
                const double mult = layers[l - 1].neurons[i].output * weights[l][i][j];
                layers[l].neurons[j].weightsSums.push_back(mult);
            }
        }

        // Activate the neurons
        for (uint i = 0; i < layers[l].neurons.size(); ++i) {
            layers[l].neurons[i].active();
        }
    }

    std::vector<double> result;
    result.reserve(layers.back().neurons.size());
    for (const auto &neuron : layers.back().neurons) {
        result.push_back(neuron.output);
    }

    return result;
}

void NeuralNetwork::mutate(int many) {
    for (int i = 0; i < many; ++i) {
        const int w = rand() % (weights.size());
        const int row = rand() % (weights[w].size());
        const int column = rand() % (weights[w][row].size());
        weights[w][row][column] += getRand();

        mutated++;
        mutatedNeurons += "N" + std::to_string(w) + "/" + std::to_string(row) + "/" + std::to_string(column);
    }
}

void NeuralNetwork::clearNeurons() {
    for (auto &layer : layers) {
        for (auto &neuron : layer.neurons) {
            neuron.clear();
        }
    }
}

void NeuralNetwork::setWeights(const std::string &text) {
    std::istringstream iss(text);
    std::vector<double> imported;

    std::string token;

    while (std::getline(iss, token, ',')) {  // Usa vírgula como delimitador
        try {
            double value = std::stod(token);  // Converte a string para double
            imported.push_back(value);
        } catch (const std::invalid_argument &e) {
            std::cerr << "Invalid number: " << token << std::endl;
        } catch (const std::out_of_range &e) {
            std::cerr << "Number out of range: " << token << std::endl;
        }
    }

    if (imported.size() <= 0) {
        return;
    }

    weights.clear();
    int w = 0;

    // Inputs x 1s layer
    std::vector<std::vector<double>> weightsLayer0;
    weightsLayer0.reserve(inputs);
    for (int i = 0; i < inputs; ++i) {
        std::vector<double> sub;
        sub.reserve(layers[0].neurons.size());
        for (uint j = 0; j < layers[0].neurons.size(); ++j) {
            sub.push_back(imported[w++]);
        }
        weightsLayer0.push_back(sub);
    }
    weights.push_back(weightsLayer0);

    // Hidden layers
    for (uint lay = 1; lay < layers.size(); ++lay) {
        std::vector<std::vector<double>> weightsLayer;
        weightsLayer.reserve(layers[lay - 1].neurons.size());
        for (uint i = 0; i < layers[lay - 1].neurons.size(); ++i) {
            std::vector<double> sub;
            sub.reserve(layers[lay].neurons.size());
            for (uint j = 0; j < layers[lay].neurons.size(); ++j) {
                sub.push_back(imported[w++]);
            }
            weightsLayer.push_back(sub);
        }
        weights.push_back(weightsLayer);
    }
}

std::string NeuralNetwork::version() const {
    return "1.0.0.4";
}

double NeuralNetwork::getRand() {
    // Inicializa a semente com o tempo atual
    static bool initialized = false;
    if (!initialized) {
        srand(static_cast<unsigned>(time(nullptr)));
        initialized = true;
    }

    // Retorna um valor pseudoaleatório entre -1 e 1
    return static_cast<double>(rand()) / RAND_MAX - static_cast<double>(rand()) / RAND_MAX;
}

std::string NeuralNetwork::getWeights() const {
    std::ostringstream weightsString;

    for (const auto &layerWeights : weights) {
        for (const auto &row : layerWeights) {
            for (const auto &value : row) {
                weightsString << value << ",";
            }
        }
    }

    // Remove a última vírgula se estiver presente
    std::string result = weightsString.str();
    if (!result.empty() && result.back() == ',') {
        result.pop_back();
    }

    return result;
}

void NeuralNetwork::printWeights() const {
    for (size_t layerIndex = 0; layerIndex < weights.size(); ++layerIndex) {
        std::cout << "Layer " << layerIndex << " Weights:" << std::endl;

        for (size_t rowIndex = 0; rowIndex < weights[layerIndex].size(); ++rowIndex) {
            for (size_t columnIndex = 0; columnIndex < weights[layerIndex][rowIndex].size(); ++columnIndex) {
                std::cout << "  W" << layerIndex << "/" << rowIndex << "/" << columnIndex << ": "
                          << weights[layerIndex][rowIndex][columnIndex] << std::endl;
            }
        }
    }
}