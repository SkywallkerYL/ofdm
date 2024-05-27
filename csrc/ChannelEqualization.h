/*************************************************************************
	> File Name: ChannelEqualization.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月26日 星期日 15时36分32秒
 ************************************************************************/
#ifndef CHANNELEQUALIZATION
#define CHANNELEQUALIZATION
#include "common.h"
#include "myfixed.h"
#include "fft.h"
#include "pilot.h"
#include<iostream>
using namespace std;

/***
 * 通过长训练序列进行信道估计和均衡
 * 
 * 信道估计的原理是将接收到的长训练序列和本地已知的长训练序列进行相关性计算
 *
 * 这里传入的数据是长训练序列和数据部分
 * 返回均衡后的数据 
*/
vector<complex<DataType>> channelEqualization(const vector<complex<DataType>> Data, vector<complex<DataType>> &ERlts) {
	//接收到的数据
	vector<complex<DataType>> receivedData = Data;
	//本地已知的长训练序列
	vector<complex<DataType>> LongTrainSequence = LongTrainSeq;
	//接受的数据在fft之后，顺序已经调整回来，这里不需要再移位 还是在这里移位
	//提取两个长训练序列的数据
	vector<complex<DataType>> receivedLongTrainSequence1;
	receivedLongTrainSequence1.resize(LongTrainSequence.size());
	for (int i = 0; i < LongTrainSequence.size(); i++) {
		receivedLongTrainSequence1[i] = receivedData[i+32];
	}
	vector<complex<DataType>> receivedLongTrainSequence2;
	receivedLongTrainSequence2.resize(LongTrainSequence.size());
	for (int i = 0; i < LongTrainSequence.size(); i++) {
		receivedLongTrainSequence2[i] = receivedData[i+32+64];
	}
	//过一边fft	
	vector<complex<DataType>> receivedLongTrainSequence1_fft = fft_dif(receivedLongTrainSequence1);
	vector<complex<DataType>> receivedLongTrainSequence2_fft = fft_dif(receivedLongTrainSequence2);
	//求个平均
	vector<complex<DataType>> receivedLongTrainSequence;
	DataType two = 2;	
	receivedLongTrainSequence.resize(LongTrainSequence.size());
	for (int i = 0; i < LongTrainSequence.size(); i++) {
		receivedLongTrainSequence[i] = (receivedLongTrainSequence1_fft[i] + receivedLongTrainSequence2_fft[i])/two;
	}
	//打印一下接受到的长训练序列
	for (int i = 0; i < LongTrainSequence.size(); i++) {
		cout << "receivedLongTrainSequence[" << i << "] = " << receivedLongTrainSequence[i] << endl;
	}
	//数据移位
	vector<complex<DataType>> receivedDataShift = shift_remove_pilot(receivedLongTrainSequence);
	//进行信道估计 
	//将接受的长训练序列和本地已知的长训练序列进行相关性计算 计算H = R_LTS‘ * L_LTS

	vector<complex<DataType>> H;
	//这个返回给外边做相位跟踪，软件上这里外边的数据还没有移回来，所以这里要把H移一下
	H.resize(LongTrainSequence.size());
	for (int i = 0; i < LongTrainSequence.size(); i++) {
		H[i] = conj(receivedDataShift[i]) * (LongTrainSequence[i]);
		//打印信道估计结果
		cout << "H[" << i << "] = " << H[i] << endl;
	}
	vector<complex<DataType>> H_shift = shift_remove_pilot(H);
	

	//能量计算 E = |R_LTS|^2
	//这里还是用移位前的操作，方便外边调用
	vector<DataType> E;
	E.resize(LongTrainSequence.size());
	for (int i = 0; i < LongTrainSequence.size(); i++) {
		E[i] = (receivedLongTrainSequence[i].real())*(receivedLongTrainSequence[i].real()) + (receivedLongTrainSequence[i].imag())*(receivedLongTrainSequence[i].imag());
	}
	for (int i = 0; i < LongTrainSequence.size(); i++) {
		ERlts[i] = E[i];
	}

	//信道补偿
	vector<complex<DataType>> equalizedData;
	int size = receivedData.size()-32-64-64;
	equalizedData.resize(size);
	for (int i = 0; i < equalizedData.size()/Num_FFT; i++) {
		for (int j = 0; j < Num_FFT; j++) {
			equalizedData[i*Num_FFT+j] = receivedData[i*Num_FFT+j+Num_Long_Train_Symbols] * H_shift[j];
		}	
		//打印一下均衡前和均衡后的数据
		for (int j = 0; j < Num_FFT; j++) {
			cout << "receivedData[" << i*Num_FFT+j << "] = " << receivedData[i*Num_FFT+j+Num_Long_Train_Symbols] << endl;
			cout << "equalizedData[" << i*Num_FFT+j << "] = " << equalizedData[i*Num_FFT+j] << endl;
		}
	}
	return equalizedData;
}




#endif
