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
	52个子载波标号为-26,-25,...,-1,1,...,25,26
	4个导频位置分别为-21,-7,7,21
	
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





#endif
