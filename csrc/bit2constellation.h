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
		DataType real_part = 0;
    	DataType imag_part = 0;
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

    	constellation[i / 4] = std::complex<DataType>(real_part*KMod, imag_part*KMod);

	}
	
#elif Modulation_Type == 3
	// 64QAM
	vector<complex<DataType>> constellation;
	constellation.resize(bitstream.size() / 6);
#endif	
	return constellation;

}

//解调

/*****
 * 信道均衡的时候为了避免除法
 * 所以没有对幅度进行矫正
 * 这里通过乘法来进行弥补 
 * 
 * 传入的是经过均衡后的数据 和能量矫正系数
 * 返回解调后的比特流对应的软信息
 * 暂时还没有做，直接用的硬解
 * 
*/
vector<int> demodulate(const vector<complex<DataType>> &Data, const vector<DataType> ERlts) {
	vector<int> bitstream;
	int N = Data.size();
#if Modulation_Type == 0
	// BPSK
	bitstream.resize(N);
	for (int i = 0; i < N; i++) {
		if (Data[i].real() > 0) {
			bitstream[i] = 0;
		}
		else {
			bitstream[i] = 1;
		}
	}
#elif Modulation_Type == 1
	// QPSK
	bitstream.resize(N * 2);
	for (int i = 0; i < N; i++) {
		if (Data[i].real() > 0) {
			bitstream[i * 2] = 0;
		}
		else {
			bitstream[i * 2] = 1;
		}
		if (Data[i].imag() > 0) {
			bitstream[i * 2 + 1] = 0;
		}
		else {
			bitstream[i * 2 + 1] = 1;
		}
	}
#elif Modulation_Type == 2
	// 16QAM
	bitstream.resize(N * 4);
	//cout << "ERlts.size() = " << N  << endl;
	cout << 2 * KMod * ERlts[0] << endl;
	for (int i = 0; i < N; i++) {
		DataType real_part = Data[i].real();
		DataType imag_part = Data[i].imag();
		//依次判决 b0 b1 b2 b3 
		//cout <<"i:" << i << " " << real_part << " " << 2 * KMod * ERlts[i%Num_data_subcarriers] << endl;
		if(real_part > 0){
			bitstream[i * 4] = 1;
			if(real_part > 2 * KMod * ERlts[i%Num_data_subcarriers]){
		//		cout <<"i:" << i << " " << real_part << " " << 2 * KMod * ERlts[i%Num_data_subcarriers] << endl;
				bitstream[i * 4 + 1] = 0;
			}else{
				bitstream[i * 4 + 1] = 1;
			}
		}else {
			bitstream[i * 4] = 0;
			if(real_part < -2 * KMod * ERlts[i%Num_data_subcarriers]){
				bitstream[i * 4 + 1] = 0;
			}else{
				bitstream[i * 4 + 1] = 1;
			}
		}
		if(imag_part > 0){
			bitstream[i * 4 + 2] = 1;
			if(imag_part > 2 * KMod * ERlts[i%Num_data_subcarriers]){
				bitstream[i * 4 + 3] = 0;
			}else{
				bitstream[i * 4 + 3] = 1;
			}
		}else {
			bitstream[i * 4 + 2] = 0;
			if(imag_part < -2 * KMod * ERlts[i%Num_data_subcarriers]){
				bitstream[i * 4 + 3] = 0;
			}else{
				bitstream[i * 4 + 3] = 1;
			}
		}
	}
#endif
	return bitstream;
}

#endif