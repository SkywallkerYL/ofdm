/*************************************************************************
	> File Name: DelayCorr.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月24日 星期五 09时53分08秒
 ************************************************************************/
#ifndef DELAYCORR
#define DELAYCORR
#include<iostream>
#include "common.h"
#include "myfixed.h"
#include "LeaderSequence.h"
//#include "matplotlibcpp.h"
#include <vector>
using namespace std;

//接收端 进行分组检测
/***
 * 根据输入计算延时相关性
 * 分别计算C(i)和P(i)
 * 
 * 
*/
vector<complex<DataType>> delay_corr(const vector<complex<DataType>> DataA,const vector<complex<DataType>> DataB,int flag = 1) {
	int N = DataA.size();
	//进行延迟相关计算	
	vector<complex<DataType>> corr(N);
	for (int i = 0; i < N; i++) {
		corr[i] = DataA[i] * conj(DataB[i]);
	}
	//对相关性进行累加
	vector<complex<DataType>> corr_sum(N);
	complex<DataType> sum(0,0);
	for (int i = 0; i < N; i++) {
		
		//前16个数据 
		if(i < Short_Train_Symbols){
			sum = sum + corr[i];
			corr_sum[i] = sum;
		}else{
			//后边的数据 sum是当前数据减去16个数据之前的数据 即始终是16个数据的和
			sum = sum + corr[i] - corr[i-Short_Train_Symbols];
			//打印计算的每一项
			//cout << i << "\t" << corr[i] << "\t" << corr[i-16] << "\t" << sum << endl;
			corr_sum[i] = sum;
		}
	}
	return corr_sum;
	//这一部还是再外边做
	////幅值简化计算 只算实部和虚部的绝对值之和
	//vector<DataType> corr_sum_abs(N);
	//if(flag == 1){
	//	for (int i = 0; i < N; i++) {
	//		corr_sum_abs[i] = abs(corr_sum[i].real()) + abs(corr_sum[i].imag());
	//	}
	//}else {
	//	//做自相关的时候只算实部的绝对值 因为虚部是0
	//	for (int i = 0; i < N; i++) {
	//		corr_sum_abs[i] = (corr_sum[i].real());
	//	}
	//}
	//return corr_sum_abs;
}
/*****
 * 计算延时相关性 
 * m = C(i)/P(i)
 * 
 * 确定数据的起始点
 * 和数据的结束点
 * 
 * 当连续32个数据的m值大于0.5时认为是数据的起始点
 * 
 * 当连续48个数据的窗口能量小于阈值时认为是数据的结束点
 * 
 * 返回确定的数据的起始点和结束点后的数据
*/
vector<complex<DataType>> delay_corr_Group_Detection(const vector<complex<DataType>>& ofdm_symbol) {
	int N = ofdm_symbol.size();
	//延迟短训练序列周期后的数据
	vector<complex<DataType>> data_shift(N+Short_Train_Symbols);
	for (int i = 0; i < N; i++) {
		data_shift[i+Short_Train_Symbols] = ofdm_symbol[i];
		if(i < Short_Train_Symbols){
			complex<DataType> zero(0,0);
			data_shift[i] = zero;
		}
	}
	//对原始数据后边加上一个周期的数据 补零
	vector<complex<DataType>> data_comp(N+Short_Train_Symbols);
	for (int i = 0; i < N; i++) {
		data_comp[i] = ofdm_symbol[i];
	}
	for (int i = 0; i < Short_Train_Symbols; i++) {
		complex<DataType> zero(0,0);
		data_comp[N+i] = zero;
	}
	//延迟相关计算

	vector<complex<DataType>> C = delay_corr(data_shift,data_comp,1);
	vector<complex<DataType>> P = delay_corr(data_comp,data_comp,0);
	//计算绝对值
	vector<DataType> C_abs(N);
	vector<DataType> P_abs(N);
	for (int i = 0; i < N; i++) {
		C_abs[i] = abs(C[i].real()) + abs(C[i].imag());
		P_abs[i] = abs(P[i].real());
	}
	//计算m 
	vector<DataType> m(N);
	for (int i = 0; i < N; i++) {
		if (P_abs[i] == 0) {
			m[i] = 0;
		}else {
			m[i] = C_abs[i]/P_abs[i];
		}
	}
	//打印m
	//for (int i = 0; i < N; i++) {
	//	cout <<i << "\t"<<m[i] << "\t" << C_abs[i] << "\t" << P_abs[i] << endl;
	//}
	//确定数据的起始点
	int start = 0;
	//连续32个数据的m值大于0.5时认为是数据的起始点
	for (int i = 0; i < N; i++) {
		bool flag = 0 ;
		if(m[i] > Energy_Ratio_Threshold){
			for (int j = i; j < i+32; j++) {
				if(m[j] < Energy_Ratio_Threshold){
					break;
				}
				if(j == i+31){
					start = i;
					flag = 1;
				}
			}
		}
		if(flag == 1){
			break;
		}
	}
	// 往回取16个 因为C[i]的数据是延迟了16个周期的 要补回去
	start = start - 16;
	//确定数据的结束点
	int end = 0;
	//连续48个数据的窗口能量小于阈值时认为是数据的结束点 
	//结束点的确定是从起始点开始的
	for (int i = start; i < N; i++) {
		bool flag = 0;
		for (int j = i; j < i+48; j++) {
			if (P_abs[j] > Energy_Detect_Threshold){
				break;
			}
			if (j == i+47){
				flag = 1;
				end = i;
			}
		}
		if (flag == 1) {
			break;
		}
	}
	//打印数据的起始点和结束点
	cout << "start:" << start << "\t" << "end:" << end << endl;
	//返回确定的数据的起始点和结束点后的数据
	//数据的长度 为end-start
	vector<complex<DataType>> data(end-start);
	for (int i = 0; i < end-start ; i++) {
		data[i] = ofdm_symbol[i+start];
	}
	return data;
}

/**
 * 一个测试，看看是否可以正确的计算出数据的起始点和结束点	
 * 
 *
 * 
*/
void delay_corr_test() {
	//生成训练序列
	vector<complex<DataType>> Leader_Sequence = generateLeaderSequence();
	//为方便观察，给前导数据补0
	vector<complex<DataType>> ofdm_symbol(Leader_Sequence.size()+200);
	//前100个数据是0
	for (int i = 0; i < 100; i++) {
		complex<DataType> zero(0,0);
		ofdm_symbol[i] = zero;
	}
	//前导数据
	for (int i = 0; i < Leader_Sequence.size(); i++) {
		ofdm_symbol[i+100] = Leader_Sequence[i];
	}
	//后边的数据是0
	for (int i = 0; i < 100; i++) {
		complex<DataType> zero(0,0);
		ofdm_symbol[i+100+Leader_Sequence.size()] = zero;
	}
	//返回确定的数据的起始点和结束点后的数据
	vector<complex<DataType>> data = delay_corr_Group_Detection(ofdm_symbol);
	//画图 以索引为横坐标 数据为纵坐标
	//for (int i = 0; i < data.size(); i++) {
	//	cout << i << "\t" << data[i] << endl;
	//}

	
}

#endif