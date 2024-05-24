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





#endif