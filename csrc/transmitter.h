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
#include "LeaderSequence.h"
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
	//打印data的大小
	//int data_size = symbol.size();
	//cout << "data_length:" << data_size << endl;
	//这里假定data的长度是Num_data_subcarriers*Num_data_in_OFDM_Symbol = Num_data_in_OFDM
	//即symbol的长度是Num_data_subcarriers
	//48个一组 对数据插入导频
	vector<complex<DataType>> ofdm_symbol;
	//ofdm 符号的个数
	int Num_ofdm_symbol = ofdm_length/Num_data_subcarriers;
	ofdm_symbol.resize(Num_OFDM*Num_ofdm_symbol);
	//每48个一组插入导频 并且进行ifft变换
	for (int i = 0; i < Num_ofdm_symbol; i++) {
		vector<complex<DataType>> temp_symbol;
		temp_symbol.resize(Num_data_subcarriers);
		for (int j = 0; j < Num_data_subcarriers; j++) {
			temp_symbol[j] = symbol[i*Num_data_subcarriers+j];
		}
		//对数据插入导频
		vector<complex<DataType>> temp_ofdm_symbol = insert_pilot(temp_symbol);
		//打印一下发送的ofdm数据
		for (int i = 0; i < temp_ofdm_symbol.size(); i++) {
			cout << "ofdm_symbol:" << temp_ofdm_symbol[i] << endl;
		}
		//数据经过ifft变换
		vector<complex<DataType>> temp_ofdm_time_symbol = fft_dif(temp_ofdm_symbol,0);
		//加循环前缀和加窗
		vector<complex<DataType>> temp_ofdm_symbol_with_cpandwindow = add_cyclic_prefix_window(temp_ofdm_time_symbol,temp_ofdm_time_symbol[0]);
		for (int j = 0; j < Num_OFDM; j++) {
			ofdm_symbol[i*Num_OFDM+j] = temp_ofdm_symbol_with_cpandwindow[j];
		}
	}
	
	//int ofdm = ofdm_symbol.size();
	//cout << "ofdm_length:" << ofdm << endl;
	//在数据前面加上训练序列
	vector<complex<DataType>> Leader_Sequence = generateLeaderSequence();
	vector<complex<DataType>> ofdm_symbol_with_leader;
	ofdm_symbol_with_leader.resize(Leader_Sequence.size() + ofdm_symbol.size());
	for (int i = 0; i < Leader_Sequence.size(); i++) {
		ofdm_symbol_with_leader[i] = Leader_Sequence[i];
	}
	for (int i = 0; i < ofdm_symbol.size(); i++) {
		ofdm_symbol_with_leader[i+Leader_Sequence.size()] = ofdm_symbol[i];
	}
	//返回ofdm符号
	return ofdm_symbol_with_leader;
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
