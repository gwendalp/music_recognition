#include <iostream>
#include "rapidcsv.h"
#include <eigen3/Eigen/Dense>
#include <vector>
#include "au_reading.h"
#include "signal.h"


using namespace std;

void max_value(Eigen::VectorXd X, int& max)
{
    max = 0;
    for (int i = 0; i < 10; i++)
    {
        if (X[i] > X[max])
        {
            max = i;
            
        }
        
    }
    
}

void prediction(string path, int& res)
{
    rapidcsv::Document bias("../data/bias.csv", rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));
    rapidcsv::Document w("../data/coef.csv", rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));
    
    Eigen::MatrixXd bias_mat = Eigen::MatrixXd::Ones(10, 1);
    Eigen::MatrixXd w_mat = Eigen::MatrixXd::Ones(10, 512);
    Eigen::MatrixXd X = Eigen::MatrixXd::Ones(512, 1);


    for (int i = 0; i < 10; i++)
    {
        bias_mat(i, 0) = bias.GetCell<float>(1, i);
    } 

   for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            w_mat(i ,j) = w.GetCell<float>(j, i);
        }
        
    } 

    
    DataVector data = readAuFile(path);
    auto bins = stft(data);
    auto ms = binsavgstd(bins);

    for (int i = 0; i < 512; i++)
    {
        X(i, 0) = ms[i];
    }

    Eigen::VectorXd pred;
    pred = w_mat * X + bias_mat;

    int argmax;

    max_value(pred, argmax);
    
    res = argmax;
}

int main(int argc, char* argv[]) {
    std::vector<string> class_name={ "blues" , "classical" , "country" , "disco" , "hiphop" , "jazz" , "metal" , "pop" , "reggae" , "rock"};
    string path = argv[1];
    int res; 
    prediction(path, res);
    cout << class_name[res];
}