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
	//接受的数据在fft之后，顺序已经调整回来，这里不需要再移位
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
	//求个平均
	vector<complex<DataType>> receivedLongTrainSequence;
	DataType two = 2;	
	receivedLongTrainSequence.resize(LongTrainSequence.size());
	for (int i = 0; i < LongTrainSequence.size(); i++) {
		receivedLongTrainSequence[i] = (receivedLongTrainSequence1[i] + receivedLongTrainSequence2[i])/two;
	}
	//进行信道估计 
	//将接受的长训练序列和本地已知的长训练序列进行相关性计算 计算H = R_LTS‘ * L_LTS

	vector<complex<DataType>> H;
	H.resize(LongTrainSequence.size());
	for (int i = 0; i < LongTrainSequence.size(); i++) {
		H[i] = conj(receivedLongTrainSequence[i]) * (LongTrainSequence[i]);
	}

	//能量计算 E = |R_LTS|^2
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
	equalizedData.resize(receivedData.size()-32-64-64);
	for (int i = 0; i < receivedData.size(); i++) {
		equalizedData[i] = receivedData[i] * H[i];
	}
	return equalizedData;
}




#endif
