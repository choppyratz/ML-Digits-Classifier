#pragma once
#include <vector>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;
class NeuralNetwork
{
public:
	NeuralNetwork() {}
	vector<vector<double>> tryRecognizeNumber(std::vector<std::vector<double>> fLayer);
	void NNInit(vector<int> layersInfo);
	vector<vector<double>> NNTrain(vector<vector<double>> inputLayer, vector<vector<double>> expectedResult);
private:
	friend class boost::serialization::access;
	double learningRate = 0.3;
	vector<int> NNConfig;
	vector<vector<vector<double>>> NNLayers;
	vector<vector<vector<double>>> NNLayersNotActivated;
	vector<vector<vector<double>>> NNWeights;
	vector<vector<vector<double>>> NNErrors;
	vector<vector<double>> activationFunction(vector<vector<double>>);
	vector<vector<double>> matrixMultiplication(vector<vector<double>> firstMatrix, vector<vector<double>> secondMatrix);
	vector<vector<double>> initWeights(int firstLayerSize, int secondLayerSize);
	vector<vector<double>> multiplicateMatrixOnNumber(vector<vector<double>> matrix, double number);
	vector<vector<double>> getOutErrors(vector<vector<double>> output, vector<vector<double>> expected);
	vector<vector<double>> getErrors(vector<vector<double>> oldWeghts, vector<vector<double>> newWeghts);
	vector<vector<double>> transposeMatrix(vector<vector<double>> matrix);
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& NNConfig;
		ar& NNLayers;
		ar& NNLayersNotActivated;
		ar& NNWeights;
		ar& NNErrors;
	}
};

