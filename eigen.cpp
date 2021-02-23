#include <iostream>
#include <eigen3/Eigen/Dense>
 
using namespace Eigen;
using namespace std;
 
int main()
{

  
  MatrixXd m = MatrixXd::Ones(4, 4);
  MatrixXd x = MatrixXd::Ones(4, 1);
  VectorXd t = VectorXd::Zero(4, 1);
  
  VectorXd res;
  res = m*x + x;

    cout << "res =" << endl << m[1, 1] << endl;
}
