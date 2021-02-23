#include <iostream>
#include "rapidcsv.h"
#include <eigen3/Eigen/Dense>
#include <vector>
#include "au_reading.h"
#include "signal.h"


using namespace std;

void max_value(Eigen::VectorXd X, int& res)
{
    res = 0;
    for (int i = 0; i < 10; i++)
    {
        if (X[i] > X[res])
        {
            res = i;
            
        }
        
    }
    
}


int main() {

    //rapidcsv::Document doc("../data/test.csv", rapidcsv::SeparatorParams(';'));
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

    string PATH = "../hiphop.00004.au";
    DataVector data = readAuFile(PATH);
    auto bins = stft(data);
    auto ms = binsavgstd(bins);

    for (int i = 0; i < 512; i++)
    {
        X(i, 0) = ms[i];
    }


    Eigen::VectorXd res;
    res = w_mat * X + bias_mat;

    int argmax;

    max_value(res, argmax);
    
    cout << argmax << endl;



    


    
    
     
    // cout << "bias"  << bias_mat << endl;
    //cout << "w "  << w.GetCell<float>(511, 9) << endl;

     
     






}