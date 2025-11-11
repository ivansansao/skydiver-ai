#include "NeuralNetwork.hpp"

#include <ctime>
#include <iostream>
#include <random>
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
        for (unsigned int j = 0; j < layers[0].neurons.size(); ++j) {
            sub.push_back(getRand());
        }
        weightsLayer0.push_back(sub);
    }
    weights.push_back(weightsLayer0);

    // Hidden layers
    for (unsigned int lay = 1; lay < layers.size(); ++lay) {
        std::vector<std::vector<double>> weightsLayer;

        weightsLayer.reserve(layers[lay - 1].neurons.size());
        for (unsigned int i = 0; i < layers[lay - 1].neurons.size(); ++i) {
            std::vector<double> sub;
            sub.reserve(layers[lay].neurons.size());
            for (unsigned int j = 0; j < layers[lay].neurons.size(); ++j) {
                sub.push_back(getRand());
            }
            weightsLayer.push_back(sub);
        }
        weights.push_back(weightsLayer);
    }
}

std::vector<double> NeuralNetwork::think(const std::vector<double>& input) {
    clearNeurons();

    this->input = input;

    // First layer
    for (unsigned int i = 0; i < weights[0].size(); ++i) {
        // Scan weights x input
        for (unsigned int j = 0; j < weights[0][i].size(); ++j) {
            const double mult = input[i] * weights[0][i][j];
            layers[0].neurons[j].weightsSums.push_back(mult);
        }
    }

    // Activate the neurons
    for (unsigned int i = 0; i < layers[0].neurons.size(); ++i) {
        layers[0].neurons[i].active();
    }

    // Hidden layers
    for (unsigned int l = 1; l < layers.size(); ++l) {
        for (unsigned int i = 0; i < weights[l].size(); ++i) {
            // Scan weights x input
            for (unsigned int j = 0; j < weights[l][i].size(); ++j) {
                const double mult = layers[l - 1].neurons[i].output * weights[l][i][j];
                layers[l].neurons[j].weightsSums.push_back(mult);
            }
        }

        // Activate the neurons
        for (unsigned int i = 0; i < layers[l].neurons.size(); ++i) {
            layers[l].neurons[i].active();
        }
    }

    std::vector<double> result;
    result.reserve(layers.back().neurons.size());
    for (const auto& neuron : layers.back().neurons) {
        result.push_back(neuron.output);
    }

    return result;
}

void NeuralNetwork::mutate(int many, bool tryAll) {
    if (tryAll) {
        for (unsigned int w = 0; w < weights.size(); ++w) {
            for (unsigned int row = 0; row < weights[w].size(); ++row) {
                for (unsigned int column = 0; column < weights[w][row].size(); ++column) {
                    if (getRand() > 0.4) {
                        weights[w][row][column] += getRand();
                        mutated++;
                        mutatedNeurons += " N" + std::to_string(w) + "/" + std::to_string(row) + "/" + std::to_string(column);
                    }
                }
            }
        }

        // Mutate bias
        if (getRand() > 0) {
            for (unsigned int l = 0; l < layers.size(); l++) {
                for (unsigned int n = 0; n < layers[l].neurons.size(); n++) {
                    if (getRand() > 0.5) {
                        layers[l].neurons[n].bias += getRand();
                        mutated++;
                        mutatedNeurons += " B" + std::to_string(l) + "/" + std::to_string(n) + ";";
                    }
                }
            }
        }

    } else {
        for (int i = 0; i < many; ++i) {
            const int w = rand() % (weights.size());
            const int row = rand() % (weights[w].size());
            const int column = rand() % (weights[w][row].size());
            weights[w][row][column] += getRand();

            mutated++;
            mutatedNeurons += "N" + std::to_string(w) + "/" + std::to_string(row) + "/" + std::to_string(column);
        }

        // Mutate bias
        const int l = rand() % layers.size();
        const int n = rand() % layers[l].neurons.size();
        layers[l].neurons[n].bias += getRand();

        mutated++;
        mutatedNeurons += "B" + std::to_string(l) + "/" + std::to_string(n) + ";";
    }
}

void NeuralNetwork::clearNeurons() {
    for (auto& layer : layers) {
        for (auto& neuron : layer.neurons) {
            neuron.clear();
        }
    }
}

void NeuralNetwork::setWeights(const std::string& text) {
    std::istringstream iss(text);
    std::vector<double> imported;

    std::string token;

    while (std::getline(iss, token, ',')) {  // Usa vírgula como delimitador
        try {
            double value = std::stod(token);  // Converte a string para double
            imported.push_back(value);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid number: " << token << std::endl;
        } catch (const std::out_of_range& e) {
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
        for (unsigned int j = 0; j < layers[0].neurons.size(); ++j) {
            sub.push_back(imported[w]);
            w++;
        }
        weightsLayer0.push_back(sub);
    }
    weights.push_back(weightsLayer0);

    // Hidden layers
    for (unsigned int lay = 1; lay < layers.size(); ++lay) {
        std::vector<std::vector<double>> weightsLayer;
        weightsLayer.reserve(layers[lay - 1].neurons.size());
        for (unsigned int i = 0; i < layers[lay - 1].neurons.size(); ++i) {
            std::vector<double> sub;
            sub.reserve(layers[lay].neurons.size());
            for (unsigned int j = 0; j < layers[lay].neurons.size(); ++j) {
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
    static std::random_device rd;                     // Obtém uma semente aleatória do hardware
    static std::mt19937 gen(rd());                    // Inicializa o gerador Mersenne Twister com a semente
    std::uniform_real_distribution<> dis(-1.0, 1.0);  // Define a distribuição entre -1 e 1
    // Gera o número aleatório entre -1 e 1
    return dis(gen);
}

std::string NeuralNetwork::getWeights() const {
    std::ostringstream weightsString;

    for (const auto& layerWeights : weights) {
        for (const auto& row : layerWeights) {
            for (const auto& value : row) {
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

void NeuralNetwork::printWeightsNoWrap() const {
    std::cout << "NN:";
    for (size_t layerIndex = 0; layerIndex < weights.size(); ++layerIndex) {
        std::cout << "Layer " << layerIndex << ": ";

        for (size_t rowIndex = 0; rowIndex < weights[layerIndex].size(); ++rowIndex) {
            for (size_t columnIndex = 0; columnIndex < weights[layerIndex][rowIndex].size(); ++columnIndex) {
                std::cout << "  W" << layerIndex << "/" << rowIndex << "/" << columnIndex << ": "
                          << weights[layerIndex][rowIndex][columnIndex];
            }
        }
    }
    std::cout << std::endl;
}

void NeuralNetwork::setBias(const std::string& text) {
    if (text.empty()) return;

    std::istringstream iss(text);
    std::vector<double> imported;

    std::string token;
    while (std::getline(iss, token, ',')) {
        try {
            imported.push_back(std::stod(token));
        } catch (...) {
            std::cerr << "Invalid number: " << token << std::endl;
        }
    }

    int index = 0;

    for (unsigned int l = 0; l < layers.size(); ++l) {
        for (unsigned int i = 0; i < layers[l].neurons.size(); ++i) {
            layers[l].neurons[i].bias = imported[index];
            index++;
        }
    }
}

std::string NeuralNetwork::getBias() const {
    std::ostringstream oss;

    for (const auto& layer : layers) {
        for (const auto& neuron : layer.neurons) {
            if (oss.tellp() > 0) oss << ',';
            oss << neuron.bias;
        }
    }

    return oss.str();
}

void NeuralNetwork::draw(sf::RenderWindow* window, uint16_t left, uint16_t top) {
    if (input.size() < 1) {
        return;
    }
    // Configurações de layout
    float neuronRadius = 20.0f;         // Raio dos neurônios
    const float layerSpacing = 150.0f;  // Espaçamento entre as camadas
    const float neuronSpacing = 6.0f;   // Espaçamento entre os neurônios na mesma camada
    const int inputNamesLength = 130;

    // Variáveis de posição inicial
    float x = left + inputNamesLength;
    float y;

    uint16_t l = 0;
    uint16_t n = 0;

    // Lista para armazenar as posições dos neurônios de cada camada
    std::vector<std::vector<sf::Vector2f>> layersPos;

    /**
     * Save positions of neurons
     */

    // INPUT LAYER

    std::vector<sf::Vector2f> layerMinMax;

    // Calcula a posição inicial para centralizar os neurônios da camada
    y = top + (window->getSize().y - (inputs * (2 * neuronRadius + neuronSpacing) - neuronSpacing)) / 2.0f;

    // Neurons from input layer

    std::vector<sf::Vector2f> neuronsPos;

    layerMinMax.emplace_back(0, 0);

    for (size_t neuronIndex = 0; neuronIndex < inputs; ++neuronIndex) {
        neuronsPos.emplace_back(x, y);
        y += 2 * neuronRadius + neuronSpacing;

        if (neuronIndex < input.size()) {
            if (input[neuronIndex] < layerMinMax[0].x) layerMinMax[0].x = input[neuronIndex];
            if (input[neuronIndex] > layerMinMax[0].y) layerMinMax[0].y = input[neuronIndex];
        }
    }

    layersPos.push_back(neuronsPos);

    // Incrementa a posição horizontal para a próxima camada
    x += layerSpacing;

    // Get min (x), max (y) outputs
    l = 0;
    n = 0;
    for (const auto& layer : layers) {
        layerMinMax.emplace_back(0, 0);
        l++;
        for (const auto& neu : layer.neurons) {
            if (l < layerMinMax.size()) {
                if (neu.output < layerMinMax[l].x) layerMinMax[l].x = neu.output;
                if (neu.output > layerMinMax[l].y) layerMinMax[l].y = neu.output;
            }
            n++;
        }
    }

    // HIDDEN LAYERS

    for (size_t layerIndex = 0; layerIndex < layers.size(); ++layerIndex) {
        const auto& layer = layers[layerIndex];
        size_t numNeurons = layer.neurons.size();

        // Calcula a posição inicial para centralizar os neurônios da camada
        y = top + (window->getSize().y - (numNeurons * (2 * neuronRadius + neuronSpacing) - neuronSpacing)) / 2.0f;

        // Neurons
        std::vector<sf::Vector2f> neuronsPos;

        for (size_t neuronIndex = 0; neuronIndex < numNeurons; ++neuronIndex) {
            neuronsPos.emplace_back(x, y);
            y += 2 * neuronRadius + neuronSpacing;
        }

        layersPos.push_back(neuronsPos);

        // Incrementa a posição horizontal para a próxima camada
        x += layerSpacing;
    }

    /**
     * Draw lines
     */

    l = 1;
    n = 0;

    // Scan layers (e.g. 3 layers)
    double out1;
    double out2;

    for (size_t layerIndex = 0; layerIndex < layersPos.size() - 1; layerIndex++) {
        const auto& currentLayer = layersPos[layerIndex];
        const auto& nextLayer = layersPos[layerIndex + 1];

        // Scan neurons of current layer
        for (size_t i = 0; i < currentLayer.size(); ++i) {
            const auto& currentNeuronPos = currentLayer[i];
            // Scan neurons of next layer
            for (size_t j = 0; j < nextLayer.size(); ++j) {
                const auto& nextNeuronPos = nextLayer[j];

                if (layerIndex == 0) {
                    out1 = input[i];
                    out2 = layers[layerIndex].neurons[j].output;

                } else {
                    out1 = layers[layerIndex - 1].neurons[i].output;
                    out2 = layers[layerIndex].neurons[j].output;
                }

                const float map1 = Tools::map(out1, layerMinMax[layerIndex].x, layerMinMax[layerIndex].y, 0, 3600);
                const float map2 = Tools::map(out2, layerMinMax[layerIndex + 1].x, layerMinMax[layerIndex + 1].y, 0, 3600);

                sf::Color c1 = Tools::hslaToRgba(map1, 0.70, 0.57, 1);
                sf::Color c2 = Tools::hslaToRgba(map2, 0.70, 0.57, 1);

                sf::Vertex line[] = {
                    sf::Vertex(currentNeuronPos, c1),
                    sf::Vertex(nextNeuronPos, c2)};
                window->draw(line, 2, sf::Lines);
            }
        }
    }

    /**
     * Draw neurons (circles)
     */

    l = 0;  // 0 is input layer on layersPos
    n = 0;

    sf::Font font;
    if (!font.loadFromFile("./src/asset/fonts/SpaceMono-Regular.ttf")) {
        // Handle error
    }
    sf::Text biasText;
    biasText.setFont(font);
    biasText.setCharacterSize(9);
    biasText.setFillColor(sf::Color::White);

    double biggerOutputValue;
    sf::Vector2f biggerOutputPos;

    // TEST

    l = 0;

    for (const auto& point : layerMinMax) {
        biasText.setFillColor(sf::Color::Black);
        biasText.setString("Min: " + std::to_string(point.x));
        biasText.setPosition(left + (inputNamesLength * 0.5) + (layerSpacing * l), 20);
        window->draw(biasText);

        biasText.setString("Max: " + std::to_string(point.y));
        biasText.setPosition(left + (inputNamesLength * 0.5) + (layerSpacing * l), 40);
        window->draw(biasText);
        l++;
    }

    // ENDTEST

    l = 0;

    for (const auto& layer : layersPos) {
        n = 0;
        biggerOutputValue = layers[0].neurons[0].output;
        biggerOutputPos = sf::Vector2f(0, 0);

        for (const auto& neuronPos : layer) {
            biasText.setCharacterSize(9);
            sf::CircleShape neuron(neuronRadius);
            neuron.setFillColor(Tools::hslaToRgba(0, 1.0, 0.0, 360.0));

            neuron.setOutlineColor(sf::Color::White);
            neuron.setOutlineThickness(2);
            neuron.setPosition(neuronPos.x - neuronRadius, neuronPos.y - neuronRadius);
            window->draw(neuron);

            // Desenhar o valor do bias

            biasText.setFillColor(sf::Color::White);
            biasText.setString(std::to_string(0.0));
            if (l > 0) {
                const double input = layers[l - 1].neurons[n].output;
                biasText.setString(Tools::formatDouble(input, 2));
                if (input > biggerOutputValue) {
                    biggerOutputValue = input;
                    biggerOutputPos = neuronPos;
                }

            } else {
                if (n < this->input.size()) {
                    biasText.setString(Tools::formatDouble(this->input[n], 2));
                }
            }
            biasText.setPosition(neuronPos.x - neuronRadius / 1.3, neuronPos.y - neuronRadius / 2.5);
            window->draw(biasText);

            if (l > 0) {
                const std::string bias = std::to_string(layers[l - 1].neurons[n].bias);
                biasText.setString(sf::String(bias.substr(0, bias.find('.') + 3)));
            } else {
                biasText.setString("-");
            }
            biasText.setPosition(neuronPos.x - neuronRadius / 1.4, neuronPos.y + neuronRadius / 4);
            window->draw(biasText);

            biasText.setCharacterSize(14);
            if (l == 0) {
                biasText.setFillColor(sf::Color::Black);
                biasText.setString(inputNames[n]);
                biasText.setPosition(neuronPos.x - inputNamesLength - neuronRadius, neuronPos.y - neuronRadius / 2.5);
                window->draw(biasText);
            } else if (l == layersPos.size() - 1) {
                biasText.setFillColor(sf::Color::Black);
                biasText.setString(outputNames[n]);
                biasText.setPosition(neuronPos.x + neuronRadius * 2, neuronPos.y - neuronRadius / 2.5);
                window->draw(biasText);
            }

            n++;
        }

        if (l > 0) {
            sf::CircleShape neuron(neuronRadius);
            neuron.setFillColor(sf::Color::Transparent);
            neuron.setOutlineColor(sf::Color::Red);
            neuron.setOutlineThickness(4);
            neuron.setPosition(biggerOutputPos.x - neuronRadius, biggerOutputPos.y - neuronRadius);
            window->draw(neuron);
        }

        l++;
    }
}