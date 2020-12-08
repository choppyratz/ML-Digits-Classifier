#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "NeuralNetwork.h"
#include "nn.h"
#include <windows.h>

using namespace std;

double normalizePixelValue(double pixel) {
    return pixel / 255;
}

std::vector<std::vector<double>> explode(std::string const& s, char delim)
{
    std::vector<std::vector<double>> result;
    std::istringstream iss(s);
    int i = 0;
    for (std::string token; std::getline(iss, token, delim); )
    {
        if (i == 0) {
            result.push_back({
                (double)atoi(
                    token.c_str()
                )
                });
        }
        else {
            result.push_back({
            normalizePixelValue(
                atoi(
                    token.c_str()
                )
            )
                });
        }
        i++;
    }
    return result;
}

int main()
{
    std::ifstream in("mnist_train.csv");
    std::string line;
    std::vector<std::vector<double>> inputLayerValue;
    NeuralNetwork* nn = new NeuralNetwork();
    nn->NNInit(vector<int> {784, 128, 10});
    //vector<double> ans = { 0, 1, 1, 0 };
    //vector<vector<vector<double>>> dataset = {
    //    {{0},{0}},
    //    {{0},{1}},
    //    {{1},{0}},
    //    {{1},{1}}
    //};
    //int a = 10000;
    //while (a-->0)
    //for (int u = 0; u < 4; u++) {
    //    std::vector<std::vector<double>> fLayerVector;
    //    for (int i = 0; i < 2; i++) {
    //        if (i == ans[u]) {
    //            fLayerVector.push_back({ 1 });
    //        }
    //        else {
    //            fLayerVector.push_back({ 0 });
    //        }
    //    }
    //    std::vector<std::vector<double>> result = nn->NNTrain(dataset[u], fLayerVector);
    //    if (a == 1) {
    //        printf("%.1f %.1f\n", dataset[u][0][0], dataset[u][1][0]);
    //        for (int i = 0; i < 2; i++) {
    //            printf("%.9f\n", result[i][0]);
    //        }
    //    }
    //}

    std::ofstream ofs("nn");
    if (in.is_open())
    {
        std::vector<std::vector<double>> result;
        int a = 0;
        int b = 15;
        while (getline(in, line)) {
            inputLayerValue = explode(line, ',');
            for (int i = 1; i < inputLayerValue.size(); i++) {
                if (inputLayerValue[i][0] > 0) inputLayerValue[i][0] = 1.0;
            }
            int num = (int)inputLayerValue[0][0];
            std::vector<std::vector<double>> fLayerVector;
            for (int j = 0; j < 10; j++) {
                if (j == num) {
                    fLayerVector.push_back({ 1 });
                }
                else {
                    fLayerVector.push_back({ 0 });
                }
            }
            inputLayerValue.erase(inputLayerValue.begin());
            result = nn->NNTrain(inputLayerValue, fLayerVector);
            
            if (b == 0) {
                break;
            }

            if (a == 2000) {
                printf("%d\n", num);
                for (int k = 0; k < 10; k++) {
                    printf("%.9f\n", result[k][0]);
                }
                b--;
                a = 0;
                in.clear();
                in.seekg(0);
            }
            a++;
        }

        {
            printf("ok1\n");
            boost::archive::text_oarchive oa(ofs);
            printf("ok2\n");
            oa << nn;
            printf("ok3\n");
        }
    }
    return 0;
}