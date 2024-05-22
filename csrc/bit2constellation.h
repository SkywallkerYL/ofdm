/*************************************************************************
	> File Name: bit2constellation.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月20日 星期一 16时00分10秒
 ************************************************************************/
#ifndef BIT2CONSTELLATION
#define BIT2CONSTELLATION
#include <iostream>
#include "common.h"
#include <complex>
#include "myfixed.h"
using namespace std;

// 2. 将比特流映射到复数星座图上
vector<complex<DataType>> map_bitstream_to_constellation(const vector<int>& bitstream) {
#if Modulation_Type == 0
	// BPSK
	vector<complex<DataType>> constellation;
	constellation.resize(bitstream.size());
	for (int i = 0; i < bitstream.size(); i++) {
		if (bitstream[i] == 0) {
			constellation[i] = complex<DataType>(1, 0);
		}
		else {
			constellation[i] = complex<DataType>(-1, 0);
		}
	}
#elif Modulation_Type == 1
	// QPSK
	vector<complex<DataType>> constellation;
	constellation.resize(bitstream.size() / 2);
	for (int i = 0; i < bitstream.size(); i += 2) {
		if (bitstream[i] == 0 && bitstream[i + 1] == 0) {
			constellation[i / 2] = complex<DataType>(1, 1);
		}
		else if (bitstream[i] == 0 && bitstream[i + 1] == 1) {
			constellation[i / 2] = complex<DataType>(1, -1);
		}
		else if (bitstream[i] == 1 && bitstream[i + 1] == 0) {
			constellation[i / 2] = complex<DataType>(-1, 1);
		}
		else {
			constellation[i / 2] = complex<DataType>(-1, -1);
		}
	}
#elif Modulation_Type == 2
	// 16QAM
	vector<complex<DataType>> constellation;
	constellation.resize(bitstream.size() / 4);
	for (int i = 0; i < bitstream.size(); i += 4) {
		int real_part = 0;
    	int imag_part = 0;
		// b0 b1 b2 b3 
    	// 映射实部
    	if (bitstream[i] == 1 && bitstream[i + 1] == 1) {
    	    real_part  = 1;
    	}else if (bitstream[i] == 1 && bitstream[i + 1] == 0) {
		    real_part = 3;
		}else if (bitstream[i] == 0 && bitstream[i + 1] == 0) {
		    real_part = -3;
		}else {
		    real_part = -1;
		}
    	// 映射虚部
    	if (bitstream[i + 2] == 1 && bitstream[i + 3] == 1) {
		    imag_part  = 1;
		}else if (bitstream[i + 2] == 1 && bitstream[i + 3] == 0) {
			imag_part = 3;
		}else if (bitstream[i + 2] == 0 && bitstream[i + 3] == 0) {
			imag_part = -3;
		}else {
			imag_part = -1;
		}

    	constellation[i / 4] = std::complex<DataType>(real_part, imag_part);

	}
	
#elif Modulation_Type == 3
	// 64QAM
	vector<complex<DataType>> constellation;
	constellation.resize(bitstream.size() / 6);
#endif	
	return constellation;

}






#endif