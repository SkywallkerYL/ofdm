/*************************************************************************
	> File Name: PhaseTracking.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月25日 星期六 16时19分39秒
 ************************************************************************/
#ifndef PHASETRACKING
#define PHASETRACKING
#include "common.h"
#include "myfixed.h"
#include "fft.h"
#include "CarrierSync.h"
#include "FrequencySync.h"

#include<iostream>
using namespace std;
/*****
 * 频偏补偿之后还有残余的相位偏移
 * 
 * 
 * 
*/
vector<complex<DataType>> PhaseTracking(const vector<complex<DataType>> ofdm_symbol) {
	int N = ofdm_symbol.size();
	vector<complex<DataType>> phaseTrackedData;
	phaseTrackedData.resize(N);
	//提取导频
	complex<DataType> pilot1 = ofdm_symbol[7];
	complex<DataType> pilot2 = ofdm_symbol[21];
	complex<DataType> pilot3 = ofdm_symbol[43];
	complex<DataType> pilot4 = ofdm_symbol[57];
	//标准导频
	complex<DataType> standard_pilot1 = complex<DataType>(Pilot_Frequency, 0);
	complex<DataType> standard_pilot2 =	complex<DataType>(Pilot_Frequency, 0);
	complex<DataType> standard_pilot3 =	complex<DataType>(Pilot_Frequency, 0);
	complex<DataType> standard_pilot4 =	complex<DataType>(Pilot_Frequency, 0);
	//计算补偿因子
	complex<DataType> compensation1 = pilot1 * conj(standard_pilot1);
	complex<DataType> compensation2 = pilot2 * conj(standard_pilot2);
	complex<DataType> compensation3 = pilot3 * conj(standard_pilot3);
	complex<DataType> compensation4 = pilot4 * conj(standard_pilot4);
	//计算补偿因子的平均值
	DataType Four = 4;
	complex<DataType> compensation = (compensation1 + compensation2 + compensation3 + compensation4) / Four;
	//打印一下补偿因子
	cout << "compensation:" << compensation << endl;
	//对数据进行相位补偿
	for (int i = 0; i < N; i++) {
		phaseTrackedData[i] = ofdm_symbol[i] * (compensation);
	}
	return phaseTrackedData;
}
#endif

