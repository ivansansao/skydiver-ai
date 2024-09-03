#include "Layer.hpp"

Layer::Layer(int size, std::function<double(double)> activationFunction)
{
    neurons.reserve(size);
    for (int i = 0; i < size; ++i)
    {
        neurons.emplace_back(activationFunction);
    }
}
