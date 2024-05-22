/*************************************************************************
	> File Name: cpandwindow.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月22日 星期三 15时17分12秒
 ************************************************************************/
#ifndef CPANDWINDOW
#define CPANDWINDOW
#include<iostream>
#include "common.h"
#include "myfixed.h"
using namespace std;
// 3. 加上循环前缀
/***
 * 循环前缀的作用是为了消除符号间的干扰
 * 将每个OFDM符号的后Tg时间中的样值复制到ofdm符号前
 * 
 * 这里就是将Num_FFT数据中后Num_CP个数据复制到前面
 */
// 4. 加窗
/****
 * 加窗的目的是为了减小频谱泄露
 * 使得带宽之外的功率谱密度下降的更快
 * 
 * 
*/
vector<complex<DataType>> add_cyclic_prefix_window(const vector<complex<DataType>>& ofdm_symbol,complex<DataType> nextFirst) {
	vector<complex<DataType>> ofdm_symbol_with_cpandwindow;
	ofdm_symbol_with_cpandwindow.resize(Num_FFT + Num_CP + 1);
	for (int i = 0; i < Num_CP; i++) {
		ofdm_symbol_with_cpandwindow[i] = ofdm_symbol[Num_FFT - Num_CP + i];
	}
	for (int i = 0; i < Num_FFT; i++) {
		ofdm_symbol_with_cpandwindow[i + Num_CP] = ofdm_symbol[i];
	}
	//加窗要求每个Ofdm符号最后多一个数据 并且仍然满足周期性 所以最后一个数据是
	//下一个Ofdm符号的第一个数据 和 这个符号的64个数据的第一个数据
	DataType two = 2;
	ofdm_symbol_with_cpandwindow[Num_FFT + Num_CP] = (nextFirst + ofdm_symbol[0])/two;
	return ofdm_symbol_with_cpandwindow;
}







#endif
