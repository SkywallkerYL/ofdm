/*************************************************************************
	> File Name: SymbolSync.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月24日 星期五 19时21分03秒
 ************************************************************************/
#ifndef SYMBOLSYNC
#define SYMBOLSYNC

#include "common.h"
#include "myfixed.h"
#include "DelayCorr.h"
#include "LeaderSequence.h"
#include "DelayCorr.h"
#include "CarrierSync.h"
#include<iostream>
using namespace std;


/****
 * 符号同步
 * 原理就是将载波同步之后输出的数据和本地已知的短训练序列进行相关性计算
 * 
 * 从而将分组的粗略估算进行进一步精确估算
 * 
 * 
 * 这里硬件上为了进行简化
 * 在接受端的时候会将数据量化为-1 1
 * 为了减少高斯白噪声的影响，接受前会持续累加接收到的短训练取样
 * 相当于经过16次累加后，对高斯白噪声进行了平均
 * 由于短训练序列的长度是16	
 * 所以这样累加，相当于还对信号自身进行了强化
 * 当然这是个人理解了
 * 硬件上通过移位寄存器来实现 
*/

/***
 * 量化接收到的数据 短训练序列
*/

vector<complex<DataType>> quantizeData(const vector<complex<DataType>> Data) {
	int N = Data.size();
	vector<complex<DataType>> quantizedData;
	quantizedData.resize(N);
	//大小为16的移位累加器
	vector<complex<DataType>> shiftReg(Short_Train_Symbols);
	//初始化为0
	for (int i = 0; i < Short_Train_Symbols; i++) {
		shiftReg[i] = complex<DataType>(0, 0);
	}
	//对接收到的数据先进行累加
	for (int i = 0; i < Data.size(); i++) {
		quantizedData[i] = Data[i] + shiftReg[Short_Train_Symbols - 1];
		//移位累加器左移
		for (int j = Short_Train_Symbols - 1; j > 0; j--) {
			shiftReg[j] = shiftReg[j - 1];
		}
		//最后一个数据为当前数据
		shiftReg[0] = quantizedData[i];
	}
	//返回量化后的数据
	for (int i = 0; i < N; i++) {
		quantizedData[i].real(quantizedData[i].real() > 0 ? 1 : -1);
		quantizedData[i].imag(quantizedData[i].imag() > 0 ? 1 : -1);
	}
	return quantizedData;
}
/***
 * 短训练序列的相关性计算
 * 
 * 返回累加和
*/
vector<complex<DataType>> STSCorrelatingAndAccumulate(const vector<complex<DataType>> Data){
	//提取短训练序列
	vector<complex<DataType>> shortTrainingSequence = generateLeaderSequence();
	//前16个数据为短训练序列
	vector<complex<DataType>> shortTrainingSequenceData;
	shortTrainingSequenceData.resize(Short_Train_Symbols);
	for (int i = 0; i < Short_Train_Symbols; i++) {
		shortTrainingSequenceData[i] = shortTrainingSequence[i];
	}
	//数据前面补15个0
	vector<complex<DataType>> datain;
	datain.resize(Data.size() + Short_Train_Symbols - 1);
	for (int i = 0; i < Short_Train_Symbols - 1; i++) {
		datain[i] = complex<DataType>(0, 0);
	}
	for (int i = 0; i < Data.size(); i++) {
		datain[i + Short_Train_Symbols - 1] = Data[i];
	}
	//一个数组 存储相关性计算累加和的结果
	int N = Data.size();
	vector<complex<DataType>> corr(N);
	//进行延迟相关计算
	for (int i = 0; i < N; i++) {
		corr[i] = complex<DataType>(0, 0);
		for (int j = 0; j < Short_Train_Symbols; j++) {
			corr[i] = corr[i] + datain[i + j] * conj(shortTrainingSequenceData[j]);
		}
	}
	//返回相关性计算的累加和
	return corr;
}
/***
 * 匹配滤波
 * 与本地短训练序列进行相关性计算 将相关值雷架求和
 * 然后寻找峰值，确定短训练符号的结束点
 * 注意这里传进来的data只是短训练序列的数据
 * 返回短训练序列的结束点
*/
int matchFilter(const vector<complex<DataType>> Data) {
	//量化接收到的数据
	vector<complex<DataType>> quantizedData = quantizeData(Data);
	//进行延迟相关计算
	vector<complex<DataType>> corr = STSCorrelatingAndAccumulate(quantizedData);
	//根据累加和计算幅值
	vector<DataType> corr_abs(corr.size());
	//简化计算，实数部分和虚数部分的绝对值之和
	for (int i = 0; i < corr.size(); i++) {
		corr_abs[i] = abs(corr[i].real()) + abs(corr[i].imag());
	}
	//打印一下相关性计算的结果 看看阈值应该设多少
	//for (int i = 0; i < corr_abs.size(); i++) {
	//	cout << i << "\t" << corr_abs[i] << endl;
	//}
	//找到峰值 9次，因为第一次不完整
	//第9个峰值出现的位值即是短训练序列的结束点
	int peakCount = 0;
	int peak = 0;
	for (int i = 0; i < corr_abs.size(); i++) {
		if (corr_abs[i] > Symbol_Sync_Threshold) {
			peakCount++;
		}
		if (peakCount == 9) {
			peak = i;
			break;
		}
	}
	return peak;
}

/***
 * 一个测试函数，看看能否把短训练序列的结束点找到
 * 
*/
void test_matchFilter() {
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
	int start = 100;
	vector<complex<DataType>> data = delay_corr_Group_Detection(ofdm_symbol,start);
	cout << "size:" << data.size() << endl;
	//分离数据
	vector<complex<DataType>> shortTrainingSequence = getShortTrainingSequence(data);
	//进行频率偏移估计
	DataType CFOEstimate = CFOEstimating(shortTrainingSequence);

	//打印
	cout << "CFOEstimate: " << CFOEstimate << endl;
	//对所有数据进行频率偏移补偿
	vector<complex<DataType>> data_compensation = CFOCompensation(data, CFOEstimate);
	//提取10个短训练序列
	vector<complex<DataType>> shortTrainingSequenceData = getShortTrainingSequence(data_compensation);
	vector<complex<DataType>> shortTrainingSequenceData1 = getShortTrainingSequenceRest(data_compensation);
	//两个拼接起来
	vector<complex<DataType>> shortTrainingSequenceData2;
	shortTrainingSequenceData2.resize(shortTrainingSequenceData.size() + shortTrainingSequenceData1.size());
	for (int i = 0; i < shortTrainingSequenceData.size(); i++) {
		shortTrainingSequenceData2[i] = shortTrainingSequenceData[i];
	}
	for (int i = 0; i < shortTrainingSequenceData1.size(); i++) {
		shortTrainingSequenceData2[i + shortTrainingSequenceData.size()] = shortTrainingSequenceData1[i];
	}
	//找到短训练序列的结束点
	int peak = matchFilter(shortTrainingSequenceData2);

	//打印
	cout << "peak:" << peak << endl;
	/***
	 * 可以看到分组模块的输出从108开始， 
	 * 数据其实从100开始
	 * 那么这个短训练序列的结束点应该是100+160 -1  = 260 -1  260-1 = 259
	 * 但分组模块少了8个数据 
	 * 也就是说短训练序列的结束点应该是259-8 = 251
	 * 这跟输出的peak = 151 + 100 = 251是一样的
	 * 表明该模块的功能是正确的
	 * 	start:108       end:426
		CFOEstimate:0
		peak:151
	 * 
	 * 
	*/
}

#endif