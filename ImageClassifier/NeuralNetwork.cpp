#include "NeuralNetwork.h"
//#define DEBUG
#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <math.h>
using namespace std;

vector<vector<double>> NeuralNetwork::activationFunction(vector<vector<double>> matrix) {
    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < matrix[0].size(); j++) {
            matrix[i][j] = 1.0 / (1.0 + exp(-matrix[i][j]));
        }
    }
    return matrix;
}

double SigmoidDerivative(double x) {
    return (1 / (1 + exp(-x))) * (1 - (1 / (1 + exp(-x))));
}

vector<vector<double>> NeuralNetwork::tryRecognizeNumber(std::vector<std::vector<double>> fLayer) {
    NeuralNetwork::NNLayers[0] = fLayer;
    NeuralNetwork::NNLayers[0].push_back({ 1.0 });
    NeuralNetwork::NNLayersNotActivated[0] = fLayer;
    NeuralNetwork::NNLayersNotActivated[0].push_back({ 1.0 });

    for (int i = 0; i < NeuralNetwork::NNWeights.size(); i++) {
        NeuralNetwork::NNLayers[i + 1] = NeuralNetwork::activationFunction(NeuralNetwork::matrixMultiplication(NNWeights[i], NNLayers[i]));
        NeuralNetwork::NNLayersNotActivated[i + 1] = NeuralNetwork::matrixMultiplication(NNWeights[i], NNLayers[i]);
        if (i + 2 != NeuralNetwork::NNLayers.size()) {
            NeuralNetwork::NNLayers[i + 1].push_back({ 1.0 });
            NeuralNetwork::NNLayersNotActivated[i + 1].push_back({ 1.0 });
        }
    }
    return NeuralNetwork::NNLayers.back();
}

vector<vector<double>> NeuralNetwork::NNTrain(vector<vector<double>> inputLayer, vector<vector<double>> expectedResult) {
    vector<vector<double>> result = NeuralNetwork::tryRecognizeNumber(inputLayer);
    NNErrors[NNErrors.size() - 1] = getOutErrors(result, expectedResult);
    for (int i = NNLayers.size() - 2; i >= 1; i--) {
        for (int j = 0; j < NNLayers[i].size() - 1; j++) {
            double  elem = 0.0;
            for (int k = 0; k < NNLayers[i + 1].size() - 1; k++) { // 2
                elem += NNWeights[i][k][j] * NNErrors[i + 1][k][0];
            }
            NNErrors[i][j][0] = elem;
        }
    }

    for (int i = 0; i < NNLayers.size() - 1; i++) {
        for (int j = 0; j < NNLayers[i + 1].size(); j++) {
            for (int k = 0; k < NNLayers[i].size(); k++) {
                if (j < NNErrors[i + 1].size())
                    NNWeights[i][j][k] += NNErrors[i + 1][j][0] * SigmoidDerivative(NNLayersNotActivated[i + 1][j][0]) * NNLayers[i][k][0] * learningRate;
            }
        }
    }
    return result;
}

void NeuralNetwork::NNInit(vector<int> layersInfo) {
    NeuralNetwork::NNConfig = layersInfo;
    for (int i = 0; i < layersInfo.size(); i++) {
        vector<vector<double>> initColumn = vector<vector<double>>(layersInfo[i], { 0.0 });
        NNLayers.push_back(initColumn);
        NNErrors.push_back(initColumn);
    }

    for (int i = 0; i < layersInfo.size() - 1; i++) {
        NNLayers[i].push_back({ {1.0} });
    }

    for (int i = 0; i < layersInfo.size() - 1; i++) {
        NeuralNetwork::NNWeights.push_back(NeuralNetwork::initWeights(layersInfo[i] + 1, layersInfo[i + 1]));
    }

    NNLayersNotActivated = NNLayers;
}

vector<vector<double>> NeuralNetwork::initWeights(int firstLayerSize, int secondLayerSize) {
    srand((unsigned int)time(NULL));
    vector<vector<double>> weights = vector<vector<double>>();
    for (int i = 0; i < secondLayerSize; i++) {
        weights.push_back(vector<double>());
        for (int j = 0; j < firstLayerSize; j++) {
            double f = (double)rand() / RAND_MAX;
            double weightValue = -0.5 + f * (0.5 - -(0.5));
            weights[i].push_back(weightValue);
        }
    }
    return weights;
}

vector<vector<double>> NeuralNetwork::matrixMultiplication(vector<vector<double>> firstMatrix, vector<vector<double>> secondMatrix) {
    vector<vector<double>> result = vector<vector<double>>();
    double tempElem = 0.0;
    for (int i = 0; i < firstMatrix.size(); i++) {
        result.push_back(std::vector<double>());
        for (int j = 0; j < secondMatrix[0].size(); j++) {
            tempElem = 0.0;
            for (int k = 0; k < secondMatrix.size(); k++) {
                tempElem += firstMatrix[i][k] * secondMatrix[k][j];
            }
            result[i].push_back(tempElem);
        }
    }
    return result;
}

vector<vector<double>> NeuralNetwork::transposeMatrix(vector<vector<double>> matrix) {
    vector<vector<double>> result = vector<vector<double>>(matrix[0].size());
    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < matrix[0].size(); j++) {
            result[j].push_back(matrix[i][j]);
        }
    }
    return result;
}

vector<vector<double>> NeuralNetwork::multiplicateMatrixOnNumber(vector<vector<double>> matrix, double number) {
    vector<vector<double>> result = vector<vector<double>>(matrix.size());
    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < matrix[0].size(); j++) {
            result[i].push_back(matrix[i][j] * number);
        }
    }
    return result;
}

vector<vector<double>> NeuralNetwork::getErrors(vector<vector<double>> vec, vector<vector<double>> out) {
    vector<vector<double>> result = vector<vector<double>>(vec.size());
    for (int i = 0; i < vec.size(); i++) {
        for (int j = 0; j < vec[0].size(); j++) {
            result[i].push_back(vec[i][j] * out[i][j] * (1 - out[i][j]));
        }
    }
    return result;
}

vector<vector<double>> NeuralNetwork::getOutErrors(vector<vector<double>> output, vector<vector<double>> expected) {
    vector<vector<double>> result = vector<vector<double>>(output.size());
    for (int i = 0; i < output.size(); i++) {
        for (int j = 0; j < output[0].size(); j++) {
            result[i].push_back((expected[i][j] - output[i][j]));
        }
    }
    return result;
}