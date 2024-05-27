/*************************************************************************
	> File Name: pilot.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月22日 星期三 20时17分53秒
 ************************************************************************/
#ifndef PILOT
#define PILOT
#include<iostream>
#include "common.h"
#include "myfixed.h"

using namespace std;

// 插入导频
vector<complex<DataType>> insert_pilot(const vector<complex<DataType>>& symbol) {
	vector<complex<DataType>> ofdm_symbol;
	ofdm_symbol.resize(Num_FFT);
	// 插入导频
	/*
	//这里其实就相当于数据前后补0 然后进行一个32的循环移位
	52个子载波标号为-26,-25,...,-1,1,...,25,26
	4个导频位置分别为-21,-7,7,21
	其实就是0,1,2,....,23,  24,25,....47
	
	对于给ifft的映射
	然后1到26映射到1到26
	-26到-1映射到38到63
	所以导频位置为-21 映射到43
	-7 映射到57
	7 映射到7
	21 映射到21
	*/
	//初始化全0
	for (int i = 0; i < Num_FFT; i++) {
		ofdm_symbol[i] = complex<DataType>(0, 0);
	}

	// 导频值，这个值可以根据你的需要进行修改
    complex<DataType> pilot_value(Pilot_Frequency, 0);

    // 插入导频
    ofdm_symbol[43] = pilot_value; // -21 映射到43
    ofdm_symbol[57] = pilot_value; // -7 映射到57
    ofdm_symbol[7] = pilot_value;  // 7 映射到7
    ofdm_symbol[21] = pilot_value; // 21 映射到21

    // 插入其他子载波
    int j = 0;
    for (int i = 0; i < Num_data_subcarriers; ++i) {
        if(i <= 4){
			ofdm_symbol[i+38] = symbol[i];
		}else if (i >= 5 && i <= 17){
			ofdm_symbol[i+39] = symbol[i];
		}else if (i >= 18 && i <= 23){
			ofdm_symbol[i+40] = symbol[i];
		}else if (i >= 24 && i <= 29){
			ofdm_symbol[i-23] = symbol[i];
		}else if (i >= 30 && i <= 42){
			ofdm_symbol[i-22] = symbol[i];
		}else if (i >= 43 && i <= 47){
			ofdm_symbol[i-21] = symbol[i];
		}
    }

    return ofdm_symbol;

}

/****
 * 接受端将数据移位回来，并且去除导频
 * 
 * 
*/

vector<complex<DataType>> shift_remove_pilot(const vector<complex<DataType>>& ofdm_symbol,bool flag = 1) {
	vector<complex<DataType>> symbol;
	symbol.resize(Num_data_subcarriers);

	//首先将数据移位回来
	vector<complex<DataType>> temp_symbol = ofdm_symbol;
	for(int i = 0; i < 32; ++i) {
		temp_symbol[i] = ofdm_symbol[32 + i];
		temp_symbol[32 + i] = ofdm_symbol[i];
	}
	if (flag == 1){
		//处理的是长训练序列 数据 直接移位回来
		return temp_symbol;
	}
	//flag != 1 的时候 处理的是数据
	//去除导频
	for (int i = 0; i < Num_data_subcarriers; ++i) {
		if(i <= 4){
			symbol[i] = ofdm_symbol[i+38];
		}else if (i >= 5 && i <= 17){
			symbol[i] = ofdm_symbol[i+39];
		}else if (i >= 18 && i <= 23){
			symbol[i] = ofdm_symbol[i+40];
		}else if (i >= 24 && i <= 29){
			symbol[i] = ofdm_symbol[i-23];
		}else if (i >= 30 && i <= 42){
			symbol[i] = ofdm_symbol[i-22];
		}else if (i >= 43 && i <= 47){
			symbol[i] = ofdm_symbol[i-21];
		}
	}
	return symbol;

}


#endif
