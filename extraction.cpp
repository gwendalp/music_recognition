#include <iostream>
#include "au_reading.h"
#include "signal.h"
#include <string>
#include <vector>
#include "csvfile.h"

using namespace std;

int main() {
    
    std::vector<string> class_name={ "blues" , "classical" , "country" , "disco" , "hiphop" , "jazz" , "metal" , "pop" , "reggae" , "rock"};
    std::vector<string> class_number={ "0" , "1" , "2" , "3" , "4" , "5" , "6" , "7" , "8" , "9"};
    
    /* Opening file */
    std::ostringstream oss_w;
    oss_w << "../data/feature.csv" ;
    std::string path_write = oss_w.str();
    csvfile csv(path_write);


     for (int k = 0; k < class_name.size(); k++){
        
        cout << k << endl; 
        for (int i = 0; i < 100; i++){
            csv<<k;
            std::ostringstream oss_r;
           if (i<10)
                oss_r << "../archive/genres/"<<class_name[k]<<"/"<<class_name[k]<<".0000" << i << ".au" ;
            else
                oss_r << "../archive/genres/"<<class_name[k]<<"/"<<class_name[k]<<".000" << i << ".au" ;
        
            std::string path_read = oss_r.str();
            DataVector data = readAuFile(path_read);
            auto bins = stft(data);
            auto ms = binsavgstd(bins);
            for (int i = 0; i < ms.size(); i++)
            {
                
                csv << ms[i];
            }

            csv << endrow;
            

            




        }
        
    
        }
    
    
    
    
    
    
    /* auto data = readAuFile("../disco.00050.au");
    auto bins = stft(data);
    auto ms = binsavgstd(bins);
    std::cout << ms.size() << std::endl;
    */



}