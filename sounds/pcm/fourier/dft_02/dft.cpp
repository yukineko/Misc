#include <cmath>
#include <complex>
#include <iostream>
#include <vector>

#include <float.h>
// 離散フーリエ変換

#define BUFFER_SIZE 512

// std::complexを使用するパターン
// std::expはネイピア数の複素数値による累乗
// オイラーの公式
void dft(std::vector<double>& input,
         std::vector< std::complex<double> >& answer){
    const auto& N = input.size();
    for(size_t j = 0; j < N; j++){
        std::complex<double> sum(0,0);
        for(size_t i = 0; i < N; i++){
            double tht = 2 * M_PI / N * j  * i;
            sum += input[i] * exp(std::complex< double >(0, -tht) );
        }
        answer.push_back(sum);
    }
}

void make_vector(short* sbuffer, std::vector<double>& input)
{
    for(size_t i = 0; i < BUFFER_SIZE; i++){
        double d = (sbuffer[i] / DBL_MAX);
        input.push_back(d);
    }
}
    
int main(int argc, char* argv[]){
    
    FILE* fp = fopen((const char*)argv[1], "rb");
    if(fp == NULL){
        std::cerr << "File not found" << std::endl;
        exit(-1);
    }
    std::cout << "*** std::complex ***" << std::endl;    
    while(1){
        short buffer[BUFFER_SIZE];
        int ret = fread(buffer, sizeof(short), BUFFER_SIZE, fp);
        if(ret <= 0){
            break;
        }
        std::vector<double> input;
        std::vector< std::complex<double> > answer;
        make_vector(buffer, input);

        dft(input, answer);
        for(size_t i = 0; i < input.size(); i++){
            std::cout << "(" << answer[i].real() << "," << answer[i].imag() << ")" << std::endl;
        }
    }
    fclose(fp);
    return 0;
}
