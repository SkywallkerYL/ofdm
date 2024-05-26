/*************************************************************************
	> File Name: FrequencySync.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月25日 星期六 13时30分40秒
 ************************************************************************/
#ifndef FREQUENCYSYNC
#define FREQUENCYSYNC
#include "common.h"
#include "myfixed.h"
#include "fft.h"
#include "CarrierSync.h"
#include<iostream>
using namespace std;
/****
 * ADC 和 DAC 之间不可能具有完全相同的时钟周期和相为
 * 会导致FFT周期偏差
 * 
 * 这里使用FFT之后进行频域矫正
 * 
 * FFT解调后，采样点的定时偏移表现为乘上一个相为因子
 * 数据产生相位旋转。
 * 
 * 这里就体现导频的作用了
 * 
 * 这里在硬件实现上也做了简化
 * 将4个导频符号的相位相加 结果右移7位
 * 传入的数据是一个ofdm 符号 64个数据
 * 注意，是已经经过fft变换的数据
 * 已经是频域数据了
 * 
*/
vector<complex<DataType>> frequency_sync(const vector<complex<DataType>>& ofdm_symbol, int & timectrl,int & timevalid) {
	//从ofdm符号中取出导频符号
	complex<DataType> pilot1 = ofdm_symbol[7];
	complex<DataType> pilot2 = ofdm_symbol[21];
	complex<DataType> pilot3 = ofdm_symbol[43];
	complex<DataType> pilot4 = ofdm_symbol[57];
	//这里计算一下标准导频 硬件上应该是直接存储的
	vector<complex<DataType>> pilotsymbol;
	pilotsymbol.resize(Num_FFT);
	for (int i = 0; i < Num_FFT; i++) {
		pilotsymbol[i] = complex<DataType>(0, 0);
	}
	pilotsymbol[7] = complex<DataType>(Pilot_Frequency, 0);
	pilotsymbol[21] = complex<DataType>(Pilot_Frequency, 0);
	pilotsymbol[43] = complex<DataType>(Pilot_Frequency, 0);
	pilotsymbol[57] = complex<DataType>(Pilot_Frequency, 0);
	//过一遍ifft 得到时域信号
	//vector<complex<DataType>> time_domain_pilot = fft_dif(pilotsymbol,0);
	//拿到标准导频信号
	complex<DataType> standard_pilot1 = pilotsymbol[7]; //time_domain_pilot[7];
	complex<DataType> standard_pilot2 = pilotsymbol[21]; //time_domain_pilot[21];
	complex<DataType> standard_pilot3 = pilotsymbol[43]; //time_domain_pilot[43];
	complex<DataType> standard_pilot4 = pilotsymbol[57];//time_domain_pilot[57];
	//计算导频相关
	complex<DataType> corr1 = pilot1 * conj(standard_pilot1);
	complex<DataType> corr2 = pilot2 * conj(standard_pilot2);
	complex<DataType> corr3 = pilot3 * conj(standard_pilot3);
	complex<DataType> corr4 = pilot4 * conj(standard_pilot4);
	//计算角度估计
	DataType angle1 = angleEstimating(corr1);
	DataType angle2 = angleEstimating(corr2);
	DataType angle3 = angleEstimating(corr3);
	DataType angle4 = angleEstimating(corr4);
	//
	DataType TwoorderSeven = 128;
	DataType TwoorderSix = 64;
	DataType angle = (angle1 + angle2 + angle3 + angle4) /TwoorderSeven;
	//
	DataType pi = PI;
	DataType two_pi = pi+pi;
	timectrl = 0;
	if(angle > two_pi/TwoorderSix || angle < -two_pi/TwoorderSix){
		//旋转超过2pi
		timevalid = 1;
		if(angle > DataType(0)){
			timectrl = 0;
		}else if(angle < DataType(0)){
			timectrl = 1;
		}
	}else {
		timevalid = 0;
	}
	// 把角度限定到-pi到pi之间
	while (angle > pi || angle < -pi) {
		if (angle > pi) {
			angle -= two_pi;
		} else if (angle < -pi) {
			angle += two_pi;
		}
	}
	//进行频率偏移补偿
	vector<complex<DataType>> data_compensation = CFOCompensation(ofdm_symbol, angle);
	return data_compensation;
}





#endif