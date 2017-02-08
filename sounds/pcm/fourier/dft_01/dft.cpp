#include <cmath>
#include <complex>
#include <iostream>
#include <vector>

// 離散フーリエ変換

// 普通に実数部と虚数部を別のvectorで処理するパターン
void dft0(std::vector<double>& input,
          std::vector<double>& real,
          std::vector<double>& imaginary ){
    const auto& N = input.size();
    for(size_t j = 0; j < N; j++){
        double re_sum = 0;
        double im_sum = 0;
        
        for(size_t i = 0; i < N; i++){
            double tht = 2 * M_PI / N * j  * i;
            re_sum += input[i] * cos(tht);
            im_sum += input[i] * sin(tht);
        }
        real.push_back(re_sum);
        imaginary.push_back(im_sum);
    }
}

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
    
    
int main(){
    std::vector<double> input{2.0, 4.0, 8.0, 16.0};
    std::vector<double> real, imag;
    std::vector< std::complex<double> > answer;

    std::cout << "*** vectors(cos,sin) ***" << std::endl;    
    dft0(input, real, imag);
    for(size_t i = 0; i < input.size(); i++){
        std::cout << "(" << real[i] << "," << imag[i] << ")" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "*** std::complex ***" << std::endl;
    dft(input, answer);
    for(size_t i = 0; i < input.size(); i++){
        std::cout << "(" << answer[i].real() << "," << answer[i].imag() << ")" << std::endl;
    }
    
    return 0;
}
