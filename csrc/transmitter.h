/*************************************************************************
	> File Name: transmitter.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月22日 星期三 16时28分04秒
 ************************************************************************/
#ifndef TRANSMITTER
#define TRANSMITTER
#include<iostream>
#include "common.h"
#include "myfixed.h"
#include "bit2constellation.h"
#include "pilot.h"
#include "cpandwindow.h"
#include "fft.h"
using namespace std;

/***
 * 数据的产生和编码不在这里
 * 发射机平台
 * 包括映射，ifft，循环前缀 加窗，发送
 * 可以接受外部的数据，也可以自己产生数据
*/
vector<complex<DataType>> transmitter(vector<int> data) {
	// 1. 产生基带数据 这个直接从外部读了
	// 2. 映射
	// 3. IFFT
	// 4. 加循环前缀
	// 5. 加窗
	// 6. 发送
	
	//获取data的长度
	int data_length = data.size();
	//确定ofdm符号的长度
	int ofdm_length = data_length / Num_data_in_OFDM_Symbol;
	//确定映射后的符号数据
	vector<complex<DataType>> symbol = map_bitstream_to_constellation(data);
	//这里假定data的长度是Num_data_subcarriers*Num_data_in_OFDM_Symbol = Num_data_in_OFDM
	//即symbol的长度是Num_data_subcarriers
	//对数据插入导频
	vector<complex<DataType>> ofdm_symbol = insert_pilot(symbol);
	//数据经过ifft变换
	vector<complex<DataType>> ofdm_time_symbol = fft_dif(ofdm_symbol,0);
	//加循环前缀和加窗
	vector<complex<DataType>> ofdm_symbol_with_cpandwindow = add_cyclic_prefix_window(ofdm_time_symbol,ofdm_time_symbol[0]);
	//返回ofdm符号
	return ofdm_symbol_with_cpandwindow;
}

//一个简单的测试
void transmitter_test() {
	vector<int> data;
	for (int i = 0; i < Num_data_in_OFDM; i++) {
		data.push_back(i % 2);
	}
	vector<complex<DataType>> ofdm_symbol = transmitter(data);
	for (int i = 0; i < ofdm_symbol.size(); i++) {
		cout << ofdm_symbol[i] << endl;
	}
	
}



#endif
